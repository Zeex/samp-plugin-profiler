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
#include <string>

#include <boost/date_time.hpp>

#include "text_printer.h"

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
		<< boost::posix_time::second_clock::local_time();
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

} // namespace samp_profiler
