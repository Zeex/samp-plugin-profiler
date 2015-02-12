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

#include "performance_counter.h"

namespace amxprof {

PerformanceCounter::PerformanceCounter(PerformanceCounter *parent,
                                       PerformanceCounter *shadow) 
 : started_(false),
   parent_(parent),
   shadow_(shadow)
{
}

void PerformanceCounter::Start() {
  if (!started_) {
    start_point_ = Clock::Now();
    ResetTimes();
    started_ = true;
  }
}

void PerformanceCounter::Stop() {
  if (started_) {
    Nanoseconds time = QueryTotalTime();

    if (shadow_ != 0) {
      latest_total_time_ = 0;
      latest_child_time_ = child_time_;
    } else {
      latest_total_time_ = time;
      latest_child_time_ = child_time_;
    }

    total_time_ = time;
    if (parent_ != 0) {
      parent_->child_time_ += time;
    }

    if (shadow_ != 0) {
      shadow_->total_time_ -= total_time_;
      shadow_->child_time_ -= self_time();
    }

    started_ = false;
  } 
}

void PerformanceCounter::ResetTimes() {
  latest_total_time_ = 0;
  latest_child_time_ = 0;
  total_time_ = 0;
  child_time_ = 0;
}

} // namespace amxprof
