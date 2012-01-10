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

#ifndef AMX_PROFILER_CALL_STACK_H
#define AMX_PROFILER_CALL_STACK_H

#include <stack>
#include <amx/amx.h>
#include "function_call.h"

namespace amx_profiler {

class FunctionProfile;

class CallStack {
public:
	void Push(Function *function, ucell frame);
	void Push(const FunctionCall &info);
	FunctionCall Pop();

	bool IsEmpty() const
		{ return calls_.empty(); }
	FunctionCall &GetTop()
		{ return calls_.top(); }
	const FunctionCall &GetTop() const
		{ return calls_.top(); }

private:
	std::stack<FunctionCall> calls_;
};

} // namespace amx_profiler

#endif // !AMX_PROFILER_CALL_STACK_H
