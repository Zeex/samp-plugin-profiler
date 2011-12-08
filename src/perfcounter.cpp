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
	, current_parent_(0)
{
}

PerformanceCounter::~PerformanceCounter() {
	Stop();
}

void PerformanceCounter::Start(PerformanceCounter *parent) {
	++num_calls_;

	if (!started_) {			
		start_ = Clock::now();
		current_parent_ = parent;
		started_ = true;
	}	
}

void PerformanceCounter::Stop() {
	if (started_) {
		Clock::duration time = Clock::now() - start_;
		total_time_ += time;		
		if (current_parent_ != 0) {
			current_parent_->child_time_ += time;
		}
		started_ = false;
	}
}

long PerformanceCounter::GetNumberOfCalls() const {
	return num_calls_;
}

TimeType PerformanceCounter::GetTotalTime() const {
	using namespace boost::chrono;
	return duration_cast<microseconds>(total_time_).count();
}

TimeType PerformanceCounter::GetChildTime() const {
	using namespace boost::chrono;
	return duration_cast<microseconds>(child_time_).count();
}

TimeType PerformanceCounter::GetTime() const {
	return GetTotalTime() - GetChildTime();
}

} // namespace samp_profiler