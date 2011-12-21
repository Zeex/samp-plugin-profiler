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

#include "timer.h"

namespace samp_profiler {

Timer::Timer() 
	: started_(false)
{
}

Timer::~Timer() {
	Stop();
}

void Timer::Start() {
	if (!started_) {			
		start_point_ = ClockType::now();
		started_ = true;
	}
}

void Timer::Stop() {
	if (started_) {
		total_time_+= ClockType::now() - start_point_;
		started_ = false;
	} 
}

Timer::TimeType Timer::total_time() const {
	using namespace boost::chrono;
	return duration_cast<microseconds>(total_time_).count();
}

} // namespace samp_profiler
