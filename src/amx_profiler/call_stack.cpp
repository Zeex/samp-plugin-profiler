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

#include <memory>
#include "call_stack.h"
#include "function_call.h"
#include "performance_counter.h"

namespace amx_profiler {

void CallStack::Push(const std::shared_ptr<Function> &function, ucell frame) {
	Push(std::shared_ptr<FunctionCall>(new FunctionCall(function, frame, calls_.empty() ? 0 : calls_.back())));
}

void CallStack::Push(const std::shared_ptr<FunctionCall> &call) {
	call->timer().Start();
	calls_.push_back(call);
}

std::shared_ptr<FunctionCall> CallStack::Pop() {
	std::shared_ptr<FunctionCall> top = calls_.back();
	calls_.pop_back();
	top->timer().Stop();
	return top;
}

} // namespace amx_profiler
