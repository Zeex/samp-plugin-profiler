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

#ifndef AMX_PROFILER_TIMER_H
#define AMX_PROFILER_TIMER_H

#include <boost/chrono.hpp>
#include <boost/integer.hpp>

namespace amx_profiler {

class Timer {
public:
	typedef boost::int64_t TimeType;
	typedef boost::chrono::high_resolution_clock ClockType;

	Timer(Timer *parent = 0);
	~Timer();

	void Start();
	void Stop();

	TimeType child_time() const;
	TimeType total_time() const;

	inline TimeType self_time() const 
		{ return total_time() - child_time(); }

private:
	bool started_;
	Timer *parent_;
	ClockType::time_point start_point_;
	ClockType::duration child_time_;
	ClockType::duration total_time_;
};

} // namespace amx_profiler

#endif // !AMX_PROFILER_TIMER_H
