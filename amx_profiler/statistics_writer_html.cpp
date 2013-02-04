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

#include <cstdio>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>
#include "duration.h"
#include "function.h"
#include "function_statistics.h"
#include "statistics_writer_html.h"
#include "performance_counter.h"
#include "statistics.h"

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
	"<body>\n"
	"	<style type=\"text/css\">\n"
	"	table {\n"
	"		border-collapse: collapse;\n"
	"	}\n"
	"	table, th, td {\n"
	"		border: 1px solid #cccccc;\n"
	"	}\n"
	"	td {\n"
	"		font-family: Consolas, \"Courier New\", \"Monospace\";\n"
	"	}\n"
	"	th {\n"
	"		background-color: #cccccc;\n"
	"	}\n"
	"	tr:nth-child(odd) {\n"
	"		background-color: #eeeeee;\n"
	"	}\n"
	"	</style>\n"
	"	<table id=\"fn_stats\" class=\"tablesorter\" border=\"1\" width=\"100%\">\n"
	"		<thead>\n"
	"			<tr>\n"
	"				<th>Type</th>\n"
	"				<th>Name</th>\n"
	"				<th>Calls</th>\n"
	"				<th colspan=\"2\">Self Time</th>\n"
	"				<th colspan=\"2\">Total Time</th>\n"
	"			</tr>\n"
	"		</thead>\n"
	"		<tbody>\n"
	;

	Duration time_all;
	stats->EnumerateFunctions([&](const FunctionStatistics *fn_stats) {
		time_all += fn_stats->total_time() - fn_stats->child_time(); 
	});

	Duration total_time_all;
	stats->EnumerateFunctions([&](const FunctionStatistics *fn_stats) {
		total_time_all += fn_stats->total_time(); 
	});

	stats->EnumerateFunctions([&](const FunctionStatistics *fn_stats) {
		double self_time_sec = Seconds(fn_stats->GetSelfTime()).count();
		double self_time_percent = fn_stats->GetSelfTime().count() * 100 / time_all.count();
		double total_time_sec = Seconds(fn_stats->total_time()).count();
		double total_time_percent = fn_stats->total_time().count() * 100 / total_time_all.count();
		*stream()
		<< "		<tr>\n"
		<< "			<td>" << fn_stats->function()->type() << "</td>\n"
		<< "			<td>" << fn_stats->function()->name() << "</td>\n"
		<< "			<td>" << fn_stats->num_calls() << "</td>\n"
		<< "			<td>" << std::fixed << std::setprecision(2) << self_time_percent << "%</td>\n"
		<< "			<td>" << std::fixed << std::setprecision(3) << self_time_sec << "s</td>\n"
		<< "			<td>" << std::fixed << std::setprecision(2) << total_time_percent << "%</td>\n"
		<< "			<td>" << std::fixed << std::setprecision(3) << total_time_sec << "s</td>\n"
		<< "			</td>\n"
		<< "		</tr>\n";
	});

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
	"		$(\"#fn_stats\").tablesorter();\n"
	"	});\n"
	"	</script>\n"
	"	<br/>\n"
	"	<footer>\n";

	if (print_date()) {
		std::time_t now = std::time(nullptr);
		*stream() <<
		"		Generated on " << std::ctime(&now);
	}

	*stream() <<
	"	</footer>\n";
	"</body>\n"
	"</html>\n"
	;
}

} // namespace amx_profiler
