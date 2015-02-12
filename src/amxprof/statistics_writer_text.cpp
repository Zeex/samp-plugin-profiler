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

#include <iomanip>
#include <iostream>
#include "duration.h"
#include "function.h"
#include "function_statistics.h"
#include "performance_counter.h"
#include "statistics_writer_text.h"
#include "statistics.h"
#include "time_utils.h"

static const int kTypeWidth = 7;
static const int kNameWidth = 32;
static const int kCallsWidth = 10;
static const int kSelfTimePercentWidth = 15;
static const int kSelfTimeWidth = 15;
static const int kAvgSelfTimeWidth = 15;
static const int kWorstSelfTimeWidth = 15;
static const int kTotalTimePercentWidth = 15;
static const int kTotalTimeWidth = 15;
static const int kAvgTotalTimeWidth = 15;
static const int kWorstTotalTimeWidth = 15;

static const int kWidthAll = kTypeWidth + kNameWidth + kCallsWidth
  + kSelfTimePercentWidth + kSelfTimeWidth + kAvgSelfTimeWidth + kWorstSelfTimeWidth
  + kTotalTimePercentWidth + kTotalTimeWidth + kAvgTotalTimeWidth + kWorstTotalTimeWidth;

static const int kNumColumns = 11;

namespace amxprof {

void StatisticsWriterText::DoHLine() {
  char fillch = stream()->fill();
  *stream() << std::setw(kWidthAll + kNumColumns * 2 + 1)
            << std::setfill('-') << "" << std::setfill(fillch) << '\n';
}

void StatisticsWriterText::Write(const Statistics *stats)
{
  *stream() << "Profile of '" << script_name() << "'";

  if (print_date()) {
    *stream() << " generated on " << CTime();
  }

  if (print_run_time()) {
    *stream() << " (duration: " << TimeSpan(stats->GetTotalRunTime()) << ")\n";
  }

  DoHLine();
  *stream() << std::left
    << "| " << std::setw(kTypeWidth) << "Type"
    << "| " << std::setw(kNameWidth) << "Name"
    << "| " << std::setw(kCallsWidth) << "Calls"
    << "| " << std::setw(kSelfTimePercentWidth) << "Self Time (%)"
    << "| " << std::setw(kSelfTimeWidth) << "Self Time (s)"
    << "| " << std::setw(kAvgSelfTimeWidth) << "Avg. ST (ms)"
    << "| " << std::setw(kWorstSelfTimeWidth) << "Worst ST (ms)"
    << "| " << std::setw(kTotalTimePercentWidth) << "Total Time (%)"
    << "| " << std::setw(kTotalTimeWidth) << "Total Time (s)"
    << "| " << std::setw(kAvgTotalTimeWidth) << "Avg. TT (ms)"
    << "| " << std::setw(kWorstTotalTimeWidth) << "Worst TT (ms)"
    << "|\n";
  DoHLine();

  std::vector<FunctionStatistics*> all_fn_stats;
  stats->GetStatistics(all_fn_stats);

  typedef std::vector<FunctionStatistics*>::const_iterator FuncIterator;

  Nanoseconds self_time_all;
  for (FuncIterator it = all_fn_stats.begin(); it != all_fn_stats.end(); ++it) {
    const FunctionStatistics *fn_stats = *it;
    self_time_all += fn_stats->self_time();
  }

  Nanoseconds total_time_all;
  for (FuncIterator it = all_fn_stats.begin(); it != all_fn_stats.end(); ++it) {
    const FunctionStatistics *fn_stats = *it;
    total_time_all += fn_stats->total_time();
  }

  std::ostream::fmtflags flags = stream()->flags();
  stream()->flags(flags | std::ostream::fixed);

  for (FuncIterator it = all_fn_stats.begin(); it != all_fn_stats.end(); ++it) {
    const FunctionStatistics *fn_stats = *it;

    double self_time_percent =
      fn_stats->self_time().count() * 100 / self_time_all.count();
    double total_time_percent =
      fn_stats->total_time().count() * 100 / total_time_all.count();

    double self_time = Seconds(fn_stats->self_time()).count();
    double total_time = Seconds(fn_stats->total_time()).count();

    double avg_self_time =
      Milliseconds(fn_stats->self_time()).count() / fn_stats->num_calls();
    double avg_total_time =
      Milliseconds(fn_stats->total_time()).count() / fn_stats->num_calls();

    double worst_self_time =
      Milliseconds(fn_stats->worst_self_time()).count();
    double worst_total_time =
      Milliseconds(fn_stats->worst_total_time()).count();

    *stream()
      << "| " << std::setw(kTypeWidth) << fn_stats->function()->GetTypeString()
      << "| " << std::setw(kNameWidth) << fn_stats->function()->name()
      << "| " << std::setw(kCallsWidth) << fn_stats->num_calls()
      << "| " << std::setw(kSelfTimePercentWidth) << std::setprecision(2)
        << self_time_percent
      << "| " << std::setw(kSelfTimeWidth) << std::setprecision(1)
        << self_time
      << "| " << std::setw(kAvgSelfTimeWidth) << std::setprecision(1)
        << avg_self_time
      << "| " << std::setw(kWorstSelfTimeWidth) << std::setprecision(1)
        << worst_self_time
      << "| " << std::setw(kTotalTimePercentWidth) << std::setprecision(2)
        << total_time_percent
      << "| " << std::setw(kTotalTimeWidth) << std::setprecision(1)
        << total_time
      << "| " << std::setw(kAvgTotalTimeWidth) << std::setprecision(1)
        << avg_total_time
      << "| " << std::setw(kWorstTotalTimeWidth) << std::setprecision(1)
        << worst_total_time
      << "|\n";
    DoHLine();
  }

  stream()->flags(flags);
}

} // namespace amxprof
