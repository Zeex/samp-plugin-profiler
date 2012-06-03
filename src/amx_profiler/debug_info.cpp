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
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <cassert>
#include <cstdio>
#include <cstring>
#include <amx/amx.h>
#include <amxdbg.h>
#include "debug_info.h"

namespace amx_profiler {

DebugInfo::DebugInfo()
{
}

DebugInfo::DebugInfo(std::shared_ptr<AMX_DBG> amxdbg)
	: amxdbg_(amxdbg)
{
}

DebugInfo::DebugInfo(const AMX_DBG *amxdbg) 
	: amxdbg_(new AMX_DBG)
{
	std::memcpy(amxdbg_.get(), amxdbg, sizeof(AMX_DBG));
}

DebugInfo::DebugInfo(const AMX_DBG &amxdbg)
	: amxdbg_(new AMX_DBG(amxdbg))
{
}

DebugInfo::DebugInfo(const std::string &filename) {
	Load(filename);
}

void DebugInfo::FreeAmxDbg(AMX_DBG *amxdbg) {
	if (amxdbg != 0) {
		dbg_FreeInfo(amxdbg);
		delete amxdbg;
	}
}

bool DebugInfo::IsLoaded() const {
	return (amxdbg_.get() != 0);
}

void DebugInfo::Load(const std::string &filename) {
	std::FILE* fp = std::fopen(filename.c_str(), "rb");
	if (fp != 0) {
		amxdbg_.reset(new AMX_DBG, FreeAmxDbg);
		if (dbg_LoadInfo(amxdbg_.get(), fp) != AMX_ERR_NONE) 
			amxdbg_.reset();
		fclose(fp);
	}
}

void DebugInfo::Free() {
	amxdbg_.reset();
}

long DebugInfo::GetLine(cell address) const {
	long line = 0;
	dbg_LookupLine(amxdbg_.get(), address, &line);
	return line;
}

std::string DebugInfo::GetFile(cell address) const {
	std::string result;
	const char *file;
	if (dbg_LookupFile(amxdbg_.get(), address, &file) == AMX_ERR_NONE)
		result.assign(file);
	return result;
}

std::string DebugInfo::GetFunction(cell address) const {
	std::string result;
	const char *function;
	if (dbg_LookupFunction(amxdbg_.get(), address, &function) == AMX_ERR_NONE)
		result.assign(function);
	return result;
}

} // namespace amx_profiler

