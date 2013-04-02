// Copyright (c) 2013, Zeex
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef AMX_PROFILER_TIME_UTILS_H
#define AMX_PROFILER_TIME_UTILS_H

#include <ctime>
#include <iosfwd>
#include "duration.h"

namespace amx_profiler {

const int kCTimeResultLength = 24;

class TimeStamp {
public:
	typedef std::time_t ValueType;

	static ValueType Now();

	TimeStamp();
	TimeStamp(ValueType value);

	ValueType value() const { return value_; }

private:
	ValueType value_;
};

const char *CTime(TimeStamp ts = TimeStamp());

class TimeSpan {
public:
	TimeSpan(Duration d);

	Duration duration() const { return duration_; }

	int hours()   const { return hours_; }
	int minutes() const { return minutes_; }
	int seconds() const { return seconds_; }

private:
	Duration duration_;
	int hours_;
	int minutes_;
	int seconds_;
};

std::ostream &operator<<(std::ostream &os, const TimeSpan &time);

} // namespace amx_profiler

#endif // !AMX_PROFILER_TIME_UTILS_H
