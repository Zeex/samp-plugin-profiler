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

std::time_t TimeNow() {
	return std::time(nullptr);
}

const char *CTimeNow() {
	std::time_t now = TimeNow();

	char *string = const_cast<char*>(std::ctime(&now));
	string[kCTimeResultLength] = '\0';

	return string;
}

TimeSpan::TimeSpan(Duration d)
	: w_(d)
	, d_(d)
	, h_(d)
	, m_(d)
	, s_(d)
{
}

static const char *Choose(double n, const char *singular, const char *plural) {
	return (n >= 2.0 || n < 1.0) ? plural : singular;
}

std::ostream &operator<<(std::ostream &os, const TimeSpan &time) {
	bool first = true;

	if (time.weeks().count() >= 1.0) {
		os << static_cast<int>(time.weeks().count())
		   << Choose(time.weeks().count(), " week ", " weeks ");
		first = false;
	}

	if (!first || time.days().count() >= 1.0) {
		os << static_cast<int>(time.days().count())
		   << Choose(time.days().count(), " day ", " days ");
		first = false;
	}

	if (!first || time.hours().count() >= 1.0) {
		os << static_cast<int>(time.hours().count())
		   << Choose(time.hours().count(), " hour ", " hours ");
		first = false;
	}

	if (!first || time.minutes().count() >= 1.0) {
		os << static_cast<int>(time.minutes().count())
		   << Choose(time.minutes().count(), " minute ", " minutes ");
		first = false;
	}

	if (!first || time.seconds().count() >= 1.0) {
		os << static_cast<int>(time.seconds().count())
		   << Choose(time.seconds().count(), " second", " seconds");
	}

	return os;
}

} // namespace amx_profiler
