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

#include "call_stack.h"
#include "function_profile.h"

namespace amx_profiler {

void CallStack::Push(Function *function, ucell frame) { 
	Push(FunctionCall(function, frame, calls_.empty() ? 0 : &calls_.top()));
}

void CallStack::Push(const FunctionCall &call) { 
	calls_.push(call); 
	calls_.top().timer().Start();
}

FunctionCall CallStack::Pop() {
	FunctionCall &top = calls_.top();
	top.timer().Stop();
	FunctionCall top_copy(top);
	calls_.pop();
	return top_copy;
}

} // namespace amx_profiler