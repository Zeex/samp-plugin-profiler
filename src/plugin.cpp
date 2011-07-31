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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <list>
#include <string>

#include "amxnamefinder.h"
#include "amxplugin.h"
#include "amxprofiler.h"
#include "jump.h"

#define AMX_EXPORTS_ALIGN16 0
#define AMX_EXPORTS_ALIGN32 1
#define AMX_EXPORTS_ALIGN64 2
#define AMX_EXPORTS_EXEC    7

extern "C" void **amx_exports; // defined in amxplugin.asm

static std::list<std::string> profiledScripts;

// Both x86 and x86-64 are Little Endian...
static void *AMXAPI DummyAmxAlign(void *v) { return v; }

static uint32_t amx_Exec_addr;
static unsigned char amx_Exec_code[5];

static int AMXAPI Exec(AMX *amx, cell *retval, int index) {
    memcpy(reinterpret_cast<void*>(::amx_Exec_addr), ::amx_Exec_code, 5);

    int error;

    // Check if this script has a profiler attached to it.
    AmxProfiler *prof = AmxProfiler::Get(amx);
    if (prof != 0) {
        error =  prof->Exec(retval, index);
    } else {
        error = amx_Exec(amx, retval, index);
    }

    SetJump(reinterpret_cast<void*>(::amx_Exec_addr), (void*)::Exec, ::amx_Exec_code);

    return error;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **pluginData) {
    PluginInit(pluginData);

    // The server does not export amx_Align* for some reason.
    // They are used in amxdbg.c and amxaux.c, so they must be callable.
    amx_exports[AMX_EXPORTS_ALIGN16] = (void*)DummyAmxAlign; // amx_Align16
    amx_exports[AMX_EXPORTS_ALIGN32] = (void*)DummyAmxAlign; // amx_Align32
    amx_exports[AMX_EXPORTS_ALIGN64] = (void*)DummyAmxAlign; // amx_Align64

    // Hook amx_Exec
    //::amx_Exec_addr = reinterpret_cast<uint32_t>(amx_exports[AMX_EXPORTS_EXEC]);
    //SetJump(reinterpret_cast<void*>(::amx_Exec_addr), (void*)::Exec, ::amx_Exec_code);

    // Get the names of scripts to be profiled.
    std::ifstream config("plugins/profiler.cfg");
    std::copy(std::istream_iterator<std::string>(config), 
              std::istream_iterator<std::string>(), 
              std::back_inserter(::profiledScripts));

    // Add SA:MP default directories to the .amx finder sarch path.
    AmxNameFinder *nameFinder = AmxNameFinder::GetInstance();
    nameFinder->AddSearchDir("gamemodes");
    nameFinder->AddSearchDir("filterscripts");
    nameFinder->UpdateCache();

    return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
    return;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
    AmxNameFinder *nameFinder = AmxNameFinder::GetInstance();
    nameFinder->UpdateCache(); 

    // If the name is established, load symbolic info.
    std::string filename = nameFinder->GetAmxName(amx);
    if (!filename.empty()) {
        std::replace(filename.begin(), filename.end(), '\\', '/');    
        if (std::find(::profiledScripts.begin(), 
                      ::profiledScripts.end(), filename) != ::profiledScripts.end()) 
        {
            FILE *fp = fopen(filename.c_str(), "rb");
            if (fp != 0) {
                AMX_DBG amxdbg;
                int error = dbg_LoadInfo(&amxdbg, fp);
                if (error == AMX_ERR_NONE) {
                    AmxProfiler::Attach(amx, amxdbg);              
                } else {
                    // An error occured, no profiler attached.
                    return error;
                }
                fclose(fp);
            } 
        }
    }

    return AMX_ERR_NONE;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) {
    // Get an instance of AmxProfiler attached to the unloading AMX.
    AmxProfiler *prof = AmxProfiler::Get(amx);

    // Detach profiler
    if (prof != 0) {
        // Before doing that, print stats to <amx_file_path>.prof.
        std::string name = AmxNameFinder::GetInstance()->GetAmxName(amx);
        if (!name.empty()) {
            prof->PrintStats(name + std::string(".prof"));
        }
        AmxProfiler::Detach(amx);
    }

    return AMX_ERR_NONE;
}
