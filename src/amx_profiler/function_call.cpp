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
#include "function_call.h"

namespace amx_profiler {

FunctionCall::FunctionCall(const FunctionPtr &function,
                           cell frame,
                           const FunctionCallPtr &parent)
		: fn_(function)
		, parent_(parent)
		, frame_(frame)
		, timer_(parent != 0 ? &parent->timer_ : 0)
		, recursive_(false)
{
	// Check if this is a recursive call
	FunctionCall *current = parent.get();
	while (current != 0) {
		if (current->fn_ == this->fn_) {
			recursive_ = true;
			break;
		}
		current = current->parent_.get();
	}
}

} // namespace amx_profiler
