// AMX profiler for SA-MP server: http://sa-mp.com
//
// Copyright (C) 2011-2012 Sergey Zolotarev
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <list>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

#ifdef _WIN32
	#include <Windows.h>
#endif

#include <amx_profiler/debug_info.h>
#include <amx_profiler/html_profile_writer.h>
#include <amx_profiler/profiler.h>
#include <amx_profiler/text_profile_writer.h>
#include <amx_profiler/xml_profile_writer.h>

#include "amx_name.h"
#include "config_reader.h"
#include "jump-x86.h"
#include "plugin.h"
#include "version.h"

using namespace amx_profiler;

typedef void (*logprintf_t)(const char *format, ...);

extern void *pAMXFunctions;

static logprintf_t logprintf;

// Profiler instances
static std::unordered_map<AMX*, std::shared_ptr<Profiler>> profilers;

// List of loaded scripts, need this to fix AmxUnload bug on Windows
static std::list<AMX*> loaded_scripts;

// Stores previously set debug hooks (if any)
static std::unordered_map<AMX*, AMX_DEBUG> old_debug_hooks;

namespace hooks {

JumpX86 amx_Exec_hook;
JumpX86 amx_Callback_hook;

static int AMXAPI amx_Debug(AMX *amx) {
	auto profiler = ::profilers[amx];
	if (profiler) {
		profiler->amx_Debug();
	}
	auto iterator = old_debug_hooks.find(amx);
	if (iterator != old_debug_hooks.end()) {
		if (iterator->second != 0) {
			return (iterator->second)(amx);
		}
	}
	return AMX_ERR_NONE;
}

static int AMXAPI amx_Callback(AMX *amx, cell index, cell *result, cell *params) {
	amx_Callback_hook.Remove();
	amx_Exec_hook.Install();

	int error = AMX_ERR_NONE;

	auto profiler = ::profilers[amx];
	if (profiler) {
		error =  profiler->amx_Callback(index, result, params);
	} else {
		error = ::amx_Callback(amx, index, result, params);
	}

	amx_Exec_hook.Remove();
	amx_Callback_hook.Install();

	return error;
}

static int AMXAPI amx_Exec(AMX *amx, cell *retval, int index) {
	amx_Exec_hook.Remove();
	amx_Callback_hook.Install();

	int error = AMX_ERR_NONE;

	auto profiler = ::profilers[amx];
	if (profiler) {
		error =  profiler->amx_Exec(retval, index);
	} else {
		error = ::amx_Exec(amx, retval, index);
	}

	amx_Callback_hook.Remove();
	amx_Exec_hook.Install();

	return error;
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

	ConfigReader server_cfg("server.cfg");
	if (IsGameMode(amxName)) {
		if (server_cfg.GetOption("profile_gamemode", false)) {
			return true;
		}
	} else if (IsFilterScript(amxName)) {
		std::string fsList = server_cfg.GetOption("profile_filterscripts", std::string(""));
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
		for (std::list<AMX*>::const_iterator iterator = ::loaded_scripts.begin();
				iterator != ::loaded_scripts.end(); ++iterator) {
			AmxUnload(*iterator);
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

	logprintf("  Profiler v"PROFILER_VERSION" is OK.");

	return true;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
	::loaded_scripts.push_back(amx);

	std::string filename = GetAmxName(amx);
	if (filename.empty()) {
		logprintf("[profiler] Can't find matching .amx file");
		return AMX_ERR_NONE;
	}

	if (!IsAmxProfilable(amx)) {
		logprintf("[profiler] Can't profile '%s' (are you using -d0?)", filename.c_str());
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

		// Load debug info if available
		if (HasDebugInfo(amx)) {
			DebugInfo debug_info;
			debug_info.Load(filename);
			if (debug_info.IsLoaded()) {
				logprintf("[profiler] Loaded debug info from '%s'", filename.c_str());
				::profilers[amx] = std::shared_ptr<Profiler>(new Profiler(amx, debug_info));
				logprintf("[profiler] Attached profiler to '%s'", filename.c_str());
				return AMX_ERR_NONE;
			} else {
				logprintf("[profiler] Error loading debug info from '%s'", filename.c_str());
			}
		}

		// No debug info loaded
		::profilers[amx] = std::shared_ptr<Profiler>(new Profiler(amx));
		logprintf("[profiler] Attached profiler to '%s' (no debug symbols)", filename.c_str());
	}

	return AMX_ERR_NONE;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) {
	auto profiler = ::profilers[amx];

	if (profiler) {
		std::string amx_path = GetAmxName(amx);
		std::string amx_name = std::string(amx_path, 0, amx_path.find_last_of("."));

		ConfigReader server_cfg("server.cfg");

		std::string format =
			server_cfg.GetOption("profile_format", std::string("html"));
		std::transform(format.begin(), format.end(), format.begin(), ::tolower);

		std::string filename = amx_name + "-profile";
		ProfileWriter *writer = 0;

		if (format == "html") {
			filename += ".html";
			writer = new HtmlProfileWriter;
		} else if (format == "text") {
			filename += ".txt";
			writer = new TextProfileWriter;
		} else if (format == "xml") {
			filename += ".xml";
			writer = new XmlProfileWriter;
		} else {
			logprintf("[profiler] Unknown output format '%s'", format.c_str());
		}

		if (writer != 0) {
			std::ofstream ostream(filename.c_str());
			profiler->WriteProfile(amx_path, writer, ostream);
			delete writer;
		}

		profilers.erase(amx);
	}

	return AMX_ERR_NONE;
}
