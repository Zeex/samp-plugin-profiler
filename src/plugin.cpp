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
#include "jump.h"
#include "logprintf.h"
#include "plugin.h"
#include "printers.h"
#include "profiler.h"

#include "amx/amx.h"

using namespace samp_profiler;

extern void *pAMXFunctions; 

// Symbolic info, used for getting function names
static std::map<AMX*, DebugInfo> debugInfos;

// Both x86 and x86-64 are Little Endian...
static void *AMXAPI DummyAmxAlign(void *v) { return v; }

static uint32_t amx_Exec_addr;
static unsigned char amx_Exec_code[5];

static int AMXAPI Exec(AMX *amx, cell *retval, int index) {
	memcpy(reinterpret_cast<void*>(::amx_Exec_addr), ::amx_Exec_code, 5);

	int error = AMX_ERR_NONE;

	// Check if this script has a profiler attached to it
	Profiler *prof = Profiler::Get(amx);
	if (prof != 0) {
		error =  prof->Exec(retval, index);
	} else {
		error = amx_Exec(amx, retval, index);
	}

	SetJump(reinterpret_cast<void*>(::amx_Exec_addr), (void*)::Exec, ::amx_Exec_code);

	return error;
}

// Replaces back slashes with forward slashes
static std::string ToPortablePath(const std::string &path) {
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
static bool WantsProfiler(const std::string &amxName) {
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

	/// Read settings from server.cfg.
	/// This only works if they used the defalt directories for gamemodes and filterscripts.
	/// Someting like ../my_scripts/awesome_script.amx obviously won't work here.
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

	// Hook amx_Exec
	::amx_Exec_addr = reinterpret_cast<uint32_t>(((void**)pAMXFunctions)[PLUGIN_AMX_EXPORT_Exec]);
	SetJump(reinterpret_cast<void*>(::amx_Exec_addr), (void*)::Exec, ::amx_Exec_code);

	ConfigReader server_cfg("server.cfg");
	Profiler::SetSubstractChildTime(server_cfg.GetOption("profiler_substract_children", true));

	logprintf("  Profiler plugin "VERSION" is OK.");

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
		std::string output_format = 
			server_cfg.GetOption("profiler_output_format", std::string("html"));

		if (output_format == "html") {			
			HtmlPrinter printer(amx_name + "-profile.html", "Profile of " + amx_path);
			prof->PrintStats(printer);
		} else if (output_format == "text") {
			TextPrinter printer(amx_name + "-profile.txt", "Profile of " + amx_path);
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
