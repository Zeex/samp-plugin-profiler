// AMX profiler for SA-MP server: http://sa-mp.com
//
// Copyright (C) 2011-2012 Sergey Zolotarev
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

#include "function_info.h"

namespace amx_profiler {

FunctionInfo::FunctionInfo(std::shared_ptr<Function> f)
	: func_(f)
	, num_calls_(1)
	, total_time_(0)
	, child_time_(0)
{
}

std::shared_ptr<Function> FunctionInfo::function() const {
	return func_;
}

long &FunctionInfo::num_calls() {
	return num_calls_;
}

const long &FunctionInfo::num_calls() const {
	return num_calls_;
}

TimeInterval &FunctionInfo::total_time() {
	return total_time_;
}

const TimeInterval &FunctionInfo::total_time() const {
	return total_time_;
}

TimeInterval &FunctionInfo::child_time() {
	return child_time_;
}

const TimeInterval &FunctionInfo::child_time() const {
	return child_time_;
}

} // namespace amx_profiler
