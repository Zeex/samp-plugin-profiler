// AMX profiler for SA-MP server: http://sa-mp.com
//
// Copyright (C) 2011-2012 Sergey Zolotarev
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
#include <vector>
#include <boost/date_time.hpp>
#include "function.h"
#include "function_info.h"
#include "timer.h"
#include "xml_profile_writer.h"

namespace amx_profiler {

void XmlProfileWriter::Write(const std::string &script_name, std::ostream &stream, 
	const std::vector<const FunctionInfo*> &stats) 
{
	stream << 
	"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
	"<profile script=\"" << script_name << "\""
		"date=\"" << boost::posix_time::second_clock::local_time() << "\">";

	Timer::TimeType time_all = 0;
	for (std::vector<const FunctionInfo*>::const_iterator iterator = stats.begin();
			iterator != stats.end(); ++iterator) {
		time_all += (*iterator)->total_time() - (*iterator)->child_time();
	}    

	Timer::TimeType total_time_all = 0;
	for (std::vector<const FunctionInfo*>::const_iterator iterator = stats.begin();
			iterator != stats.end(); ++iterator) {
		total_time_all += (*iterator)->total_time();
	}

	for (std::vector<const FunctionInfo*>::const_iterator iterator = stats.begin();
			iterator != stats.end(); ++iterator) {
		stream << "		<function";
		stream << " type=\"" << (*iterator)->function()->type() << "\"";
		stream << " name=\"" << (*iterator)->function()->name() << "\"";
		stream << " calls=\"" << (*iterator)->num_calls() << "\"";
		stream << " total_time=\"" <<  std::fixed << std::setprecision(2) 
			<< static_cast<double>(((*iterator)->total_time() - (*iterator)->child_time()) * 100) / time_all << "\"";
		stream << " total_time=\"" <<  std::fixed << std::setprecision(2) 
			<< static_cast<double>((*iterator)->total_time() * 100) / total_time_all << "\"";
		stream << " />\n";
	}

	stream << "</profile>";
}

} // namespace amx_profiler

