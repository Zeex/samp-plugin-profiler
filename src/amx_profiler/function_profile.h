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

#ifndef AMX_PROFILER_FUNCTION_PROFILE_H
#define AMX_PROFILER_FUNCTION_PROFILE_H

#include "timer.h"

namespace amx_profiler {

class Function;

class FunctionProfile {
public:
	friend class Function;

	explicit FunctionProfile(Function *f);

	Function *function();
	const Function *function() const;

	long &num_calls();
	const long &num_calls() const;

	Timer::TimeType &total_time();
	const Timer::TimeType &total_time() const; 

	Timer::TimeType &child_time();
	const Timer::TimeType &child_time() const;

private:
	Function *func_;

	long num_calls_;
	Timer::TimeType total_time_;
	Timer::TimeType child_time_;
};

} // namespace amx_profiler

#endif // !AMX_PROFILER_FUNCTION_PROFILE_H
