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

#ifndef SAMP_PROFILER_TIMER_H
#define SAMP_PROFILER_TIMER_H

#include <boost/chrono.hpp>
#include <boost/integer.hpp>

namespace samp_profiler {

class Timer {
public:
	typedef boost::int64_t TimeType;
	typedef boost::chrono::high_resolution_clock ClockType;

	Timer();
	~Timer();

	void Start();
	void Stop();

	TimeType total_time() const;

private:
	bool started_;
	ClockType::time_point start_point_;
	ClockType::duration total_time_;
};

} // namespace samp_profiler

#endif // !SAMP_PROFILER_TIMER_H
