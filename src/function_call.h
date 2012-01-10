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

#ifndef SAMP_PROFILER_FUNCTION_CALL_H
#define SAMP_PROFILER_FUNCTION_CALL_H

#include "function.h"
#include "function_runtime_info.h"
#include "timer.h"
#include "amx/amx.h"

namespace samp_profiler {

class FunctionCall {
public:
	FunctionCall(Function *function, cell frame, bool recursive = false);

	Function *function() const 
		{ return fn_; }
	cell frame() const
		{ return frame_; }
	Timer &timer()
		{ return timer_; }
	const Timer &timer() const 
		{ return timer_; }
	bool IsRecursive() const 
		{ return recursive_; }
	Timer::TimeType GetExecutionTime() const 
		{ return timer_.total_time(); }

private:
	Function *fn_;
	cell frame_; // frame address on AMX stack
	Timer timer_;
	bool recursive_; // whether it's a recursive call
};

} // namespace samp_profiler

#endif // !SAMP_PROFILER_FUNCTION_CALL_H
