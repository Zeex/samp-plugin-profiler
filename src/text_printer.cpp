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
#include "profile.h"

namespace samp_profiler {

void TextPrinter::Print(const std::string &script_name, std::ostream &stream, Profile &profile) {
	stream << "Profile of '" << script_name 
		<< "' generated on " << boost::posix_time::second_clock::local_time() << "\n" << std::endl;

	stream 
		<< std::setw(kTypeWidth) << "Type"
		<< std::setw(kNameWidth) << "Name"
		<< std::setw(kCallsWidth) << "Calls"
		<< std::setw(kSelfTimeWidth) << "Self Time"
		<< std::setw(kTotalTimeWidth) << "Total Time"
	<< std::endl;

	ProfileEntry::Time time_all = 0;
	for (Profile::const_iterator it = profile.begin(); it != profile.end(); ++it) {
		time_all += it->self_time() - it->child_time();
	}    

	ProfileEntry::Time total_time_all = 0;
	for (Profile::const_iterator it = profile.begin(); it != profile.end(); ++it) {
		total_time_all += it->self_time();
	}

	for (Profile::const_iterator it = profile.begin(); it != profile.end(); ++it) {
		stream 
			<< std::setw(kTypeWidth) << it->function_type()
			<< std::setw(kNameWidth) << it->function_name()
			<< std::setw(kCallsWidth) << it->num_calls()
			<< std::setw(kSelfTimeWidth) << std::setprecision(2) << std::fixed 
				<< static_cast<double>((it->self_time() - it->child_time()) * 100) / time_all
			<< std::setw(kTotalTimeWidth) << std::setprecision(2) << std::fixed 
				<< static_cast<double>(it->self_time() * 100) / total_time_all
		<< std::endl;
	}
}

} // namespace samp_profiler
