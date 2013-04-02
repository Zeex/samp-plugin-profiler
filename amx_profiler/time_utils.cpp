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

#include <ctime>
#include <iomanip>
#include <iostream>
#include "duration.h"
#include "time_utils.h"

namespace amx_profiler {

std::time_t TimeStamp::Now() {
	return std::time(0);
}

TimeStamp::TimeStamp()
	: value_(Now())
{
}

TimeStamp::TimeStamp(std::time_t value)
	: value_(value)
{
}

const char *CTime(TimeStamp ts) {
	std::time_t now = TimeStamp::Now();

	char *string = const_cast<char*>(std::ctime(&now));
	string[kCTimeResultLength] = '\0';

	return string;
}

TimeSpan::TimeSpan(Duration d)
	: duration_(d)
{
	weeks_ = static_cast<int>(Weeks(d).count());
	d -= Weeks(weeks_);

	days_ = static_cast<int>(Days(d).count());
	d -= Days(days_);

	hours_ = static_cast<int>(Hours(d).count());
	d -= Hours(hours_);

	minutes_ = static_cast<int>(Minutes(d).count());
	d -= Minutes(minutes_);

	seconds_ = static_cast<int>(Seconds(d).count());
	d -= Seconds(seconds_);
}

static const char *Choose(int n, const char *singular, const char *plural) {
	return (n > 1 || n == 0) ? plural : singular;
}

std::ostream &operator<<(std::ostream &os, const TimeSpan &time) {
	Duration duration = time.duration();

	if (time.weeks() > 0) {
		os << time.weeks() << Choose(time.weeks(), " week ", " weeks ");
	}
	if (time.days() > 0) {
		os << time.days() << Choose(time.days(), " day ", " days ");
	}
	if (time.hours() > 0) {
		os << time.hours() << Choose(time.hours(), " hour ", " hours ");
	}
	if (time.minutes() > 0) {
		os << time.minutes() << Choose(time.minutes(), " minute ", " minutes ");
	}
	if (time.seconds() > 0) {
		os << time.seconds() << Choose(time.seconds(), " second ", " seconds ");
	}

	return os;
}

} // namespace amx_profiler
