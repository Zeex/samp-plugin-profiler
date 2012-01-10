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

#include "function_profile.h"

namespace samp_profiler {

FunctionProfile::FunctionProfile(Function *f)
	: func_(f)
	, num_calls_(1)
	, total_time_(0)
	, child_time_(0)
{
}

const Function *FunctionProfile::function() const {
	return func_;
}

long &FunctionProfile::num_calls() { 
	return num_calls_; 
}

const long &FunctionProfile::num_calls() const { 
	return num_calls_; 
}

Timer::TimeType &FunctionProfile::total_time() { 
	return total_time_; 
}

const Timer::TimeType &FunctionProfile::total_time() const { 
	return total_time_; 
}

Timer::TimeType &FunctionProfile::child_time() { 
	return child_time_; 
}

const Timer::TimeType &FunctionProfile::child_time() const { 
	return child_time_; 
}

FunctionProfile &FunctionProfile::operator+=(const FunctionProfile &that) {
	this->num_calls_ += that.num_calls_;
	this->total_time_ += that.total_time_;
	this->child_time_ += that.child_time_;
	return *this;
}

} // namespace samp_profiler
