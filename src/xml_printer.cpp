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

#include "xml_printer.h"

namespace samp_profiler {

void XmlPrinter::Print(std::ostream &stream, Profile &profile) {
	stream << 
	"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
	"<profile date=\"" << boost::posix_time::second_clock::local_time() << "\">";

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

		stream << "		<function";
		stream << " type=\"" << it->GetFunctionType() << "\"";
		stream << " name=\"" << it->GetFunctionName() << "\"";
		stream << " calls=\"" << counter.GetNumberOfCalls() << "\"";
		stream << " mean_time=\"" << counter.GetTime() / counter.GetNumberOfCalls() << "\"";
		stream << " total_time=\"" << counter.GetTime() << "\"";
		stream << " total_time_percent=\"" <<  std::fixed << std::setprecision(2) 
			<< static_cast<double>(counter.GetTime() * 100) / time_all << "\"";
		stream << " mean_full_time=\"" << counter.GetTotalTime() / counter.GetNumberOfCalls() << "\"";
		stream << " total_full_time=\"" << counter.GetTotalTime() << "\"";
		stream << " total_full_time_percent=\"" <<  std::fixed << std::setprecision(2) 
			<< static_cast<double>(counter.GetTotalTime() * 100) / total_time_all << "\"";
		stream << " />\n";
	}

	stream << "</profile>";
}

} // namespace samp_profiler

