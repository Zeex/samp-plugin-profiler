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

#ifndef SAMP_PROFILER_PROFILE_H
#define SAMP_PROFILER_PROFILE_H

#include <string>
#include <vector>

#include "timer.h"

namespace samp_profiler {

class ProfileEntry {
public:
	typedef Timer::TimeType Time;

	ProfileEntry(const std::string &function_name, const std::string &function_type, 
			Time time, Time child_time, long num_calls)
		: name_(function_name)
		, type_(function_type)
		, time_(time)
		, child_time_(child_time)
		, num_calls_(num_calls)
	{
	}

	const std::string &function_name() const 
		{ return name_; }
	const std::string &function_type() const 
		{ return type_; }
	Time time() const 
		{ return time_; }
	Time child_time() const 
		{ return child_time_; }
	long num_calls() const 
		{ return num_calls_; }

private:
	std::string name_;
	std::string type_;
	Time        time_;
	Time        child_time_;
	long        num_calls_;
};

typedef std::vector<ProfileEntry> Profile;

} // namespace samp_profiler

#endif // !SAMP_PROFILER_PROFILE_H
