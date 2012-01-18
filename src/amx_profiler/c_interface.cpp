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

#include <memory>
#include <unordered_map>
#include "c_interface.h"
#include "debug_info.h"
#include "profiler.h"

using namespace amx_profiler;

static std::unordered_map<AMX*, Profiler*> profilers;

extern "C" int prof_Init(AMX *amx, AMX_DBG *amxdbg /* = NULL */) {
	if (::profilers.find(amx) != ::profilers.end()) {
		// Profiler already initialized
		return AMX_ERR_INIT;
	}
	if (amxdbg == nullptr) {
		::profilers[amx] = new Profiler(amx);
	} else {
		::profilers[amx] = new Profiler(amx, DebugInfo(amxdbg));
	}
	return AMX_ERR_NONE;
}

extern "C" int prof_Shutdown(AMX *amx) {
	::profilers.erase(amx);
	return AMX_ERR_NONE;
}

extern "C" int prof_DebugHook(AMX *amx) {
	auto iterator = ::profilers.find(amx);
	if (iterator != ::profilers.end()) {
		iterator->second->amx_Debug();
		return AMX_ERR_NONE;
	}
	// Profiler is not initialized
	return AMX_ERR_INIT;
}

extern "C" int prof_Callback(AMX *amx, cell index, cell *result, cell *params) {
	auto iterator = ::profilers.find(amx);
	if (iterator != ::profilers.end()) {
		iterator->second->amx_Callback(index, result, params);
		return AMX_ERR_NONE;
	}
	// Profiler is not initialized
	return AMX_ERR_INIT;
}

extern "C" int prof_Exec(AMX *amx, cell *retval, int index) {
	auto iterator = ::profilers.find(amx);
	if (iterator != ::profilers.end()) {
		iterator->second->amx_Exec(retval, index);
		return AMX_ERR_NONE;
	}
	// Profiler is not initialized
	return AMX_ERR_INIT;
}
