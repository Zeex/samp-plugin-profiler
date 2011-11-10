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

#ifndef PERFCOUNTER_H
#define PERFCOUNTER_H

#include <cstdint>

#include <boost/chrono.hpp>

class PerformanceCounter {
public:
	typedef boost::chrono::system_clock Clock;

	PerformanceCounter();
	~PerformanceCounter();

	void Start(PerformanceCounter *parent = 0);
	void Stop();

	std::int64_t GetNumberOfCalls() const;
	std::int64_t GetTotalTime() const;

private:
	void Pause();
	void Resume();

	bool started_;
	bool paused_;

	PerformanceCounter *child_;
	PerformanceCounter *parent_;

	std::int64_t num_calls_;

	Clock::time_point start_;
	Clock::duration   total_time_;
};

#endif