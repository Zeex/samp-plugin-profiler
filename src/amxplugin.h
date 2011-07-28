#ifndef AMXPLUGIN_H
#define AMXPLUGIN_H

#include "amx/amx.h"

#define SAMP_PLUGIN_VERSION 0x0200

#if defined __cplusplus
    #define PLUGIN_EXTERN_C extern "C"
#else
    #define PLUGIN_EXTERN_C 
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    #define PLUGIN_CALL __stdcall
    #define PLUGIN_EXPORT PLUGIN_EXTERN_C __declspec(dllexport)
#else
    #define PLUGIN_CALL
    #define PLUGIN_EXPORT PLUGIN_EXTERN_C
#endif

enum SUPPORTS_FLAGS {
    SUPPORTS_VERSION        = SAMP_PLUGIN_VERSION,
    SUPPORTS_VERSION_MASK   = 0xffff,
    SUPPORTS_AMX_NATIVES    = 0x10000,
    SUPPORTS_PROCESS_TICK   = 0x20000
};

PLUGIN_EXTERN_C void logprintf(char* format, ...);
PLUGIN_EXTERN_C void PluginInit(void **ppPluginData);

#endif
