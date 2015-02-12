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

#ifndef AMXPROF_FUNCTION_INFO_H
#define AMXPROF_FUNCTION_INFO_H

#include "duration.h"

namespace amxprof {

class Function;

// Various runtime information about a function.
class FunctionStatistics {
 public:
  explicit FunctionStatistics(Function *fn);

  Function *function() { return fn_; }
  const Function *function() const { return fn_; }

  long num_calls() const { return num_calls_; }
  void AdjustNumCalls(long delta) { num_calls_ += delta; }

  Nanoseconds self_time() const { return self_time_; }
  Nanoseconds total_time() const { return total_time_; }

  Nanoseconds worst_self_time() const { return worst_self_time_; }
  Nanoseconds worst_total_time() const { return worst_total_time_; }

  void set_worst_self_time(Nanoseconds worst_self_time) {
    worst_self_time_ = worst_self_time;
  }

  void set_worst_total_time(Nanoseconds worst_total_time) {
    worst_total_time_ = worst_total_time;
  }

  void AdjustSelfTime(Nanoseconds delta);
  void AdjustTotalTime(Nanoseconds delta);

 private:
  Function *fn_;
  long num_calls_;
  Nanoseconds self_time_;
  Nanoseconds total_time_;
  Nanoseconds worst_self_time_;
  Nanoseconds worst_total_time_;
};

} // namespace amxprof

#endif // !AMXPROF_FUNCTION_INFO_H
