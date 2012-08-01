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

#ifndef AMX_PROFILER_PROFILER_H
#define AMX_PROFILER_PROFILER_H

#include <set>
#include <vector>
#include <unordered_map>
#include <amx/amx.h>
#include "call_graph.h"
#include "call_stack.h"
#include "debug_info.h"

namespace amx_profiler {

class FunctionStatistics;
class LineStatistics;
class ProfileWriter;

class Profiler {
public:
	Profiler(AMX *amx, DebugInfo debug_info = DebugInfo(), bool enable_call_graph = true);
	~Profiler();

	std::vector<FunctionStatistics*> GetProfile() const;
	void WriteProfile(ProfileWriter *writer) const;

	inline const CallStack *call_stack() const {
		return &call_stack_;
	}

	inline const CallGraph *call_graph() const {
		return &call_graph_;
	}

	int amx_Debug(
		int (AMXAPI *debug)(AMX *amx) = 0);
	int amx_Exec(cell *retval, int index,
		int (AMXAPI *exec)(AMX *amx, cell *retval, int index) = ::amx_Exec);
	int amx_Callback(cell index, cell *result, cell *params,
		int (AMXAPI *callback)(AMX *amx, cell index, cell *result, cell *params) = ::amx_Callback);

private:
	Profiler();

	ucell GetNativeAddress(cell index);
	ucell GetPublicAddress(cell index);

	void BeginFunction(ucell address, ucell frm);
	void EndFunction(ucell address = 0);

private:
	AMX *amx_;
	DebugInfo debug_info_;
	bool call_graph_enabled_;

	CallStack call_stack_;
	CallGraph call_graph_;

	std::unordered_map<ucell, FunctionStatistics*> address_to_fun_stats_;
	std::set<Function*> functions_;

	std::unordered_map<ucell, LineStatistics*> address_to_line_stats_;
};

inline bool IsAmxProfilable(AMX *amx) {
	uint16_t flags;
	amx_Flags(amx, &flags);
	if ((flags & AMX_FLAG_DEBUG) != 0) {
		return true; 
	}
	if ((flags & AMX_FLAG_NOCHECKS) == 0) {
		return true;
	}
	return false;
}

} // namespace amx_profiler

#endif // !AMX_PROFILER_PROFILER_H
