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

void TextPrinter::Print(Profile &profile) {
	std::ofstream stream(out_file_.c_str());
	if (!stream.is_open()) return;	

	switch (sort_mode_) {
	case SORT_BY_TIME:
		std::sort(profile.begin(), profile.end(), ProfileEntry::CompareTime);
		break;
	case SORT_BY_CALLS:
		std::sort(profile.begin(), profile.end(), ProfileEntry::CompareCalls);
		break;
	case SORT_BY_TOTAL_TIME:
		std::sort(profile.begin(), profile.end(), ProfileEntry::CompareTotalTime);
		break;
	}	

	stream << "Profile of " << script_name_ << " generated on " 
		<< GetCurrentDateAndTime();
	if (!sub_child_time_) {
		stream << " (with child time included)";
	} 
	stream << "\n" << std::endl;

	stream 
		<< std::setw(kFunctionTypeWidth)  << "Function Type"
		<< std::setw(kFunctionNameWidth)  << "Function Name"
		<< std::setw(kNumberOfCallsWidth) << "Calls"
		<< std::setw(kAverageTimeWidth)   << "Average Time"
		<< std::setw(kOverallTimeWidth)   << "Overall Time"
		<< std::setw(kPercentOfTimeWidth) << "Overall Time, %"
	<< std::endl;

	TimeType overall_time = 0;
	for (Profile::const_iterator it = profile.begin(); it != profile.end(); ++it) {
		if (!sub_child_time_) {
			overall_time += it->GetCounter().GetTotalTime();
		} else {
			overall_time += it->GetCounter().GetTime();
		}
	}   

	for (Profile::const_iterator it = profile.begin(); it != profile.end(); ++it) {
		const PerformanceCounter &counter = it->GetCounter();

		TimeType time;
		if (sub_child_time_) {
			time = counter.GetTime();
		} else {
			time = counter.GetTotalTime();
		}

		stream 
			<< std::setw(kFunctionTypeWidth)  << it->GetFunctionType()
			<< std::setw(kFunctionNameWidth)  << it->GetFunctionName()
			<< std::setw(kNumberOfCallsWidth) << counter.GetNumberOfCalls()
			<< std::setw(kAverageTimeWidth)   << time / counter.GetNumberOfCalls()
			<< std::setw(kOverallTimeWidth)   << time
			<< std::setw(kPercentOfTimeWidth) << std::setprecision(2) << std::fixed 
				<< static_cast<double>(time * 100) / overall_time
		<< std::endl;
	}
}

void HtmlPrinter::Print(Profile &profile) {
	std::ofstream stream(out_file_.c_str());
	if (!stream.is_open()) return;

	switch (sort_mode_) {
	case SORT_BY_TIME:
		std::sort(profile.begin(), profile.end(), ProfileEntry::CompareTime);
		break;
	case SORT_BY_CALLS:
		std::sort(profile.begin(), profile.end(), ProfileEntry::CompareCalls);
		break;
	case SORT_BY_TOTAL_TIME:
		std::sort(profile.begin(), profile.end(), ProfileEntry::CompareTotalTime);
		break;
	}	

	stream << 
	"<html>\n"
	"<head>\n"
	"	<title>" << "Profile of " << script_name_ << "</title>\n"
	"</head>\n\n"
	"<body>"
	"	<h1>" << "Profile of " << script_name_ << " generated on " << GetCurrentDateAndTime();
	if (!sub_child_time_) {
		stream << " (with child time included)";
	} 
	stream << "</h1>\n"
	"	<table id=\"stats\" class=\"tablesorter\" border=\"1\" width=\"100%\">\n"
	"		<thead>\n"
	"			<tr>\n"
	"				<th>Function Type</th>\n"
	"				<th>Function Name</th>\n"
	"				<th>Calls</th>\n"
	"				<th>Average Time, &#181;s</th>\n"
	"				<th>Overall Time, &#181;s</th>\n"
	"				<th>Overall Time, &#037;</th>\n"
	"			</tr>\n"
	"		</thead>\n"
	"		<tbody>\n"
	;

	TimeType overall_time = 0;
	for (Profile::const_iterator it = profile.begin(); it != profile.end(); ++it) {
		if (!sub_child_time_) {
			overall_time += it->GetCounter().GetTotalTime();
		} else {
			overall_time += it->GetCounter().GetTime();
		}
	}    

	for (Profile::const_iterator it = profile.begin(); it != profile.end(); ++it) {
		const PerformanceCounter &counter = it->GetCounter();

		TimeType time;
		if (sub_child_time_) {
			time = counter.GetTime();
		} else {
			time = counter.GetTotalTime();
		}

		stream 
		<< "		<tr>\n"
		<< "			<td>" << it->GetFunctionType() << "</td>\n"
		<< "			<td>" << it->GetFunctionName() << "</td>\n"
		<< "			<td>" << counter.GetNumberOfCalls() << "</td>\n"
		<< "			<td>" << time / counter.GetNumberOfCalls() << "</td>\n"
		<< "			<td>" << time << "</td>\n"
		<< "			<td>" << std::fixed << std::setprecision(2) 
			<< static_cast<double>(time * 100) / overall_time << "</td>\n"
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
