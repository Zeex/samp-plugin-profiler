// SA:MP Profiler plugin
//
// Copyright (c) 2011 Zeex
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
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "amxname.h"
#include "configreader.h"
#include "debuginfo.h"
#include "html_printer.h"
#include "jump-x86.h"
#include "logprintf.h"
#include "plugin.h"
#include "profiler.h"
#include "text_printer.h"
#include "version.h"

#include "amx/amx.h"

using namespace samp_profiler;

extern void *pAMXFunctions; 

namespace {

// Symbolic info, used for getting function names
std::map<AMX*, DebugInfo> debugInfos;

// x86 is Little Endian...
void *AMXAPI DummyAmxAlign(void *v) { return v; }

JumpX86 ExecHook;
JumpX86 CallbackHook;

int AMXAPI Exec(AMX *amx, cell *retval, int index) {
	//char name[32];
	//if (index == AMX_EXEC_MAIN) {
	//	strcpy(name, "main");
	//} else {
	//	amx_GetPublic(amx, index, name);
	//}
	//printf("%s\n", name);

	ExecHook.Remove();		
	CallbackHook.Install(); // P-code may call natives 

	// Return code
	int error = AMX_ERR_NONE;

	// Check if this script has a profiler attached to it
	Profiler *prof = Profiler::Get(amx);
	if (prof != 0) {
		error =  prof->Exec(retval, index);
	} else {
		error = amx_Exec(amx, retval, index);
	}

	CallbackHook.Remove();
	ExecHook.Install();

	return error;
}

int AMXAPI Callback(AMX *amx, cell index, cell *result, cell *params) {
	//char name[32];
	//amx_GetNative(amx, index, name);
	//printf("%s\n", name);

	CallbackHook.Remove();		
	ExecHook.Install(); // Natives may call amx_Exec() 

	// The default AMX callback (amx_Callback) can replace SYSREQ.C opcodes
	// with SYSREQ.D for better performance. 
	amx->sysreq_d = 0; 

	// Return code
	int error = AMX_ERR_NONE;

	// Check if this script has a profiler attached to it
	Profiler *prof = Profiler::Get(amx);
	if (prof != 0) {
		error =  prof->Callback(index, result, params);
	} else {
		error = amx_Callback(amx, index, result, params);
	}

	ExecHook.Remove();
	CallbackHook.Install();

	return error;
}

// Replaces back slashes with forward slashes
std::string ToPortablePath(const std::string &path) {
	std::string fsPath = path;
	std::replace(fsPath.begin(), fsPath.end(), '\\', '/');   
	return fsPath;
}

bool IsGameMode(const std::string &amxName) {
	return ToPortablePath(amxName).find("gamemodes/") != std::string::npos;
}

bool IsFilterScript(const std::string &amxName) {
	return ToPortablePath(amxName).find("filterscripts/") != std::string::npos;
}

// Returns true if the .amx should be profiled
bool WantsProfiler(const std::string &amxName) {
	std::string goodAmxName = ToPortablePath(amxName);

	/// Look at profiler.cfg
	/// It should be just a list of .amx files, one per line.
	std::ifstream config("plugins/profiler.cfg");    
	std::vector<std::string> filenames;
	std::transform(
		std::istream_iterator<std::string>(config), 
		std::istream_iterator<std::string>(),
		std::back_inserter(filenames), ToPortablePath
	);
	if (std::find(filenames.begin(), filenames.end(), 
			goodAmxName) != filenames.end()) {
		return true;
	}

	// This only works if they place their gamemodes and filterscripts in default directories.
	// Someting like ../my_scripts/awesome_script.amx obviously won't work.
	ConfigReader server_cfg("server.cfg");
	if (IsGameMode(amxName)) {
		// This is a gamemode
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

} // namespace

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];

	// The server does not export amx_Align* for some reason.
	// They are used in amxdbg.c and amxaux.c, so they must be callable.
	((void**)pAMXFunctions)[PLUGIN_AMX_EXPORT_Align16] = (void*)DummyAmxAlign; // amx_Align16
	((void**)pAMXFunctions)[PLUGIN_AMX_EXPORT_Align32] = (void*)DummyAmxAlign; // amx_Align32
	((void**)pAMXFunctions)[PLUGIN_AMX_EXPORT_Align64] = (void*)DummyAmxAlign; // amx_Align64

	ExecHook.Install(
		((void**)pAMXFunctions)[PLUGIN_AMX_EXPORT_Exec], 
		(void*)::Exec);
	CallbackHook.Install(
		((void**)pAMXFunctions)[PLUGIN_AMX_EXPORT_Callback], 
		(void*)::Callback);

	logprintf("  Profiler plugin "VERSION_STRING" is OK.");

	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
	return;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
	std::string filename = GetAmxName(amx);
	if (filename.empty()) {
		logprintf("Profiler: Failed to detect .amx name, prifiling will not be done");
		return AMX_ERR_NONE;
	}

	if (!Profiler::IsScriptProfilable(amx)) {
		logprintf("Profiler: Can't profile script %s (are you using -d0?)", filename.c_str());
		return AMX_ERR_NONE;
	}
	 
	if (WantsProfiler(filename)) {
		if (DebugInfo::HasDebugInfo(amx)) {
			DebugInfo debugInfo;
			debugInfo.Load(filename);
			if (debugInfo.IsLoaded()) {
				logprintf("Profiler: Loaded debug info from %s", filename.c_str());
				::debugInfos[amx] = debugInfo;				
				Profiler::Attach(amx, debugInfo); 
				logprintf("Profiler: Attached profiler instance to %s", filename.c_str());
				return AMX_ERR_NONE;
			} else {
				logprintf("Profiler: Error loading debug info from %s", filename.c_str());
			}
		}		
		Profiler::Attach(amx);
		logprintf("Profiler: Attached profiler instance to %s (no debug symbols)", filename.c_str());
	} 	

	return AMX_ERR_NONE;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) {
	// Get an instance of Profiler attached to the unloading AMX
	Profiler *prof = Profiler::Get(amx);

	// Detach profiler
	if (prof != 0) {
		std::string amx_path = GetAmxName(amx);
		std::string amx_name = std::string(amx_path, 0, amx_path.find_last_of("."));

		// Output stats depending on currently set output_format
		ConfigReader server_cfg("server.cfg");

		bool subtract_child_time = 
			server_cfg.GetOption("profiler_subtract_child_time", true);
		std::string sort_output_by = 
			server_cfg.GetOption("profiler_sort_output_by", std::string("time"));
		std::string output_format = 
			server_cfg.GetOption("profiler_output_format", std::string("html"));

		Printer::OutputSortMode sort_mode;
		if (sort_output_by == "calls") {
			sort_mode = Printer::SORT_BY_CALLS;
		} else if (sort_output_by == "time") {
			if (subtract_child_time) {
				sort_mode = Printer::SORT_BY_TIME;
			} else {
				sort_mode = Printer::SORT_BY_TOTAL_TIME;
			}
		}

		if (output_format == "html") {			
			HtmlPrinter printer(amx_name + "-profile.html", amx_path, subtract_child_time, sort_mode);
			prof->PrintStats(printer);
		} else if (output_format == "text") {
			TextPrinter printer(amx_name + "-profile.txt", amx_path, subtract_child_time, sort_mode);
			prof->PrintStats(printer);
		}

		Profiler::Detach(amx);
	}

	// Free debug info
	std::map<AMX*, DebugInfo>::iterator it = ::debugInfos.find(amx);
	if (it != ::debugInfos.end()) {
		it->second.Free();
		::debugInfos.erase(it);
	}

	return AMX_ERR_NONE;
}
