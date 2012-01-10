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

#include "call_stack.h"
#include "function_profile.h"
#include "function_runtime_info.h"

namespace samp_profiler {

void CallStack::Push(Function *function, ucell frame, bool recursive) { 
	Push(FunctionCall(function, frame, recursive));
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

} // namespace samp_profiler