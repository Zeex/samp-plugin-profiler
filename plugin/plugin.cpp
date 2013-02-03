// Copyright (c) 2011-2012, Zeex
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <algorithm>
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#ifdef _WIN32
	#include <windows.h>
#endif
#include <amx/amx.h>
#include <amx_profiler/call_graph_writer_gv.h>
#include <amx_profiler/debug_info.h>
#include <amx_profiler/profile_writer_html.h>
#include <amx_profiler/profile_writer_text.h>
#include <amx_profiler/profile_writer_xml.h>
#include <amx_profiler/profiler.h>
#include "amx_path.h"
#include "config_reader.h"
#include "hook.h"
#include "plugin.h"
#include "version.h"

typedef void (*logprintf_t)(const char *format, ...);

extern void *pAMXFunctions;

static logprintf_t logprintf;

// Profiler instances.
static std::unordered_map<AMX*, std::shared_ptr<amx_profiler::Profiler>> profilers;

// List of loaded scripts, need this to fix AmxUnload bug on Windows.
static std::list<AMX*> loaded_scripts;

// Stores previously set debug hooks (if any).
static std::unordered_map<AMX*, AMX_DEBUG> old_debug_hooks;

// Plugin settings and their defauls.
namespace cfg {
	bool          profile_gamemode      = false;
	std::string   profile_filterscripts = "";
	std::string   profile_format        = "html";
	bool          call_graph            = false;
	std::string   call_graph_format     = "gv";
};

namespace hooks {

Hook amx_Exec_hook;
Hook amx_Callback_hook;

static int AMXAPI amx_Debug(AMX *amx) {
	auto profiler = ::profilers[amx];
	if (profiler) {
		profiler->amx_Debug();
	}
	auto iterator = old_debug_hooks.find(amx);
	if (iterator != old_debug_hooks.end()) {
		if (iterator->second != nullptr) {
			return (iterator->second)(amx);
		}
	}
	return AMX_ERR_NONE;
}

static int AMXAPI amx_Callback(AMX *amx, cell index, cell *result, cell *params) {
	Hook::ScopedRemove r(&amx_Callback_hook);
	Hook::ScopedInstall i(&amx_Exec_hook);

	auto profiler = ::profilers[amx];
	if (profiler) {
		return profiler->amx_Callback(index, result, params);
	} else {
		return ::amx_Callback(amx, index, result, params);
	}
}

static int AMXAPI amx_Exec(AMX *amx, cell *retval, int index) {
	Hook::ScopedRemove r(&amx_Exec_hook);
	Hook::ScopedInstall i(&amx_Callback_hook);

	auto profiler = ::profilers[amx];
	if (profiler) {
		return profiler->amx_Exec(retval, index);
	} else {
		return ::amx_Exec(amx, retval, index);
	}
}

} // namespace hooks

static std::string ToUnixPath(const std::string &path) {
	std::string fsPath = path;
	std::replace(fsPath.begin(), fsPath.end(), '\\', '/');
	return fsPath;
}

static bool IsGameMode(const std::string &amxName) {
	return ToUnixPath(amxName).find("gamemodes/") != std::string::npos;
}

static bool IsFilterScript(const std::string &amxName) {
	return ToUnixPath(amxName).find("filterscripts/") != std::string::npos;
}

static bool GetPublicVariable(AMX *amx, const char *name, cell &value) {
	cell amx_addr;
	if (amx_FindPubVar(amx, name, &amx_addr) == AMX_ERR_NONE) {
		cell *phys_addr;
		amx_GetAddr(amx, amx_addr, &phys_addr);
		value = *phys_addr;
		return true;
	}
	return false;
}

static bool WantsProfiler(const std::string &amxName) {
	std::string goodAmxName = ToUnixPath(amxName);

	if (IsGameMode(amxName)) {
		if (cfg::profile_gamemode) {
			return true;
		}
	} else if (IsFilterScript(amxName)) {
		std::string fsList = cfg::profile_filterscripts;
		std::stringstream fsStream(fsList);
		do {
			std::string fsName;
			fsStream >> fsName;
			if (goodAmxName == "filterscripts/" + fsName + ".amx"
					|| goodAmxName == "filterscripts/" + fsName) {
				return true;
			}
		} while (!fsStream.eof());
	}

	return false;
}

#ifdef _WIN32

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx);

static BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType) {
	switch (dwCtrlType) {
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
		for (auto amx : loaded_scripts) {
			AmxUnload(amx);
		}
	}
	return FALSE;
}

#endif

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

static void *AMXAPI my_amx_Align(void *v) { return v; }

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];

	((void**)pAMXFunctions)[PLUGIN_AMX_EXPORT_Align16] = (void*)my_amx_Align;
	((void**)pAMXFunctions)[PLUGIN_AMX_EXPORT_Align32] = (void*)my_amx_Align;
	((void**)pAMXFunctions)[PLUGIN_AMX_EXPORT_Align64] = (void*)my_amx_Align;

	hooks::amx_Exec_hook.Install(
		((void**)pAMXFunctions)[PLUGIN_AMX_EXPORT_Exec],
		(void*)hooks::amx_Exec);
	hooks::amx_Callback_hook.Install(
		((void**)pAMXFunctions)[PLUGIN_AMX_EXPORT_Callback],
		(void*)hooks::amx_Callback);

	#ifdef _WIN32
		SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
	#endif

	// Read plugin settings from server.cfg.
	ConfigReader server_cfg("server.cfg");
	cfg::profile_gamemode      = server_cfg.GetOption("profile_gamemode", cfg::profile_gamemode);
	cfg::profile_filterscripts = server_cfg.GetOption("profile_filterscripts", cfg::profile_filterscripts);
	cfg::profile_format        = server_cfg.GetOption("profile_format", cfg::profile_format);
	cfg::call_graph            = server_cfg.GetOption("call_graph", cfg::call_graph);
	cfg::call_graph_format     = server_cfg.GetOption("call_graph_format", cfg::call_graph_format);

	logprintf("  Profiler v" PLUGIN_VERSION_STRING " is OK.");

	return true;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
	::loaded_scripts.push_back(amx);

	std::string filename = GetAmxPath(amx);
	if (filename.empty()) {
		logprintf("[profiler] Can't find matching .amx file");
		return AMX_ERR_NONE;
	}

	cell profiler_enabled = false;
	if (GetPublicVariable(amx, "profiler_enabled", profiler_enabled)
			&& !profiler_enabled) {
		return AMX_ERR_NONE;
	}

	if (profiler_enabled || WantsProfiler(filename)) {
		// Disable SYSREQ.D
		amx->sysreq_d = 0;

		// Store previous debug hook somewhere before setting a new one
		::old_debug_hooks[amx] = amx->debug;
		amx_SetDebugHook(amx, hooks::amx_Debug);

		// Load debug stats if available
		amx_profiler::DebugInfo debug_info;
		if (amx_profiler::HasDebugInfo(amx)) {
			debug_info.Load(filename);
			if (debug_info.IsLoaded()) {
				logprintf("[profiler] Loaded debug stats from '%s'", filename.c_str());
			} else {
				logprintf("[profiler] Error loading debug stats from '%s'", filename.c_str());
			}
		}

		::profilers[amx] = std::shared_ptr<amx_profiler::Profiler>(new amx_profiler::Profiler(amx, debug_info, cfg::call_graph));
		if (debug_info.IsLoaded()) {
			logprintf("[profiler] Attached profiler to '%s'", filename.c_str());
		} else {
			logprintf("[profiler] Attached profiler to '%s' (no debug symbols)", filename.c_str());
		}
	}

	return AMX_ERR_NONE;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) {
	auto profiler = ::profilers[amx];

	if (profiler) {
		std::string amx_path = GetAmxPath(amx);
		std::string amx_name = std::string(amx_path, 0, amx_path.find_last_of("."));

		// Convert profile_format to lower case.
		std::transform(
			cfg::profile_format.begin(),
			cfg::profile_format.end(),
			cfg::profile_format.begin(),
			::tolower
		);

		auto profile_name = amx_name + "-profile." + cfg::profile_format;
		std::ofstream profile_stream(profile_name.c_str());

		if (profile_stream.is_open()) {
			amx_profiler::ProfileWriter *writer = nullptr;

			if (cfg::profile_format == "html") {
				writer = new amx_profiler::ProfileWriterHtml(&profile_stream, amx_path);
			} else if (cfg::profile_format == "txt") {
				writer = new amx_profiler::ProfileWriterText(&profile_stream, amx_path);
			} else if (cfg::profile_format == "xml") {
				writer = new amx_profiler::ProfileWriterXml(&profile_stream, amx_path);
			} else {
				logprintf("[profiler] Unknown output format '%s'", cfg::profile_format.c_str());
			}

			if (writer != nullptr) {
				logprintf("[profiler] Writing '%s'", profile_name.c_str());
				writer->set_print_date(true);
				writer->Write(profiler->GetStatistics());
				delete writer;
			}

			profile_stream.close();
		}

		if (cfg::call_graph) {
			auto call_graph_name = amx_name + "-calls.gv";
			std::ofstream call_graph_stream(call_graph_name.c_str());

			if (call_graph_stream.is_open()) {
				amx_profiler::CallGraphWriterGV *call_graph_writer = nullptr;

				if (cfg::call_graph_format == "gv") {
					call_graph_writer = new amx_profiler::CallGraphWriterGV(&call_graph_stream, amx_path, "SA-MP Server");
				} else {
					logprintf("[profiler] Unknown call graph format '%s'", cfg::call_graph_format.c_str());
				}

				if (call_graph_writer != nullptr) {
					logprintf("[profiler] Writing '%s'", call_graph_name.c_str());
					profiler->call_graph()->Write(call_graph_writer);
					delete call_graph_writer;
				}

				call_graph_stream.close();
			}
		}

		profilers.erase(amx);
	}

	return AMX_ERR_NONE;
}
