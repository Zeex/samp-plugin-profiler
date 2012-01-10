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

#ifndef SAMP_PROFILER_FUNCTION_RUNTIME_INFO_H
#define SAMP_PROFILER_FUNCTION_RUNTIME_INFO_H

#include "function_profile.h"

namespace samp_profiler {

class Function;

class FunctionRuntimeInfo {
public:
	explicit FunctionRuntimeInfo(Function *f);

	Function *function() const 
		{ return function_; }

	bool running() const 
		{ return running_; }
	void set_running(bool set) 
		{ running_ = set; }

	const FunctionProfile &profile() const
		{ return profile_; }
	FunctionProfile &profile() 
		{ return profile_; }

private:
	Function *function_;
	FunctionProfile profile_;
	bool running_;
};

} // namespace samp_profiler

#endif // !SAMP_PROFILER_FUNCTION_RUNTIME_INFO_H
