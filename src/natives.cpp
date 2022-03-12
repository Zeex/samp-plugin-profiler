// Copyright (c) 2014-2018 Zeex
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

#include "natives.h"
#include "profilerhandler.h"

namespace {

cell AMX_NATIVE_CALL Profiler_GetState(AMX *amx, cell *params) {
  return static_cast<cell>(ProfilerHandler::GetHandler(amx)->GetState());
}

cell AMX_NATIVE_CALL Profiler_Start(AMX *amx, cell *params) {
  return ProfilerHandler::GetHandler(amx)->Start();
}

cell AMX_NATIVE_CALL Profiler_Stop(AMX *amx, cell *params) {
  return ProfilerHandler::GetHandler(amx)->Stop();
}

cell AMX_NATIVE_CALL Profiler_Dump(AMX *amx, cell *params) {
  return ProfilerHandler::GetHandler(amx)->Dump();
}

const AMX_NATIVE_INFO natives[] = {
  { "Profiler_GetState", Profiler_GetState },
  { "Profiler_Start",    Profiler_Start },
  { "Profiler_Stop",     Profiler_Stop },
  { "Profiler_Dump",     Profiler_Dump }
};

} // anonymous namespace

int RegisterNatives(AMX *amx) {
  std::size_t num_natives = sizeof(natives) / sizeof(AMX_NATIVE_INFO);
  return amx_Register(amx, natives, static_cast<int>(num_natives));
}
