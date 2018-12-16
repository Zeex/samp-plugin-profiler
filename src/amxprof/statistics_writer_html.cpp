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
#include "statistics_writer_html.h"
#include "performance_counter.h"
#include "statistics.h"
#include "time_utils.h"

namespace amxprof {

void StatisticsWriterHtml::Write(const Statistics *stats)
{
  *stream() <<
  "<!DOCTYPE html>\n"
  "<html>\n"
  "<head>\n"
  "  <title>" << "Profile of '" << script_name() << "'</title>\n"
  "  <script type=\"text/javascript\"\n"
  "          src=\"http://code.jquery.com/jquery-latest.min.js\">\n"
  "  </script>\n"
  "  <script type=\"text/javascript\"\n"
  "          src=\"https://cdnjs.cloudflare.com/ajax/libs/jquery.tablesorter/2.31.1/js/jquery.tablesorter.min.js\">\n"
  "  </script>\n"
  "  <script type=\"text/javascript\">\n"
  "    $(document).ready(function() {\n"
  "      $('#data').tablesorter();\n"
  "      $('#data th').hover(function () {\n"
  "        $(this).css({ cursor: 'pointer' });\n"
  "      });\n"
  "    });\n"
  "  </script>\n"
  "</head>\n"
  "<body>\n"
  ;

  *stream() <<
  "  <style type=\"text/css\">\n"
  "    table {\n"
  "      margin: 0 auto;\n"
  "    }\n"
  "    table, th, td {\n"
  "      border: thin solid #555;\n"
  "      border-collapse: collapse;\n"
  "      padding: 7px 10px;\n"
  "    }\n"
  "    th, td {\n"
  "      border: none;\n"
  "    }\n"
  "    th {\n"
  "      color: white;\n"
  "      background-color: #555;\n"
  "    }\n"
  "    td {\n"
  "      font-family: Consolas, \"DejaVu Sans Mono\", "
  "\"Courier New\", Monospace;\n"
  "    }\n"
  "    td.numeric {\n"
  "      text-align: right;\n"
  "    }\n"
  "    tbody tr:nth-child(odd) {\n"
  "      background-color: #eee;\n"
  "    }\n"
  "    tbody tr:hover {\n"
  "      background-color: #c0e3eb;\n"
  "    }\n"
  "  </style>\n"
  "  <table id=\"meta\">\n"
  "    <thead>\n"
  "      <tr>\n"
  "        <th colspan=\"2\">Meta</th>\n"
  "      </tr>\n"
  "    </thead>\n"
  "    <tbody>\n"
  ;

  if (print_date()) {
    *stream() <<
    "      <tr>\n"
    "        <td>Date</td>\n"
    "        <td>" << CTime() << "</td>\n"
    "      </tr>\n"
    ;
  }

  if (print_run_time()) {
    *stream() <<
    "      <tr>\n"
    "        <td>Duration</td>\n"
    "        <td>" << TimeSpan(stats->GetTotalRunTime()) << "</td>\n"
    "      </tr>\n"
    ;
  }

  *stream() <<
  "    </tbody>\n"
  "  </table>\n"
  "  <br/>\n"
  "  <table id=\"data\" class=\"tablesorter\">\n"
  "    <thead>\n"
  "      <tr>\n"
  "        <th rowspan=\"2\">Type</th>\n"
  "        <th rowspan=\"2\">Name</th>\n"
  "        <th rowspan=\"2\">Calls</th>\n"
  "        <th colspan=\"4\" class=\"group\">Self Time</th>\n"
  "        <th colspan=\"4\" class=\"group\">Total Time</th>\n"
  "      </tr>\n"
  "      <tr>\n"
  "        <th>%</th>\n"
  "        <th>Overall</th>\n"
  "        <th>Average</th>\n"
  "        <th>Worst</th>\n"
  "        <th>%</th>\n"
  "        <th>Overall</th>\n"
  "        <th>Average</th>\n"
  "        <th>Worst</th>\n"
  "      </tr>\n"
  "    </thead>\n"
  "    <tbody>\n"
  ;

  std::vector<FunctionStatistics*> all_fn_stats;
  stats->GetStatistics(all_fn_stats);

  typedef std::vector<FunctionStatistics*>::const_iterator FuncIterator;

  Nanoseconds self_time_all;
  for (FuncIterator it = all_fn_stats.begin(); it != all_fn_stats.end(); ++it) {
    const FunctionStatistics *fn_stats = *it;
    self_time_all += fn_stats->self_time();
  };

  Nanoseconds total_time_all;
  for (FuncIterator it = all_fn_stats.begin(); it != all_fn_stats.end(); ++it) {
    const FunctionStatistics *fn_stats = *it;
    total_time_all += fn_stats->total_time();
  };

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
    << "    <tr>\n"
    << "      <td>" << fn_stats->function()->GetTypeString() << "</td>\n"
    << "      <td>" << fn_stats->function()->name() << "</td>\n"
    << "      <td class=\"numeric\">" << fn_stats->num_calls() << "</td>\n"
    << "      <td class=\"numeric\">" << std::setprecision(2)
                                      << self_time_percent << "%</td>\n"
    << "      <td class=\"numeric\">" << std::setprecision(1)
                                      << self_time << "</td>\n"
    << "      <td class=\"numeric\">" << std::setprecision(1)
                                      << avg_self_time << "</td>\n"
    << "      <td class=\"numeric\">" << std::setprecision(1)
                                      << worst_self_time << "</td>\n"
    << "      <td class=\"numeric\">" << std::setprecision(2)
                                      << total_time_percent << "%</td>\n"
    << "      <td class=\"numeric\">" << std::setprecision(1)
                                      << total_time << "</td>\n"
    << "      <td class=\"numeric\">" << std::setprecision(1)
                                      << avg_total_time << "</td>\n"
    << "      <td class=\"numeric\">" << std::setprecision(1)
                                      << worst_total_time << "</td>\n"
    << "    </tr>\n";
  };

  stream()->flags(flags);

  *stream() <<
  "    </tbody>\n"
  "  </table>\n"
  "</body>\n"
  "</html>\n"
  ;
}

} // namespace amxprof
