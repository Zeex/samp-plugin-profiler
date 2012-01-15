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
#include <iomanip>
#include <string>
#include <vector>
#include "function.h"
#include "function_info.h"
#include "performance_counter.h"
#include "text_profile_writer.h"

namespace amx_profiler {

void TextProfileWriter::Write(const std::string &script_name, std::ostream &stream,
		const std::vector<const FunctionInfo*> &stats)
{
	stream << "Profile of '" << script_name;

	stream
		<< std::setw(kTypeWidth) << "Type"
		<< std::setw(kNameWidth) << "Name"
		<< std::setw(kCallsWidth) << "Calls"
		<< std::setw(kSelfTimeWidth) << "Self Time"
		<< std::setw(kTotalTimeWidth) << "Total Time"
	<< std::endl;

	TimeInterval time_all = 0;
	for (std::vector<const FunctionInfo*>::const_iterator iterator = stats.begin();
			iterator != stats.end(); ++iterator) {
		time_all += (*iterator)->total_time() - (*iterator)->child_time();
	}

	TimeInterval total_time_all = 0;
	for (std::vector<const FunctionInfo*>::const_iterator iterator = stats.begin();
			iterator != stats.end(); ++iterator) {
		total_time_all += (*iterator)->total_time();
	}

	for (std::vector<const FunctionInfo*>::const_iterator iterator = stats.begin();
			iterator != stats.end(); ++iterator) {
		stream
			<< std::setw(kTypeWidth) << (*iterator)->function()->type()
			<< std::setw(kNameWidth) << (*iterator)->function()->name()
			<< std::setw(kCallsWidth) << (*iterator)->num_calls()
			<< std::setw(kSelfTimeWidth) << std::setprecision(2) << std::fixed
				<< static_cast<double>(((*iterator)->total_time() - (*iterator)->child_time()) * 100) / time_all
			<< std::setw(kTotalTimeWidth) << std::setprecision(2) << std::fixed
				<< static_cast<double>((*iterator)->total_time() * 100) / total_time_all
		<< std::endl;
	}
}

} // namespace amx_profiler
