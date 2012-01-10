// AMX profiler for SA-MP server: http://sa-mp.com
//
// Copyright (C) 2011 Sergey Zolotarev
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

#include <cassert>
#include <cstdio>

#include "amx/amx.h"
#include "amx/amxdbg.h"

#include "debug_info.h"

namespace samp_profiler {

DebugInfo::DebugInfo() {}

DebugInfo::DebugInfo(const std::string &filename) {
	Load(filename);
}

bool DebugInfo::HasDebugInfo(AMX *amx) {
	uint16_t flags;
	amx_Flags(amx, &flags);
	return ((flags & AMX_FLAG_DEBUG) != 0);
}

void DebugInfo::FreeAmxDbg(AMX_DBG *amxdbg) {
	if (amxdbg != 0) {
		dbg_FreeInfo(amxdbg);
		delete amxdbg;
	}
}

bool DebugInfo::IsLoaded() const {
	return (amxdbgPtr_.get() != 0);
}

void DebugInfo::Load(const std::string &filename) {
	std::FILE* fp = std::fopen(filename.c_str(), "rb");
	if (fp != 0) {
		amxdbgPtr_.reset(new AMX_DBG, FreeAmxDbg);
		if (dbg_LoadInfo(amxdbgPtr_.get(), fp) != AMX_ERR_NONE) 
			amxdbgPtr_.reset();
		fclose(fp);
	}
}

void DebugInfo::Free() {
	amxdbgPtr_.reset();
}

long DebugInfo::GetLine(cell address) const {
	long line = 0;
	dbg_LookupLine(amxdbgPtr_.get(), address, &line);
	return line;
}

std::string DebugInfo::GetFile(cell address) const {
	std::string result;
	const char *file;
	if (dbg_LookupFile(amxdbgPtr_.get(), address, &file) == AMX_ERR_NONE)
		result.assign(file);
	return result;
}

std::string DebugInfo::GetFunction(cell address) const {
	std::string result;
	const char *function;
	if (dbg_LookupFunction(amxdbgPtr_.get(), address, &function) == AMX_ERR_NONE)
		result.assign(function);
	return result;
}

} // namespace samp_profiler

