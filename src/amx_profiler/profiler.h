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

#ifndef AMX_PROFILER_PROFILER_H
#define AMX_PROFILER_PROFILER_H

#include <iosfwd>
#include <stack>
#include <string>
#include <vector>
#include <unordered_map>
#include <amx/amx.h>
#include "call_graph.h"
#include "call_stack.h"
#include "debug_info.h"

namespace amx_profiler {

class FunctionInfo;

class Profiler {
public:
	Profiler(AMX *amx, DebugInfo debug_info = DebugInfo());

	// Get profiling results.
	std::vector<std::shared_ptr<FunctionInfo>> GetProfile() const;

	// Get the current call stack.
	inline const CallStack &call_stack() const {
		return call_stack_;
	}

	// Get the call graph.
	// The root node points out to the currently executng function.
	inline const CallGraph &call_graph() const {
		return call_graph_;
	}

	// These methods must be invoked by Profiler class user
	// to make it do the job.
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
	// The AMX instance associated with this Profiler instance.
	AMX *amx_;

	// AMX debug info, used to extract function names.
	DebugInfo debug_info_;

	CallStack call_stack_;
	CallGraph call_graph_;

	std::unordered_map<
		ucell,  // address
		std::shared_ptr<FunctionInfo>
	> functions_;
};

// Check if an AMX program can be profiled.
inline bool IsAmxProfilable(AMX *amx) {
	uint16_t flags;
	amx_Flags(amx, &flags);
	if ((flags & AMX_FLAG_DEBUG) != 0) {
		// Have debug info.
		return true; 
	}
	if ((flags & AMX_FLAG_NOCHECKS) == 0) {
		// Have BREAK instructions.
		return true;
	}
	return false;
}

} // namespace amx_profiler

#endif // !AMX_PROFILER_PROFILER_H
