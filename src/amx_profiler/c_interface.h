// AMX profiler for SA-MP server: http://sa-mp.com
//
// Copyright (C) 2011-2012 Sergey Zolotarev
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

#ifndef AMX_PROFILER_C_INTERFACE_H
#define AMX_PROFILER_C_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "amx/amx.h"
#include "amx/amxdbg.h"

int prof_Init(AMX *amx, AMX_DBG *amxdbg /* = NULL */);
int prof_Shutdown(AMX *amx);
int prof_DebugHook(AMX *amx);
int prof_Callback(AMX *amx, cell index, cell *result, cell *params);
int prof_Exec(AMX *amx, cell *retval, int index);

#ifdef __cplusplus
}
#endif

#endif // !AMX_PROFILER_C_INTERFACE_H
