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

#include "profile.h"
#include "xml_printer.h"

namespace samp_profiler {

void XmlPrinter::Print(const std::string &script_name, std::ostream &stream, Profile &profile) {
	stream << 
	"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
	"<profile script=\"" << script_name << "\""
		"date=\"" << boost::posix_time::second_clock::local_time() << "\">";

	ProfileEntry::Time time_all = 0;
	for (Profile::const_iterator it = profile.begin(); it != profile.end(); ++it) {
		time_all += it->self_time() - it->child_time();
	}    

	ProfileEntry::Time total_time_all = 0;
	for (Profile::const_iterator it = profile.begin(); it != profile.end(); ++it) {
		total_time_all += it->self_time();
	}

	for (Profile::const_iterator it = profile.begin(); it != profile.end(); ++it) {
		stream << "		<function";
		stream << " type=\"" << it->function_type() << "\"";
		stream << " name=\"" << it->function_name() << "\"";
		stream << " calls=\"" << it->num_calls() << "\"";
		stream << " self_time=\"" <<  std::fixed << std::setprecision(2) 
			<< static_cast<double>((it->self_time() - it->child_time()) * 100) / time_all << "\"";
		stream << " total_time=\"" <<  std::fixed << std::setprecision(2) 
			<< static_cast<double>(it->self_time() * 100) / total_time_all << "\"";
		stream << " />\n";
	}

	stream << "</profile>";
}

} // namespace samp_profiler

