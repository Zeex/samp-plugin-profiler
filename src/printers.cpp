// SA:MP Profiler plugin
//
// Copyright (c) 2011 Zeex
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "printers.h"
#include "profile.h"

LogPrinter::LogPrinter(const std::string script_name) 
	: script_name_(script_name)
{
}

void LogPrinter::Print(const std::vector<Profile> &profiles) {
	
}

HtmlPrinter::HtmlPrinter(const std::string &out_file, const std::string &title) 
	: out_file_(out_file)
	, title_(title)
{
}

void HtmlPrinter::Print(const std::vector<Profile> &profiles) {
	std::ofstream stream(out_file_);
	if (!stream.is_open()) return;

	stream << 
	"<html>\n\n"
	"<head>\n"
	;

	if (!title_.empty()) {
		stream << 
		"	<title>" << title_ << "</title>\n"
		;
	}

	stream << 
	"	<script type=\"text/javascript\"\n"
	"		src=\"http://code.jquery.com/jquery-latest.min.js\"></script>\n"
	"	<script type=\"text/javascript\"\n"
	"		src=\"http://autobahn.tablesorter.com/jquery.tablesorter.min.js\"></script>\n"
	"	<script type=\"text/javascript\">\n"
	"	$(document).ready(function() {\n"
	"		$(\"#stats\").tablesorter();\n"
	"	});\n"
	"	</script>\n"
	"</head>\n\n"
	;

	stream << 
	"<body>"
	"	<h1>" << title_ << "</h1>\n"
	"	<table id=\"stats\" class=\"tablesorter\" border=\"1\" width=\"100%\">\n"
	"		<thead>\n"
	"			<tr>\n"
	"				<th>Function Type</th>\n"
	"				<th>Function Name</th>\n"
	"				<th>Calls</th>\n"
	"				<th>Time per call, &#181;s</th>\n"
	"				<th>Overall time, &#181;s</th>\n"
	"				<th>Overall time, &#037;</th>\n"
	"			</tr>\n"
	"		</thead>\n"
	"		<tbody>\n"
	;

	std::int64_t total_time = 0;
	for (std::vector<Profile>::const_iterator it = profiles.begin(); it != profiles.end(); ++it) {
		total_time += it->GetCounter().GetTotalTime();
	}        

	for (std::vector<Profile>::const_iterator it = profiles.begin(); it != profiles.end(); ++it) {
		const PerformanceCounter &counter = it->GetCounter();

		stream 
		<< "		<tr>\n"
		<< "			<td>" << it->GetFunctionType() << "</td>\n"
		<< "			<td>" << it->GetFunctionName() << "</td>\n"
		<< "			<td>" << counter.GetNumberOfCalls() << "</td>\n"
		<< "			<td>" << counter.GetTotalTime() / counter.GetNumberOfCalls() << "</td>\n"
		<< "			<td>" << counter.GetTotalTime() << "</td>\n"
		<< "			<td>" << std::fixed << std::setprecision(2) 
			<< static_cast<double>(counter.GetTotalTime() * 100) / total_time << "</td>\n"
		<< "		</tr>\n";
	}

	stream << 
	"		</tbody>\n"
	"	</table>\n\n"
	;

	stream <<
	"</body>\n"
	"</html>\n"
	;
}
