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

PerformanceCounter::PerformanceCounter() 
	: started_(false)
	, paused_(false)
	, child_(0)
	, parent_(0)
	, num_calls_(0)
{
}

PerformanceCounter::~PerformanceCounter() {
	Stop();
}

void PerformanceCounter::Start(PerformanceCounter *parent) {
	if (!started_) {	
		if (parent != 0) {
			parent->child_ = this;
			parent->Pause();
		}	
		parent_ = parent;

		++num_calls_;
		started_ = true;		
		start_ = Clock::now();
	}
}

void PerformanceCounter::Stop() {
	if (started_) {
		total_time_ += (Clock::now() - start_);

		if (child_ != 0) {
			child_->Stop();
			child_ = 0;
		}

		if (parent_ != 0) {
			parent_->child_ = 0;
			parent_->Resume();
			parent_ = 0;
		}

		started_ = false;
	}
}

void PerformanceCounter::Pause() {
	if (!paused_) {
		total_time_ += (Clock::now() - start_);
		paused_ = true;
	}
}

void PerformanceCounter::Resume() {
	if (paused_) {
		start_ = Clock::now();
		paused_ = false;
	}
}

std::int64_t PerformanceCounter::GetNumberOfCalls() const {
	return num_calls_;
}

std::int64_t PerformanceCounter::GetTotalTime() const {
	using namespace boost::chrono;
	return duration_cast<microseconds>(total_time_).count();
}