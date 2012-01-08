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

#ifndef SAMP_PROFILER_FUNCTION_H
#define SAMP_PROFILER_FUNCTION_H

#include "timer.h"
#include "amx/amx.h"

namespace samp_profiler {

class Function {
public:
	enum Type {
		NATIVE,
		PUBLIC,
		NORMAL
	};

	union Handle {
		ucell address;
		cell  index;
	};

	typedef Timer::TimeType TimeType;

	Function(Type type, Handle handle);

	static Function Native(cell index);
	static Function Public(cell index);
	static Function Normal(ucell address);

	Type type() const
		{ return type_; }
	ucell address() const
		{ return handle_.address; }
	cell index() const 
		{ return handle_.index; }
	TimeType self_time() const 
		{ return self_time_; }
	TimeType child_time() const 
		{ return child_time_; }
	long num_calls() const
		{ return num_calls_; }

	void IncreaseCalls() const {
		++num_calls_; 
	}

	void AdjustSelfTime(TimeType time) const { 
		self_time_ += time; 
	}

	void AdjustChildTime(TimeType child_time) const { 
		child_time_ += child_time; 
	}

	bool operator<(const Function &that) const;
	bool operator==(const Function &that) const;

private:
	Function();

	Type type_;
	Handle handle_;

	mutable TimeType self_time_;
	mutable TimeType child_time_;
	mutable long num_calls_;
};

} // namespace samp_profiler

#endif // !SAMP_PROFILER_FUNCTION_H
