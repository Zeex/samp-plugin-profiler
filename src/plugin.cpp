// Copyright (c) 2011-2014 Zeex
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

#include <subhook.h>
#include "logprintf.h"
#include "plugin.h"
#include "pluginversion.h"
#include "profiler.h"

typedef void (*logprintf_t)(const char *format, ...);

extern void *pAMXFunctions;

namespace hooks {

SubHook exec_hook;

int AMXAPI amx_Debug(AMX *amx) {
  return Profiler::GetInstance(amx)->Debug();
}

int AMXAPI amx_Callback(AMX *amx, cell index, cell *result, cell *params) {
  return Profiler::GetInstance(amx)->Callback(index, result, params);
}

int AMXAPI amx_Exec(AMX *amx, cell *retval, int index) {
  SubHook::ScopedRemove _(&exec_hook);
  if (amx->flags & AMX_FLAG_BROWSE) {
    return ::amx_Exec(amx, retval, index);
  }
  return Profiler::GetInstance(amx)->Exec(retval, index);
}

} // namespace hooks

template<typename Func>
static void *FunctionToVoidPtr(Func func) {
  return (void*)func;
}

static void *AMXAPI amx_Align_stub(void *v) {
  return v;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
  return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {
  pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
  logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];

  void **exports = reinterpret_cast<void**>(pAMXFunctions);

  exports[PLUGIN_AMX_EXPORT_Align16] = FunctionToVoidPtr(amx_Align_stub);
  exports[PLUGIN_AMX_EXPORT_Align32] = FunctionToVoidPtr(amx_Align_stub);
  exports[PLUGIN_AMX_EXPORT_Align64] = FunctionToVoidPtr(amx_Align_stub);

  hooks::exec_hook.SetSrc(exports[PLUGIN_AMX_EXPORT_Exec]);
  hooks::exec_hook.SetDst(FunctionToVoidPtr(hooks::amx_Exec));
  hooks::exec_hook.Install();

  logprintf("  Profiler v" PROJECT_VERSION_STRING " is OK.");
  return true;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
  Profiler::CreateInstance(amx);
  amx_SetDebugHook(amx, hooks::amx_Debug);
  amx_SetCallback(amx, hooks::amx_Callback);
  return AMX_ERR_NONE;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) {
  Profiler::DestroyInstance(amx);
  return AMX_ERR_NONE;
}
