// Copyright (c) 2011-2013, Zeex
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

namespace amx_profiler {

void StatisticsWriterHtml::Write(const Statistics *stats)
{
	*stream() <<
	"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n"
	"\"http://www.w3.org/TR/html4/loose.dtd\">\n"
	"<html>\n"
	"<head>\n"
	"	<title>" << "Profile of '" << script_name() << "'</title>\n"
	"</head>\n"
	"<body>\n";

	*stream() <<
	"	<style type=\"text/css\">\n"
	"	table.meta {\n"
	"		width: auto;\n"
	"	}\n"
	"	table {\n"
	"		border-spacing:0;\n"
	"		border-collapse: collapse;\n"
	"	}\n"
	"	table, th, td {\n"
	"		border: 1px solid #cccccc;\n"
	"	}\n"
	"	td {\n"
	"		font-family: Consolas, \"DejaVu Sans Mono\", \"Courier New\", Monospace;\n"
	"		border: solid 5x transparent;\n"
	"	}\n"
	"	th {\n"
	"		background-color: #cccccc;\n"
	"	}\n"
	"	</style>\n"
	"	<table class=\"meta\" border=\"1\">\n"
	"		<thead>\n"
	"			<tr>\n"
	"				<th>Name</th>\n"
	"				<th>Value</th>\n"
	"			</tr>\n"
	"		</thead>\n"
	"		<tbody>\n";

	if (print_date()) {
		*stream() <<
		"			<tr>\n"
		"				<td>Date</td>\n"
		"				<td>" << CTime() << "</td>\n"
		"			</tr>\n";
	}

	if (print_run_time()) {
		*stream() <<
		"			<tr>\n"
		"				<td>Duration</td>\n"
		"				<td>" << TimeSpan(stats->GetTotalRunTime()) << "</td>\n"
		"			</tr>\n";
	}

	*stream() <<
	"		</tbody>\n"
	"	</table>\n"
	"	<br/>"
	"	<style type=\"text/css\">\n"
	"	tr:nth-child(odd) {\n"
	"		background-color: #eeeeee;\n"
	"	}\n"
	"	</style>\n"
	"	<table id=\"func-stats\" class=\"tablesorter\" border=\"1\" width=\"100%\">\n"
	"		<thead>\n"
	"			<tr>\n"
	"				<th rowspan=\"2\">Type</th>\n"
	"				<th rowspan=\"2\">Name</th>\n"
	"				<th rowspan=\"2\">Calls</th>\n"
	"				<th colspan=\"3\">Self Time</th>\n"
	"				<th colspan=\"3\">Total Time</th>\n"
	"			</tr>\n"
	"			<tr>\n"
	"				<th align=\"left\">%</th>\n"
	"				<th align=\"left\">whole</th>\n"
	"				<th align=\"left\">average</th>\n"
	"				<th align=\"left\">%</th>\n"
	"				<th align=\"left\">whole</th>\n"
	"				<th align=\"left\">average</th>\n"
	"			</tr>\n"
	"		</thead>\n"
	"		<tbody>\n"
	;

	std::vector<FunctionStatistics*> all_fn_stats;
	stats->GetStatistics(all_fn_stats);

	Nanoseconds self_time_all;
	for (std::vector<FunctionStatistics*>::const_iterator iterator = all_fn_stats.begin();
			iterator != all_fn_stats.end(); ++iterator)
	{
		const FunctionStatistics *fn_stats = *iterator;
		self_time_all += fn_stats->self_time();
	};

	Nanoseconds total_time_all;
	for (std::vector<FunctionStatistics*>::const_iterator iterator = all_fn_stats.begin();
			iterator != all_fn_stats.end(); ++iterator)
	{
		const FunctionStatistics *fn_stats = *iterator;
		total_time_all += fn_stats->total_time(); 
	};

	for (std::vector<FunctionStatistics*>::const_iterator iterator = all_fn_stats.begin();
			iterator != all_fn_stats.end(); ++iterator)
	{
		const FunctionStatistics *fn_stats = *iterator;

		double self_time_percent = fn_stats->self_time().count() * 100 / self_time_all.count();
		double self_time = Seconds(fn_stats->self_time()).count();
		double avg_self_time = Milliseconds(fn_stats->self_time()).count() / fn_stats->num_calls();

		double total_time_percent = fn_stats->total_time().count() * 100 / total_time_all.count();
		double total_time = Seconds(fn_stats->total_time()).count();
		double avg_total_time = Milliseconds(fn_stats->total_time()).count() / fn_stats->num_calls();

		*stream()
		<< "		<tr>\n"
		<< "			<td>" << fn_stats->function()->GetTypeString() << "</td>\n"
		<< "			<td>" << fn_stats->function()->name() << "</td>\n"
		<< "			<td>" << fn_stats->num_calls() << "</td>\n"
		<< "			<td>" << std::fixed << std::setprecision(2) << self_time_percent << "%</td>\n"
		<< "			<td>" << std::fixed << std::setprecision(1) << self_time << "</td>\n"
		<< "			<td>" << std::fixed << std::setprecision(1) << avg_self_time << "</td>\n"
		<< "			<td>" << std::fixed << std::setprecision(2) << total_time_percent << "%</td>\n"
		<< "			<td>" << std::fixed << std::setprecision(1) << total_time << "</td>\n"
		<< "			<td>" << std::fixed << std::setprecision(1) << avg_total_time << "</td>\n"
		<< "			</td>\n"
		<< "		</tr>\n";

	};

	*stream() <<
	"		</tbody>\n"
	"	</table>\n"
	;

	*stream() <<
	"	<script type=\"text/javascript\"\n"
	"		src=\"http://code.jquery.com/jquery-latest.min.js\"></script>\n"
	"	<script type=\"text/javascript\"\n"
	"		src=\"http://tablesorter.com/__jquery.tablesorter.min.js\"></script>\n"
	"	<script type=\"text/javascript\">\n"
	"	$(document).ready(function() {\n"
	"		$(\"#func-stats\").tablesorter();\n"
	"	});\n"
	"	</script>\n"
	"	<br/>\n"
	"	<footer>\n"
	;

	*stream() <<
	"	</footer>\n"
	"</body>\n"
	"</html>\n"
	;
}

} // namespace amx_profiler
