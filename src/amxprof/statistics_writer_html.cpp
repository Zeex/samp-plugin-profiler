// Copyright (c) 2011-2019 Zeex
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
  *stream() << "\
<!DOCTYPE html>\n\
<html>\n\
<head>\n\
  <title>Profile of 'gamemodes/grandlarc.amx'</title>\n\
  <meta charset=\"UTF-8\">\n\
\n\
  <script type=\"text/javascript\">\n\
    var intComparator = function(a, b) { \n\
      return parseInt(a) - parseInt(b); \n\
    };\n\
    var floatComparator = function(a, b) { \n\
      return parseFloat(a) - parseFloat(b); \n\
    };\n\
    var stringComparator = function(a, b) {\n\
      return String(a).localeCompare(b);\n\
    };\n\
\n\
    function sort(table, colIndex, reverseOrder) {\n\
      var tbody = table.tBodies[0];\n\
      var rows = Array.prototype.slice.call(tbody.getElementsByTagName('tr'));\n\
\n\
      for (var i = 0; i < rows.length; i++) {\n\
        var row = rows[i];\n\
        var value = row.cells[colIndex].innerHTML;\n\
        var comparator;\n\
        if (/^[0-9]+/.test(value)) {\n\
          comparator = intComparator;\n\
        } else if (/^[0-9.]+/.test(value)) {\n\
          comparator = floatComparator;\n\
        } else {\n\
          comparator = stringComparator;\n\
        }\n\
        var sign = reverseOrder ? -1 : 1;\n\
        for (var j = 0; j < i; j++) {\n\
          var otherRow = rows[j];\n\
          var otherValue = otherRow.cells[colIndex].innerHTML;\n\
          if (sign * comparator(value, otherValue) < 0) {\n\
            tbody.removeChild(row);\n\
            rows.splice(i, 1);\n\
            tbody.insertBefore(row, otherRow);\n\
            rows.splice(j, 0, row);\n\
            break;\n\
          }\n\
        }\n\
      }\n\
    }\n\
\n\
    document.addEventListener('DOMContentLoaded', function() {\n\
      var asc = {};\n\
      var table = document.getElementById('data');\n\
      var headRows = table.tHead.getElementsByTagName('tr');\n\
\n\
      for (var i = 0; i < headRows.length; i++) {\n\
        var row = headRows[i];\n\
        for (var j = 0; j < row.cells.length; j++) {\n\
          var cell = row.cells[j];\n\
          var arrow = document.createElement('span');\n\
          arrow.className = 'sort-direction';\n\
          cell.appendChild(arrow);\n\
          cell.style.cursor = 'pointer';\n\
          (function(cell, arrow) {\n\
            var colIndex = cell.dataset.sortIndex;\n\
            cell.addEventListener('click', function() {\n\
              sort(table, colIndex, !!asc[colIndex]);\n\
              for (var i = 0; i < headRows.length; i++) {\n\
                var row = headRows[i];\n\
                for (var j = 0; j < row.cells.length; j++) {\n\
                  var arrows = \n\
                    row.cells[j].getElementsByClassName('sort-direction');\n\
                  for (var k = 0; k < arrows.length; k++) {\n\
                    arrows[k].style.display = 'none';\n\
                  }\n\
                }\n\
              }\n\
              arrow.innerHTML = asc[colIndex] ? '&#9660;' : '&#9650;';\n\
              arrow.style.display = 'inline';\n\
              asc[colIndex] = !asc[colIndex];\n\
            });\n\
          })(cell, arrow);\n\
        }\n\
      }\n\
    });\n\
  </script>\n\
  <style type=\"text/css\">\n\
    * {\n\
      font-family: sans-serif;\n\
    }\n\
    table {\n\
      margin: 30px auto;\n\
    }\n\
    table, th, td {\n\
      border: 1px solid #505050;\n\
      border-collapse: collapse;\n\
      padding: 7px 10px;\n\
    }\n\
    th {\n\
      color: white;\n\
      background-color: #505050;\n\
      border: 1px solid #707070;\n\
    }\n\
    td {\n\
      border: 1px solid #d8d8d8;\n\
      font-family: 'Source Code Pro', Consolas, \"DejaVu Sans Mono\", \"Courier New\", Monospace;\n\
    }\n\
    td.numeric {\n\
      text-align: right;\n\
    }\n\
    tbody tr:nth-child(odd) {\n\
      background-color: #f0f0f0;\n\
    }\n\
    tbody tr:hover {\n\
      background-color: #c0e3eb;\n\
    }\n\
  </style>\n\
</head>\n\
\n\
<body>\n\
  <table id=\"meta\">\n\
    <thead>\n\
      <tr>\n\
        <th colspan=\"2\">Meta</th>\n\
      </tr>\n\
    </thead>\n\
    <tbody>";

  if (print_date()) {
    *stream() << "\
      <tr>\n\
        <td>Date</td>\n\
        <td>" << CTime() << "</td>\n\
      </tr>\n";
  }

  if (print_run_time()) {
    *stream() << "\
      <tr>\n\
        <td>Duration</td>\n\
        <td>" << TimeSpan(stats->GetTotalRunTime()) << "</td>\n\
      </tr>\n";
  }

  *stream() << "\
</tbody>\n\
  </table>\n\
  <table id=\"data\" class=\"tablesorter\">\n\
    <thead>\n\
      <tr>\n\
        <th rowspan=\"2\" data-sort-index=\"0\">Type</th>\n\
        <th rowspan=\"2\" data-sort-index=\"1\">Name</th>\n\
        <th rowspan=\"2\" data-sort-index=\"2\">Calls</th>\n\
        <th colspan=\"4\" data-sort-index=\"3\" class=\"group\">Self Time</th>\n\
        <th colspan=\"4\" data-sort-index=\"7\" class=\"group\">Total Time</th>\n\
      </tr>\n\
      <tr>\n\
        <th data-sort-index=\"3\">%</th>\n\
        <th data-sort-index=\"4\">Overall</th>\n\
        <th data-sort-index=\"5\">Average</th>\n\
        <th data-sort-index=\"6\">Worst</th>\n\
        <th data-sort-index=\"7\">%</th>\n\
        <th data-sort-index=\"8\">Overall</th>\n\
        <th data-sort-index=\"9\">Average</th>\n\
        <th data-sort-index=\"10\">Worst</th>\n\
      </tr>\n\
    </thead>\n\
    <tbody>\n";

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

  *stream() << "\
    </tbody>\n\
  </table>\n\
</body>\n\
</html>\n";
}

} // namespace amxprof
