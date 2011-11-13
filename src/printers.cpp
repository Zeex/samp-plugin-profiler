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

#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "printers.h"
#include "profile.h"

namespace {

using samp_profiler::Profile;
using samp_profiler::TimeType;

TimeType GetTotalRunTime(const Profile &profile) {
	TimeType total_time = 0;
	for (Profile::const_iterator it = profile.begin(); it != profile.end(); ++it) {
		total_time += it->GetCounter().GetTotalTime();
	}   
	return total_time;
}

std::string GetCurrentDateAndTime() {
	static const size_t kBufferSize = 80u;

	std::time_t raw_time;
	std::time(&raw_time);

	std::tm *time_info;
	time_info = std::localtime(&raw_time);

	char buffer[kBufferSize];
	std::strftime(buffer, kBufferSize, "%c", time_info);

	return std::string(buffer);
}

} // namespace 

namespace samp_profiler {

TextPrinter::TextPrinter(const std::string &out_file, const std::string &script_name)
	: out_file_(out_file)
	, script_name_(script_name)
{
}

void TextPrinter::Print(const Profile &profile) {
	std::ofstream stream(out_file_.c_str());
	if (!stream.is_open()) return;	

	stream << "Profile of " << script_name_ << " generated on " 
		<< GetCurrentDateAndTime() << "\n" << std::endl;
	stream 
		<< std::setw(kFunctionTypeWidth)  << "Function Type"
		<< std::setw(kFunctionNameWidth)  << "Function Name"
		<< std::setw(kNumberOfCallsWidth) << "# Calls"
		<< std::setw(kAverageTimeWidth)   << "Average Time"
		<< std::setw(kOverallTimeWidth)   << "Overall Time"
		<< std::setw(kPercentOfTimeWidth) << "Overall Time, %"
	<< std::endl;

	TimeType total_time = GetTotalRunTime(profile);

	for (Profile::const_iterator it = profile.begin(); it != profile.end(); ++it) {
		const PerformanceCounter &counter = it->GetCounter();

		stream 
			<< std::setw(kFunctionTypeWidth)  << it->GetFunctionType()
			<< std::setw(kFunctionNameWidth)  << it->GetFunctionName()
			<< std::setw(kNumberOfCallsWidth) << counter.GetNumberOfCalls()
			<< std::setw(kAverageTimeWidth)   << counter.GetTotalTime() / counter.GetNumberOfCalls()
			<< std::setw(kOverallTimeWidth)   << counter.GetTotalTime()
			<< std::setw(kPercentOfTimeWidth) << std::setprecision(2) << std::fixed 
				<< static_cast<double>(counter.GetTotalTime() * 100) / total_time
		<< std::endl;
	}
}

HtmlPrinter::HtmlPrinter(const std::string &out_file, const std::string &script_name) 
	: out_file_(out_file)
	, script_name_(script_name)
{
}

void HtmlPrinter::Print(const Profile &profile) {
	std::ofstream stream(out_file_.c_str());
	if (!stream.is_open()) return;

	stream << 
	"<html>\n"
	"<head>\n"
	"	<title>" << "Profile of " << script_name_ << "</title>\n"
	"</head>\n\n"
	"<body>"
	"	<h1>" << "Profile of " << script_name_ << " generated on " << GetCurrentDateAndTime() << "</h1>\n"
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

    TimeType total_time = GetTotalRunTime(profile);

	for (Profile::const_iterator it = profile.begin(); it != profile.end(); ++it) {
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
	"	</table>\n"
	;

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
	"</body>\n"
	"</html>\n"
	;
}

} // namespace samp_profiler
