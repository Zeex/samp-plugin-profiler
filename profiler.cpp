#include <malloc.h> // _alloca
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <map>

#include "amxprofiler.h"
#include "logprintf.h"
#include "plugin.h"

extern void *pAMXFunctions;

static std::map<AMX*, AMX_DBG> amxDebugInfo;
static std::map<AMX*, AMXProfiler*> amxProfilers;

namespace natives {
    
    // native Profiler_Start();
    cell AMX_NATIVE_CALL Profiler_Start(AMX *amx, cell *params) {
        amxProfilers[amx]->Run();
        return 1;
    }

    // native Profiler_Stop();
    cell AMX_NATIVE_CALL Profiler_Stop(AMX *amx, cell *params) {
        AMXProfiler *prof = amxProfilers[amx];

        prof->Terminate();

        std::ofstream outFile("profiler.txt");
        if (!outFile.is_open()) {
            return 0;
        }

        std::map<AMX*, AMX_DBG>::iterator it = amxDebugInfo.find(amx);
        if (it == amxDebugInfo.end()) {
            prof->PrintStats(outFile);
        } else {
            prof->PrintStats(outFile, &it->second);
        }

        return 1;
    }

    // native Profiler_SetScriptName(const name[]);
    cell AMX_NATIVE_CALL Profiler_SetScriptName(AMX *amx, cell *params) {
        char *name;
        amx_StrParam(amx, params[1], name);

        FILE *fp = fopen(name, "rb");
        if (fp != 0) {
            AMX_DBG amxdbg;
            if (dbg_LoadInfo(&amxdbg, fp) == AMX_ERR_NONE) {
                amxDebugInfo[amx] = amxdbg;
                return 1;
            }
            fclose(fp);
        }

        return 0;
    }

    const AMX_NATIVE_INFO all[] = { 
        {"Profiler_Start",         Profiler_Start},
        {"Profiler_Stop",          Profiler_Stop},
        {"Profiler_SetScriptName", Profiler_SetScriptName}, 
        {0,                        0}
    };
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **pluginData) {
    pAMXFunctions = pluginData[PLUGIN_DATA_AMX_EXPORTS];
    logprintf = (logprintf_t)pluginData[PLUGIN_DATA_LOGPRINTF];
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