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

#include <iostream>
#include "duration.h"
#include "function.h"
#include "function_statistics.h"
#include "performance_counter.h"
#include "statistics_writer_json.h"
#include "statistics.h"
#include "time_utils.h"

namespace amxprof {

static std::string EscapString(const std::string &s) {
  std::string t;

  for (std::string::const_iterator iterator = s.begin();
       iterator != s.end(); ++iterator) {
    switch (*iterator) {
      // According to http://www.json.org other escape sequences,
      // apart from Unicode, are not supported by JSON.
      case '"': t.append("\\\""); break;
      case '\\': t.append("\\\\"); break;
      case '\b': t.append("\\b"); break;
      case '\f': t.append("\\f"); break;
      case '\n': t.append("\\n"); break;
      case '\r': t.append("\\r"); break;
      case '\t': t.append("\\t"); break;
      default: t.push_back(*iterator);
    }
  }

  return t;
}

void StatisticsWriterJson::Write(const Statistics *stats)
{
  *stream() << "{\n"
            << "  \"script\": \"" << EscapString(script_name()) << "\",\n";

  if (print_date()) {
    *stream() << "  \"timestamp\": " << TimeStamp::Now() << ",\n";
  }

  if (print_run_time()) {
    *stream() << "  \"duration\": "
              << Seconds(stats->GetTotalRunTime()).count() << ",\n";
  }

  *stream() << "  \"functions\": [\n";

  std::vector<FunctionStatistics*> all_fn_stats;
  stats->GetStatistics(all_fn_stats);

  typedef std::vector<FunctionStatistics*>::const_iterator FuncIterator;

  for (FuncIterator it = all_fn_stats.begin(); it != all_fn_stats.end(); ++it) {
    const FunctionStatistics *fn_stats = *it;

    *stream() << "    {\n"
      << "      \"type\": \""
        << fn_stats->function()->GetTypeString() << "\",\n"
      << "      \"name\": \""
        << fn_stats->function()->name() << "\",\n"
      << "      \"calls\": "
       << fn_stats->num_calls() << ",\n"
      << "      \"selfTime\": "
        << fn_stats->self_time().count() << ",\n"
      << "      \"worstSelfTime\": "
        << fn_stats->worst_self_time().count() << ",\n"
      << "      \"totalTime\": "
        << fn_stats->total_time().count() << ",\n"
      << "      \"worstTotalTime\": "
        << fn_stats->worst_total_time().count() << "\n"
    << "    },\n";
  }

  *stream() << "    {}\n  ]\n}\n";
}

} // namespace amxprof
