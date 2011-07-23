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
#include <iomanip>
#include <iostream>
#include <fstream>
#include <numeric>
#include <map>

#include <malloc.h> // _alloca
#include <stdlib.h>

#include "amxprofiler.h"
#include "logprintf.h"
#include "plugin.h"
#include "version.h"

#include "amx/amx.h"
#include "amx/amxdbg.h"

extern void *pAMXFunctions;

static std::map<AMX*, AMX_DBG> amxDebugInfo;
static std::map<AMX*, AMXProfiler*> amxProfilers;

static bool ByExecutionTime(const AMXFunPerfStats &op1, const AMXFunPerfStats &op2) {
    return op1.executionTime > op2.executionTime;
}

static bool ByNumberOfCalls(const AMXFunPerfStats &op1, const AMXFunPerfStats &op2) {
    return op1.numberOfCalls > op2.numberOfCalls;
}

static const char *FindNativeByIndex(AMX *amx, cell index) {
    AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx->base);

    AMX_FUNCSTUBNT *natives = reinterpret_cast<AMX_FUNCSTUBNT*>(amx->base + hdr->natives);
    int numNatives = (hdr->libraries - hdr->natives) / hdr->defsize;

    for (int i = 0; i < numNatives; i++) {
        if (i == index) {
            return reinterpret_cast<char*>(amx->base + natives[i].nameofs);
        }
    }

    return 0;
}

namespace natives {
    
    // native Profiler_Init(const path_to_amx[]);
    cell AMX_NATIVE_CALL Profiler_Init(AMX *amx, cell *params) {
        char *path_to_amx;
        amx_StrParam(amx, params[1], path_to_amx);

        FILE *fp = fopen(path_to_amx, "rb");
        if (fp == 0) {
            logprintf("Profiler: Error opening file %s", path_to_amx);
            return 0;
        }

        AMX_DBG amxdbg;
        if (dbg_LoadInfo(&amxdbg, fp) != AMX_ERR_NONE) {
            logprintf("Profiler: Error loading symbols from %s", path_to_amx);
            fclose(fp);
            return 0;
        }

        amxDebugInfo[amx] = amxdbg;
        fclose(fp);

        logprintf("Initialized profiler in %s", path_to_amx);

        return 1;
    }

    // native Profiler_Start();
    cell AMX_NATIVE_CALL Profiler_Start(AMX *amx, cell *params) {
        AMXProfiler *prof = amxProfilers[amx];

        if (!prof->IsRunning()) {
            amxProfilers[amx]->Run();
            return 1;
        }

        return 0;
    }

    // native Profiler_Stop();
    cell AMX_NATIVE_CALL Profiler_Stop(AMX *amx, cell *params) {
        AMXProfiler *prof = amxProfilers[amx];

        if (prof->IsRunning()) {
            prof->Terminate();
            return 1;
        } 
        
        return 0;
    }

    // native Profiler_PrintStats(const filename[]);
    cell AMX_NATIVE_CALL Profiler_PrintStats(AMX *amx, cell *params) {
        AMXProfiler *prof = amxProfilers[amx];

        if (prof->IsRunning()) { 
            return 0;
        }

        std::map<AMX*, AMX_DBG>::iterator dbgIter = ::amxDebugInfo.find(amx);
        if (dbgIter == ::amxDebugInfo.end()) {
            return 0;
        }
        AMX_DBG amxdbg = dbgIter->second;

        char *filename;
        amx_StrParam(amx, params[1], filename);

        std::ofstream stream(filename);
        if (!stream.is_open()) {
            return 0;
        }

        std::vector<AMXFunPerfStats> v = prof->GetStats();
        std::sort(v.begin(), v.end(), ByExecutionTime);

        stream << "<table>\n"
               << "\t<tr>\n"
               << "\t\t<td>Function</td>\n"
               << "\t\t<td>Calls</td>\n"
               << "\t\t<td>Time per call</td>\n"
               << "\t\t<td>Overall time</td>\n"
               << "\t\t<td>Overall time, %</td>\n"
               << "\t</tr>\n";

        platformstl::int64_t totalTime = 0;
        for (std::vector<AMXFunPerfStats>::iterator it = v.begin(); it != v.end(); ++it) {
            totalTime += it->executionTime;
        }        

        for (std::vector<AMXFunPerfStats>::iterator it = v.begin(); it != v.end(); ++it)
        {
            stream << "\t<tr>\n";

            if (it->native) {
                const char *name = FindNativeByIndex(amx, it->address);
                if (name != 0) {
                    stream << "\t\t<td>" << name << "</td>\n";
                }
            } else {
                const char *name;
                if (dbg_LookupFunction(&amxdbg, it->address, &name) == AMX_ERR_NONE) {
                    stream << "\t\t<td>" << name << "</td>\n";
                } else {
                    stream << "\t\t<td>" << std::hex << it->address << std::dec << "</td>\n";
                }
            }
            stream << "\t\t<td>" << it->numberOfCalls << "</td>\n"
                   << "\t\t<td>" << std::fixed << std::setprecision(0)
                                 << static_cast<double>(it->executionTime) / 
                                        static_cast<double>(it->numberOfCalls) << "</td>\n"
                   << "\t\t<td>" << it->executionTime << "</td>\n"
                   << "\t\t<td>" << std::setprecision(2)
                                 << static_cast<double>(it->executionTime * 100) / 
                                        static_cast<double>(totalTime) << "</td>\n";
            stream << "\t</tr>\n";
        }
        stream << "</table>\n";

        return 1;
    }

    const AMX_NATIVE_INFO all[] = { 
        {"Profiler_Init",       Profiler_Init},
        {"Profiler_Start",      Profiler_Start},
        {"Profiler_Stop",       Profiler_Stop},
        {"Profiler_PrintStats", Profiler_PrintStats},
        {0,                     0}
    };
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **pluginData) {
    pAMXFunctions = pluginData[PLUGIN_DATA_AMX_EXPORTS];
    logprintf = (logprintf_t)pluginData[PLUGIN_DATA_LOGPRINTF];

    extern uint16_t * AMXAPI amx_Align16_(uint16_t *v);
    ((void**)pAMXFunctions)[PLUGIN_AMX_EXPORT_Align16] = (void*)amx_Align16_;
    extern uint32_t * AMXAPI amx_Align32_(uint32_t *v);
    ((void**)pAMXFunctions)[PLUGIN_AMX_EXPORT_Align32] = (void*)amx_Align32_;

    logprintf("  Profiler plugin " VERSION " (built on " __DATE__ ")");  

    return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
    return;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
    amxProfilers[amx] = new AMXProfiler(amx);
    return amx_Register(amx, natives::all, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) {
    delete amxProfilers[amx];
    return AMX_ERR_NONE;
}
