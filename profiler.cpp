#include <malloc.h> // _alloca
#include <stdlib.h>

#include <algorithm>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <numeric>
#include <map>

#include "amxprofiler.h"
#include "logprintf.h"
#include "plugin.h"
#include "version.h"

extern void *pAMXFunctions;

static std::map<AMX*, AMX_DBG> amxDebugInfo;
static std::map<AMX*, AMXProfiler*> amxProfilers;

static bool ByExecutionTime(const AMXProfilerStat &op1, const AMXProfilerStat &op2) {
    return op1.executionTime > op2.executionTime;
}

static bool ByNumberOfCalls(const AMXProfilerStat &op1, const AMXProfilerStat &op2) {
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
    
    // native Profiler_Start(const path_to_amx[]);
    cell AMX_NATIVE_CALL Profiler_Start(AMX *amx, cell *params) {
        char *path_to_amx;
        amx_StrParam(amx, params[1], path_to_amx);

        FILE *fp = fopen(path_to_amx, "rb");
        if (fp == 0) {
            return 0;
        }

        AMX_DBG amxdbg;
        if (dbg_LoadInfo(&amxdbg, fp) != AMX_ERR_NONE) {
            fclose(fp);
            return 0;
        }

        amxDebugInfo[amx] = amxdbg;
        fclose(fp);

        amxProfilers[amx]->Run();

        return 1;
    }

    // native Profiler_Stop();
    cell AMX_NATIVE_CALL Profiler_Stop(AMX *amx, cell *params) {
        AMXProfiler *prof = amxProfilers[amx];

        if (!prof->IsRunning()) {
            return 0;
        }

        prof->Terminate();
        return 1;
    }

    // native Profiler_PrintStats(const filename[]);
    cell AMX_NATIVE_CALL Profiler_PrintStats(AMX *amx, cell *params) {
        AMXProfiler *prof = amxProfilers[amx];

        if (prof->IsRunning()) { 
            return 0;
        }

        char *filename;
        amx_StrParam(amx, params[1], filename);

        std::ofstream stream(filename);
        if (!stream.is_open()) {
            return 0;
        }

        AMX_DBG amxdbg = ::amxDebugInfo[amx];

        std::vector<AMXProfilerStat> v = prof->GetStats();
        std::sort(v.begin(), v.end(), ByExecutionTime);

        // Table header
        stream << "<table>\n"
               << "\t<tr>\n"
               << "\t\t<td>Function</td>\n"
               << "\t\t<td>No. calls</td>\n"
               << "\t\t<td>Time per call</td>\n"
               << "\t\t<td>Overall time, %</td>\n"
               << "\t</tr>\n";

        // Calculate overall execution time
        uint64_t totalTime = 0;
        for (std::vector<AMXProfilerStat>::iterator it = v.begin(); it != v.end(); ++it) {
            totalTime += it->executionTime;
        }

        for (std::vector<AMXProfilerStat>::iterator it = v.begin(); it != v.end(); ++it)
        {
            if (it->numberOfCalls <= 0) {
                continue;
            }

            stream << "\t<tr>\n";

            if (it->native) {
                stream << "\t\t<td>" << FindNativeByIndex(amx, it->address) << "</td>\n";
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
                                 << it->executionTime * 1.0 / it->numberOfCalls << "</td>\n"
                   << "\t\t<td>" << std::setprecision(4)
                                 << it->executionTime * 100.0 / totalTime << "</td>\n";
            stream << "\t</tr>\n";
        }
        stream << "</table>\n";

        return 1;
    }

    const AMX_NATIVE_INFO all[] = { 
        {"Profiler_Start", Profiler_Start},
        {"Profiler_Stop",  Profiler_Stop},
        {"Profiler_Print", Profiler_PrintStats},
        {0,                0}
    };
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **pluginData) {
    pAMXFunctions = pluginData[PLUGIN_DATA_AMX_EXPORTS];
    logprintf = (logprintf_t)pluginData[PLUGIN_DATA_LOGPRINTF];

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