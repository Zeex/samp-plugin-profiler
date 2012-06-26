// Copyright (c) 2011-2012, Zeex
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met: 
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer. 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <ctime>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "function.h"
#include "function_statistics.h"
#include "profile_writer_html.h"
#include "performance_counter.h"

namespace amx_profiler {

ProfileWriterHtml::ProfileWriterHtml(std::ostream *stream, const std::string script_name)
	: stream_(stream)
	, script_name_(script_name)
{
}

void ProfileWriterHtml::Write(const std::vector<FunctionStatistics*> profile)
{
	*stream_ <<
	"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n"
	"\"http://www.w3.org/TR/html4/loose.dtd\">\n"
	"<html>\n"
	"<head>\n"
	"	<title>" << "Profile of '" << script_name_ << "'</title>\n"
	"</head>\n"
	"<body>\n"
	"	<h1>\n" <<
	"		Profile of '" << script_name_ << "'\n"
	"	</h1>\n"
	"	<table id=\"stats\" class=\"tablesorter\" border=\"1\" width=\"100%\">\n"
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

	TimeInterval time_all = 0;
	for (auto stats : profile) {
		time_all += stats->total_time() - stats->child_time(); 
	}

	TimeInterval total_time_all = 0;
	for (auto stats : profile) {
		total_time_all += stats->total_time(); 
	}

	for (auto stats : profile) {
		double self_time_sec = static_cast<double>(stats->GetSelfTime()) / 1E+9;
		double self_time_percent = static_cast<double>(stats->GetSelfTime() * 100) / time_all;
		double total_time_sec = static_cast<double>(stats->total_time()) / 1E+9;
		double total_time_percent =  static_cast<double>(stats->total_time() * 100) / total_time_all;
		*stream_
		<< "		<tr>\n"
		<< "			<td>" << stats->function()->type() << "</td>\n"
		<< "			<td>" << stats->function()->name() << "</td>\n"
		<< "			<td>" << stats->num_calls() << "</td>\n"
		<< "			<td>" << std::fixed << std::setprecision(2) << self_time_percent << "%</td>\n"
		<< "			<td>" << std::fixed << std::setprecision(3) << self_time_sec << "s</td>\n"
		<< "			<td>" << std::fixed << std::setprecision(2) << total_time_percent << "%</td>\n"
		<< "			<td>" << std::fixed << std::setprecision(3) << total_time_sec << "s</td>\n"
		<< "			</td>\n"
		<< "		</tr>\n";
	}

	*stream_ <<
	"		</tbody>\n"
	"	</table>\n"
	;

	std::time_t now = std::time(0);

	*stream_ <<
	"	<script type=\"text/javascript\"\n"
	"		src=\"http://code.jquery.com/jquery-latest.min.js\"></script>\n"
	"	<script type=\"text/javascript\"\n"
	"		src=\"http://tablesorter.com/__jquery.tablesorter.min.js\"></script>\n"
	"	<script type=\"text/javascript\">\n"
	"	$(document).ready(function() {\n"
	"		$(\"#stats\").tablesorter();\n"
	"	});\n"
	"	</script>\n"
	"	<br/>\n"
	"	<footer>\n"
	"		Generated on " << ctime(&now) <<
	"	</footer>\n"
	"</body>\n"
	"</html>\n"
	;
}

} // namespace amx_profiler
