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

#ifndef AMX_PROFILER_DEBUG_INFO_H
#define AMX_PROFILER_DEBUG_INFO_H

#include <memory>
#include <string>
#include <amx/amx.h>
#include <amx/amxdbg.h>

namespace amx_profiler {

class DebugInfo {
public:
	DebugInfo();
	explicit DebugInfo(const AMX_DBG *amxdbg);
	explicit DebugInfo(const AMX_DBG &amxdbg);
	explicit DebugInfo(std::shared_ptr<AMX_DBG> amxdbg);
	explicit DebugInfo(const std::string &filename);

	static void FreeAmxDbg(AMX_DBG *amxdbg);

	void Load(const std::string &filename);
	bool IsLoaded() const;
	void Free();

	long GetLine(cell address) const;
	std::string GetFile(cell address) const;
	std::string GetFunction(cell address) const;

private:
	std::shared_ptr<AMX_DBG> amxdbg_;
};

inline bool HasDebugInfo(AMX *amx) {
	uint16_t flags;
	amx_Flags(amx, &flags);
	return ((flags & AMX_FLAG_DEBUG) != 0);
}	

} // namespace amx_profiler

#endif // !AMX_PROFILER_DEBUGINFO_H

