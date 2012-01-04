// SA:MP Profiler plugin
//
// Copyright (c) 2011 Zeex
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

#ifndef SAMP_PROFILER_PROFILER_H
#define SAMP_PROFILER_PROFILER_H

#include <ostream>
#include <set>
#include <stack>
#include <string>
#include <vector>

#include "call_stack.h"
#include "debug_info.h"
#include "function.h"
#include "timer.h"

#include "amx/amx.h"
#include "amx/amxdbg.h"

namespace samp_profiler {

class AbstractPrinter;

class Profiler {
public:
	static bool IsScriptProfilable(AMX *amx);

	Profiler(AMX *amx);

	static void Attach(AMX *amx);
	static void Attach(AMX *amx, const DebugInfo &debug_info);
	static void Detach(AMX *amx);

	static Profiler *Get(AMX *amx);

	void Activate();
	bool IsActive() const;
	void Deactivate();

	void SetDebugInfo(const DebugInfo &info);

	void ResetStats();
	void PrintStats(const std::string &script_name, std::ostream &stream, AbstractPrinter *printer) const;

	int Debug();
	int Callback(cell index, cell *result, cell *params);
	int Exec(cell *retval, int index);

private:
	Profiler();

	bool active_;

	AMX       *amx_;
	DebugInfo debug_info_;
	AMX_DEBUG debug_;

	void EnterFunction(const CallInfo &info);
	void LeaveFunction(const Function &function);

	CallStack call_stack_;

	std::vector<std::string> native_names_;
	std::vector<std::string> public_names_;

	std::set<Function> functions_;

	static std::map<AMX*, Profiler*> instances_;
};

} // namespace samp_profiler

#endif // !SAMP_SAMP_PROFILER_SAMP_SAMP_PROFILER_H
