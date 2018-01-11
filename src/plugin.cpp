// Copyright (c) 2011-2015 Zeex
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

#include <cstring>
#include <subhook.h>
#include "logprintf.h"
#include "natives.h"
#include "plugin.h"
#include "pluginversion.h"
#include "profiler.h"

extern void *pAMXFunctions;
static void *exports[PLUGIN_AMX_EXPORT_UTF8Put + 1];

static SubHook exec_hook;

static int AMXAPI amx_Debug_Profiler(AMX *amx) {
  Profiler *profiler = Profiler::GetInstance(amx);
  return profiler->Debug();
}

static int AMXAPI amx_Callback_Profiler(AMX *amx,
                                        cell index,
                                        cell *result,
                                        cell *params) {
  Profiler *profiler = Profiler::GetInstance(amx);
  return profiler->Callback(index, result, params);
}

static int AMXAPI amx_Exec_Profiler(AMX *amx, cell *retval, int index) {
  if (amx->flags & AMX_FLAG_BROWSE) {
    // Not an actual exec, just some internal AMX hack.
    return amx_Exec(amx, retval, index);
  } else {
    Profiler *profiler = Profiler::GetInstance(amx);
    return profiler->Exec(retval, index);
  }
}

static void *AMXAPI amx_Align_Profiler(void *v) {
  return v; // x86 is already little endian
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
  return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {
  void **real_exports = (void **)ppData[PLUGIN_DATA_AMX_EXPORTS];
  std::memcpy(exports, real_exports, sizeof(exports));

  exports[PLUGIN_AMX_EXPORT_Align16] = (void *)amx_Align_Profiler;
  exports[PLUGIN_AMX_EXPORT_Align32] = (void *)amx_Align_Profiler;
  exports[PLUGIN_AMX_EXPORT_Align64] = (void *)amx_Align_Profiler;

  exec_hook.Install(exports[PLUGIN_AMX_EXPORT_Exec], (void *)amx_Exec_Profiler);
  exports[PLUGIN_AMX_EXPORT_Exec] = exec_hook.GetTrampoline();

  pAMXFunctions = exports;
  logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];

  logprintf("  Profiler plugin " PROJECT_VERSION_STRING);
  return true;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
  Profiler *profiler = Profiler::CreateInstance(amx);

  int error = profiler->Load();
  if (error != AMX_ERR_NONE) {
    return error;
  }

  if (profiler->GetState() > PROFILER_DISABLED) {
    profiler->Start();

    amx_SetDebugHook(amx, amx_Debug_Profiler);
    amx_SetCallback(amx, amx_Callback_Profiler);

    // This should stop the VM from replacing SYSREQ.C instructions with
    // SYSREQ.D and allow us to profile native functions.
    amx->sysreq_d = 0;
  }

  return RegisterNatives(amx);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) {
  Profiler *profiler = Profiler::GetInstance(amx);

  int error = profiler->Unload();
  profiler->Stop();
  profiler->Dump();

  Profiler::DestroyInstance(amx);
  return error;
}
