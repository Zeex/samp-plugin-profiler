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

#include <ostream>
#include <stack>
#include <string>
#include <vector>
#include <unordered_map>
#include <amx/amx.h>
#include "call_stack.h"
#include "debug_info.h"
#include "function.h"
#include "function_info.h"
#include "performance_counter.h"

namespace amx_profiler {

class ProfileWriter;

class Profiler {
public:
	Profiler(AMX *amx);
	Profiler(AMX *amx, DebugInfo debug_info = DebugInfo());
	~Profiler();

	static void Attach(AMX *amx, DebugInfo debug_info = DebugInfo());
	static void Detach(AMX *amx);

	static std::shared_ptr<Profiler> GetInstance(AMX *amx);

	void WriteProfile(const std::string &script_name, 
	                  ProfileWriter *writer,
	                  std::ostream &stream) const;
	std::vector<std::shared_ptr<FunctionInfo>> GetProfile() const;

	inline const CallStack &GetCallStack() const {
		return call_stack_;
	}

	// These methods should be called by Profiler client code to make 
	// it actually do something. 
	// All *Hooks can accept a pointer to the function they have to 
	// invoke in place of the original amx_* routine, e.g. for AmxExecHook 
	// this could be amx_Exec(), etc.
	int AmxDebugHook(
		int (AMXAPI *debug)(AMX *amx) = 0);
	int AmxExecHook(cell *retval, int index, 
		int (AMXAPI *exec)(AMX *amx, cell *retval, int index) = 0);
	int AmxCallbackHook(cell index, cell *result, cell *params,
		int (AMXAPI *callback)(AMX *amx, cell index, cell *result, cell *params) = 0);

private:
	Profiler();

	ucell GetNativeAddress(cell index);
	ucell GetPublicAddress(cell index);

	void EnterFunction(ucell address, ucell frm);
	void LeaveFunction(ucell address = 0);

private:
	AMX       *amx_;
	DebugInfo debug_info_;

	CallStack call_stack_;

	std::unordered_map<
		ucell,  // address
		std::shared_ptr<FunctionInfo>
	> functions_;

	static std::unordered_map<
		AMX*,
		std::shared_ptr<Profiler>
	> instances_;
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
