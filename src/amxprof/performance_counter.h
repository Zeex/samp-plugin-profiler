// Copyright (c) 2011-2015 Zeex
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

#ifndef AMXPROF_PERFORMANCE_COUNTER_H
#define AMXPROF_PERFORMANCE_COUNTER_H

#include "clock.h"

namespace amxprof {

class PerformanceCounter {
 public:
  PerformanceCounter(PerformanceCounter *parent = 0,
                     PerformanceCounter *shadow = 0);

  void Start();
  void Stop();

  void ResetTimes();

  Nanoseconds QueryTotalTime() const {
    return Clock::Now() - start_point_;
  }

  void set_parent(PerformanceCounter *parent) { parent_ = parent; }
  void set_shadow(PerformanceCounter *shadow) { shadow_ = shadow; }

  Nanoseconds latest_total_time() const { return latest_total_time_; }
  Nanoseconds latest_child_time() const { return latest_child_time_; }

  Nanoseconds latest_self_time() const {
    return latest_total_time_ - latest_child_time_;
  }

  Nanoseconds child_time() const { return child_time_; }
  Nanoseconds total_time() const { return total_time_; }

  Nanoseconds self_time() const {
    return total_time_ - child_time_;
  }

 private:
  bool started_;

  PerformanceCounter *parent_;
  PerformanceCounter *shadow_;

  TimePoint start_point_;

  Nanoseconds latest_total_time_;
  Nanoseconds latest_child_time_;
  Nanoseconds child_time_;
  Nanoseconds total_time_;
};

} // namespace amxprof

#endif // !AMXPROF_PERFORMANCE_COUNTER_H
