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

#include <map>
#include <ostream>
#include <stack>
#include <string>
#include <vector>

#include "debug_info.h"
#include "performance_counter.h"

#include "amx/amx.h"
#include "amx/amxdbg.h"

namespace samp_profiler {

class AbstractPrinter;

class Profiler {
public:
	class Function {
	public:
		Function(cell address, const char *name) 
			: address_(address),
			  name_(name)
		{}

		cell address() const { return address_; }
		std::string name() const { return name_; }

	private:
		cell address_;
		std::string name_;
	};

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
	void PrintStats(std::ostream &stream, AbstractPrinter *printer);

	int Debug();
	int Callback(cell index, cell *result, cell *params);
	int Exec(cell *retval, int index);

private:
	Profiler();

	bool active_;

	AMX          *amx_;
	DebugInfo     debug_info_;
	AMX_DEBUG     debug_;

	class CallInfo {
	public:
		enum FunctionType {
			NATIVE,
			PUBLIC,
			ORDINARY
		};

		CallInfo(cell frame, cell address, FunctionType functionType) 
			: frame_(frame), 
			  address_(address), 
			  functionType_(functionType) 
		{}

		cell frame() const  { return frame_; }
		cell address() const  { return address_; }
		FunctionType functionType() const { return functionType_; }

	private:
		cell frame_;
		cell address_;
		FunctionType functionType_;
	};

	void EnterFunction(const CallInfo &info);
	void LeaveFunction(cell address);
	bool GetLastCall(CallInfo &call) const;

	std::stack<CallInfo> call_stack_;

	std::vector<Function> natives_;
	std::vector<Function> publics_;

	std::map<cell, PerformanceCounter> counters_;

	static std::map<AMX*, Profiler*> instances_;
};

} // namespace samp_profiler

#endif // !SAMP_SAMP_PROFILER_SAMP_SAMP_PROFILER_H
