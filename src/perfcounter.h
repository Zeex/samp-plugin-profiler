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

#ifndef SAMP_SAMP_PROFILER_PERFCOUNTER_H
#define SAMP_SAMP_PROFILER_PERFCOUNTER_H

#include <boost/chrono.hpp>
#include <boost/integer.hpp>

namespace samp_profiler {

typedef boost::int64_t TimeType;

class PerformanceCounter {
public:
	typedef boost::chrono::system_clock Clock;

	PerformanceCounter();
	~PerformanceCounter();

	void Start(PerformanceCounter *parent = 0);
	void Stop();

	long GetNumberOfCalls() const;

	TimeType GetTotalTime() const;
	TimeType GetChildTime() const;
	TimeType GetTime() const;

private:
	// Whether the counter started
	bool started_;

	// Number of calls to Start()
	long num_calls_;

	// Start point
	Clock::time_point start_;

	// Amount of time taken by child counters
	Clock::duration child_time_;

	// Total time (including child_time_)
	Clock::duration total_time_;

	// Current parent counter 
	PerformanceCounter *current_parent_;
};

} // namespace samp_profiler

#endif // !SAMP_SAMP_PROFILER_PERFCOUNTER_H