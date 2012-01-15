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

#ifndef AMX_PROFILER_FUNCTION_CALL_H
#define AMX_PROFILER_FUNCTION_CALL_H

#include <memory>
#include <amx/amx.h>
#include "function.h"
#include "performance_counter.h"

namespace amx_profiler {

class FunctionCall;
typedef std::shared_ptr<FunctionCall> FunctionCallPtr;

class FunctionCall {
public:
	FunctionCall(const FunctionPtr &function,
                 cell frame,
                 const FunctionCallPtr &parent = 0);

	FunctionPtr &function()
		{ return fn_; }
	const FunctionPtr &function() const
		{ return fn_; }
	cell frame() const
		{ return frame_; }
	PerformanceCounter &timer()
		{ return timer_; }
	const PerformanceCounter &timer() const
		{ return timer_; }
	bool IsRecursive() const
		{ return recursive_; }

private:
	FunctionPtr fn_;
	FunctionCallPtr parent_;
	cell frame_; // frame address on AMX stack
	PerformanceCounter timer_;
	bool recursive_; // whether it's a recursive call
};

} // namespace amx_profiler

#endif // !AMX_PROFILER_FUNCTION_CALL_H
