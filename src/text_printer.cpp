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

#include <boost/date_time.hpp>

#include "text_printer.h"

namespace samp_profiler {

void TextPrinter::Print(std::ostream &stream, Profile &profile) {
	stream << "Generated on " << boost::posix_time::second_clock::local_time() << "\n" << std::endl;

	stream 
		<< std::setw(kFunctionTypeWidth)     << "Function Type"
		<< std::setw(kFunctionNameWidth)     << "Function Name"
		<< std::setw(kNumberOfCallsWidth)    << "Calls"
		<< std::setw(kTimePerCallWidth)      << "Time Per Call"
		<< std::setw(kTimeWidth)             << "Time"
		<< std::setw(kTimePercentWidth)      << "Time, %"
		<< std::setw(kTotalTimePerCallWidth) << "Total Time Per Call"
		<< std::setw(kTotalTimeWidth)        << "Total Time"
		<< std::setw(kTotalTimePercentWidth) << "Total Time, %"
	<< std::endl;

	TimeType time_all = 0;
	for (Profile::const_iterator it = profile.begin(); it != profile.end(); ++it) {
		time_all += it->GetCounter().GetTime();
	}    

	TimeType total_time_all = 0;
	for (Profile::const_iterator it = profile.begin(); it != profile.end(); ++it) {
		total_time_all += it->GetCounter().GetTotalTime();
	}   

	for (Profile::const_iterator it = profile.begin(); it != profile.end(); ++it) {
		const PerformanceCounter &counter = it->GetCounter();

		stream 
			<< std::setw(kFunctionTypeWidth)     << it->GetFunctionType()
			<< std::setw(kFunctionNameWidth)     << it->GetFunctionName()
			<< std::setw(kNumberOfCallsWidth)    << counter.GetNumberOfCalls()
			<< std::setw(kTimePerCallWidth)      << counter.GetTime() / counter.GetNumberOfCalls()
			<< std::setw(kTimeWidth)             << counter.GetTime()
			<< std::setw(kTimePercentWidth)      << std::setprecision(2) << std::fixed 
				<< static_cast<double>(counter.GetTime() * 100) / time_all
			<< std::setw(kTotalTimePerCallWidth) << counter.GetTotalTime() / counter.GetNumberOfCalls()
			<< std::setw(kTotalTimeWidth)        << counter.GetTotalTime()
			<< std::setw(kTotalTimePercentWidth) << std::setprecision(2) << std::fixed 
				<< static_cast<double>(counter.GetTotalTime() * 100) / total_time_all
		<< std::endl;
	}
}

} // namespace samp_profiler
