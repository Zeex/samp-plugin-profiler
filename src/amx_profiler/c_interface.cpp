// Copyright (c) 2011-2012, Zeex
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met: 
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer. 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// // LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
