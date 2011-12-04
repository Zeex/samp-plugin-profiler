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

#include <iostream>

#include "perfcounter.h"

namespace samp_profiler {

PerformanceCounter::PerformanceCounter() 
	: started_(false)
	, num_calls_(0)
{
}

PerformanceCounter::~PerformanceCounter() {
	Stop();
}

void PerformanceCounter::Start() {
	++num_calls_;

	if (!started_) {	
		started_ = true;		
		start_ = Clock::now();
	}
}

void PerformanceCounter::Stop() {
	if (started_) {
		total_time_ += (Clock::now() - start_);
		started_ = false;
	}
}

TimeType PerformanceCounter::GetNumberOfCalls() const {
	return num_calls_;
}

TimeType PerformanceCounter::GetTime() const {
	using namespace boost::chrono;
	return duration_cast<microseconds>(total_time_).count();
}

} // namespace samp_profiler