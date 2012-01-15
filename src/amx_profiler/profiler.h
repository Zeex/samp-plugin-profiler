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
#include <map>
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

	int AmxDebugHook();
	int AmxExecHook(cell *retval, int index);
	int AmxCallbackHook(cell index, cell *result, cell *params);

private:
	Profiler();

private:
	AMX       *amx_;
	DebugInfo debug_info_;

	CallStack call_stack_;

	void EnterFunction(const std::shared_ptr<Function> &function, ucell frm);
	void LeaveFunction(const std::shared_ptr<Function> &function = 0);

	class CompFunByPtr {
	public:
		bool operator()(const std::shared_ptr<Function> left,
						const std::shared_ptr<Function> right) {
			return left->address() < right->address();
		}
	};

	std::map<
		std::shared_ptr<Function>,
		std::shared_ptr<FunctionInfo>,
		CompFunByPtr
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
