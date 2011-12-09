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

#include "performance_counter.h"

namespace samp_profiler {

class ProfileEntry {
public:
	ProfileEntry(const std::string &function_name, const std::string &function_type, PerformanceCounter counter)
		: name_(function_name)
		, type_(function_type)
		, counter_(counter)
	{
	}

	inline const std::string &GetFunctionName() const 
		{ return name_; }
	inline const std::string &GetFunctionType() const 
		{ return type_; }
	inline const PerformanceCounter &GetCounter() const 
		{ return counter_; }

	static bool CompareCalls(const ProfileEntry &left, const ProfileEntry &right) {
		return left.GetCounter().GetNumberOfCalls() > right.GetCounter().GetNumberOfCalls();
	}

	static bool CompareTime(const ProfileEntry &left, const ProfileEntry &right) {
		return left.GetCounter().GetTime() > right.GetCounter().GetTime();
	}

	static bool CompareTotalTime(const ProfileEntry &left, const ProfileEntry &right) {
		return left.GetCounter().GetTotalTime() > right.GetCounter().GetTotalTime();
	}

private:
	std::string        name_;
	std::string        type_;
	PerformanceCounter counter_;
};

typedef std::vector<ProfileEntry> Profile;

} // namespace samp_profiler

#endif // !SAMP_PROFILER_PROFILE_H
