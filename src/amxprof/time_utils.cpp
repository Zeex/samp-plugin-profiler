// Copyright (c) 2013-2015 Zeex
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
#include "time_utils.h"

namespace amxprof {

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

std::string CTime(TimeStamp ts) {
  std::time_t now = TimeStamp::Now();
  std::string str = std::ctime(&now);
  str.erase(str.length() - 1);
  return str;
}

TimeSpan::TimeSpan(Seconds d) {
  hours_ = static_cast<int>(Hours(d).count());
  d -= Hours(hours_);

  minutes_ = static_cast<int>(Minutes(d).count());
  d -= Minutes(minutes_);

  seconds_ = static_cast<int>(Seconds(d).count());
  d -= Seconds(seconds_);
}

std::ostream &operator<<(std::ostream &os, const TimeSpan &time) {
  char fill = os.fill('0');

  os << std::setw(2) << time.hours()   << ':'
     << std::setw(2) << time.minutes() << ':'
     << std::setw(2) << time.seconds();

  os.fill(fill);
  return os;
}

} // namespace amxprof
