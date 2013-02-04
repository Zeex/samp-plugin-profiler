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

Time::Time(Duration d)
	: h_(d)
	, m_(d)
	, s_(d)
{
}

Time::Time(Hours h, Minutes m, Seconds s)
	: h_(h)
	, m_(m)
	, s_(s)
{
}

std::ostream &operator<<(std::ostream &os, const Time &time) {
	char old_fill = os.fill('0');
	os <<  std::setw(2) << static_cast<int>(time.hours().count()) << ":" <<
	       std::setw(2) << static_cast<int>(time.minutes().count()) << ":" <<
	       std::setw(2) << static_cast<int>(time.seconds().count());
	os.fill(old_fill);
	return os;
}

} // namespace amx_profiler
