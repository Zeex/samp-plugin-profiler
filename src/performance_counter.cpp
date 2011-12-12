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

#include "performance_counter.h"

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
	if (!started_) {			
		++num_calls_;
		current_parent_ = parent;
		start_point_ = Clock::now();
		started_ = true;
	} else {
		// Start() called twice - restart counter
		Stop();
		Start();
	}
}

void PerformanceCounter::Stop() {
	Clock::duration time;
	if (started_) {
		time = Clock::now() - start_point_;
		started_ = false;
	} else {
		// Stop() called twice 
		time = Clock::now() - stop_point_;	
	}

	total_time_ += time;		
	if (current_parent_ != 0) {
		current_parent_->child_time_ += time;
	}

	stop_point_ = Clock::now();
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
