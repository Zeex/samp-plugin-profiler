// Copyright (c) 2011-2021 Zeex
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

#include <cstdlib>
#include <cstring>
#include <functional>
#include <subhook.h>
#ifdef _WIN32
  #include <windows.h>
#else
  #include <stdio.h>
#endif
#include "amxpathfinder.h"
#include "fileutils.h"
#include "logprintf.h"
#include "natives.h"
#include "plugin.h"
#include "pluginversion.h"
#include "profilerhandler.h"
#include "stringutils.h"

extern void *pAMXFunctions;

namespace {

// Holds pointers to AMX functions. Usually setting pAMXFunctions is enough
// but we want to replace amx_Align* functions without own versions.
void *exports[PLUGIN_AMX_EXPORT_UTF8Put + 1];

// amx_Exec() hook. This hook is used to intercept calls to public functions.
subhook::Hook exec_hook;

// Path to the last loaded AMX file. This is used to make a connection between
// *.amx files and their corresponding AMX instances.
std::string last_amx_path;

// Stores paths to loaded AMX files and is able to find a path by a pointer to
// an AMX instance.
AMXPathFinder amx_path_finder;

#ifdef _WIN32
  subhook::Hook create_file_hook;

  HANDLE WINAPI CreateFileAHook(
      _In_ LPCSTR lpFileName,
      _In_ DWORD dwDesiredAccess,
      _In_ DWORD dwShareMode,
      _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
      _In_ DWORD dwCreationDisposition,
      _In_ DWORD dwFlagsAndAttributes,
      _In_opt_ HANDLE hTemplateFile)
  {
    subhook::ScopedHookRemove _(&create_file_hook);
    const char *ext = fileutils::GetFileExtensionPtr(lpFileName);
    if (ext != 0 && stringutils::CompareIgnoreCase(ext, "amx") == 0) {
      last_amx_path = lpFileName;
    }
    return CreateFileA(
      lpFileName,
      dwDesiredAccess,
      dwShareMode,
      lpSecurityAttributes,
      dwCreationDisposition,
      dwFlagsAndAttributes,
      hTemplateFile);
  }
#else
  subhook::Hook fopen_hook;

  FILE *FopenHook(const char *filename, const char *mode) {
    subhook::ScopedHookRemove _(&fopen_hook);
    const char *ext = fileutils::GetFileExtensionPtr(filename);
    if (ext != 0 && stringutils::CompareIgnoreCase(ext, "amx") == 0) {
      last_amx_path = filename;
    }
    return fopen(filename, mode);
  }
#endif

int AMXAPI amx_Debug_Profiler(AMX *amx) {
  ProfilerHandler *profiler = ProfilerHandler::GetHandler(amx);
  return profiler->Debug();
}

int AMXAPI amx_Callback_Profiler(AMX *amx,
                                        cell index,
                                        cell *result,
                                        cell *params) {
  ProfilerHandler *profiler = ProfilerHandler::GetHandler(amx);
  return profiler->Callback(index, result, params);
}

int AMXAPI amx_Exec_Profiler(AMX *amx, cell *retval, int index) {
  if (amx->flags & AMX_FLAG_BROWSE) {
    // Not an actual exec, just some internal AMX hack.
    return amx_Exec(amx, retval, index);
  } else {
    ProfilerHandler *profiler = ProfilerHandler::GetHandler(amx);
    return profiler->Exec(retval, index);
  }
}

void *AMXAPI amx_Align_Profiler(void *v) {
  return v; // x86 is already little endian
}

} // anonymous namespace

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
  return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {
  void **real_exports = (void **)ppData[PLUGIN_DATA_AMX_EXPORTS];
  std::memcpy(exports, real_exports, sizeof(exports));

  exports[PLUGIN_AMX_EXPORT_Align16] = (void *)amx_Align_Profiler;
  exports[PLUGIN_AMX_EXPORT_Align32] = (void *)amx_Align_Profiler;
  exports[PLUGIN_AMX_EXPORT_Align64] = (void *)amx_Align_Profiler;

  exec_hook.Install(exports[PLUGIN_AMX_EXPORT_Exec],
                    (void *)amx_Exec_Profiler);
  exports[PLUGIN_AMX_EXPORT_Exec] = exec_hook.GetTrampoline();

  pAMXFunctions = exports;
  logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];

  #if _WIN32
    create_file_hook.Install((void*)CreateFileA, (void*)CreateFileAHook);
  #else
    fopen_hook.Install((void*)fopen, (void*)FopenHook);
  #endif

  amx_path_finder.AddSearchPath("gamemodes");
  amx_path_finder.AddSearchPath("filterscripts");

  const char *amx_path_var = getenv("AMX_PATH");
  if (amx_path_var != 0) {
    stringutils::SplitString(
      amx_path_var,
      fileutils::kNativePathListSepChar,
      std::bind1st(std::mem_fun(&AMXPathFinder::AddSearchPath),
                   &amx_path_finder));
  }

  logprintf("  Profiler plugin " PLUGIN_VERSION_STRING);
  return true;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
  if (last_amx_path.length() != 0) {
    amx_path_finder.AddKnownFile(amx, last_amx_path);
  }

  ProfilerHandler *profiler = ProfilerHandler::CreateHandler(amx);
  profiler->set_amx_path_finder(&amx_path_finder);

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
  ProfilerHandler *profiler = ProfilerHandler::GetHandler(amx);

  int error = profiler->Unload();
  profiler->Stop();
  profiler->Dump();

  ProfilerHandler::DestroyHandler(amx);
  return error;
}
