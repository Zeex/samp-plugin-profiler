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

#ifndef SAMP_PROFILER_CALL_STACK_H
#define SAMP_PROFILER_CALL_STACK_H

#include <stack>

#include "function.h"
#include "timer.h"

namespace samp_profiler {

class CallInfo {
public:
	CallInfo(const Function &function, cell frame = 0)
		: function_(function)
		, frame_(frame)
	{
	}

	const Function &function() const 
		{ return function_; }
	cell frame() const
		{ return frame_; }
	Timer &timer() 
		{ return timer_; }

private:
	Function function_;
	cell frame_; // frame address on AMX stack
	Timer timer_;
};

class CallStack {
public:
	void Push(const Function &function, cell frame);
	void Push(const CallInfo &info);
	CallInfo Pop();

	bool IsEmpty() const 
		{ return calls_.empty(); }
	const CallInfo &GetTop()
		{ return calls_.top(); }

private:
	std::stack<CallInfo> calls_;
};

} // namespace samp_profiler

#endif // !SAMP_PROFILER_CALL_STACK_H
