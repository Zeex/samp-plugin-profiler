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

#include <functional>
#include <ostream>
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <amx/amx.h>
#include "call_stack.h"
#include "debug_info.h"
#include "function.h"
#include "function_profile.h"
#include "timer.h"

namespace amx_profiler {

class Printer;

class Profiler {
public:
	static bool IsScriptProfilable(AMX *amx);

	Profiler(AMX *amx);
	~Profiler();

	static void Attach(AMX *amx);
	static void Attach(AMX *amx, const DebugInfo &debug_info);
	static void Detach(AMX *amx);

	static Profiler *Get(AMX *amx);

	void Activate();
	bool IsActive() const;
	void Deactivate();

	void SetDebugInfo(const DebugInfo &info);

	void ResetStats();
	void PrintStats(const std::string &script_name, std::ostream &stream, Printer *printer) const;

	int Debug();
	int Callback(cell index, cell *result, cell *params);
	int Exec(cell *retval, int index);	

private:
	Profiler();

	bool active_;

	AMX       *amx_;
	DebugInfo debug_info_;
	AMX_DEBUG debug_;

	CallStack call_stack_;

	void EnterFunction(const Function *function, ucell frm);
	void LeaveFunction(const Function *function = 0);	

	class CompFunByPtr : public std::binary_function<const Function*, const Function*, bool> {
	public:
		bool operator()(const Function *left, const Function *right) {
			return left->address() < right->address();
		}
	};

	typedef std::map<Function*, FunctionProfile, CompFunByPtr> Functions;
	Functions functions_;

	static std::map<AMX*, Profiler*> instances_;
};

} // namespace amx_profiler

#endif // !AMX_PROFILER_PROFILER_H
