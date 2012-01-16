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

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#ifdef HAVE_BOOST_DATE_TIME
	#include <boost/date_time.hpp>
#endif
#include "function.h"
#include "function_info.h"
#include "performance_counter.h"
#include "text_profile_writer.h"

namespace amx_profiler {

void TextProfileWriter::Write(const std::string &script_name, std::ostream &stream,
		const std::vector<FunctionInfoPtr> &stats)
{
	stream << "Profile of '" << script_name << "'";
	#ifdef HAVE_BOOST_DATE_TIME
		stream << " generated on " << boost::posix_time::second_clock::local_time() << "\n" << std::endl;
	#endif

	stream
		<< std::setw(kTypeWidth) << "Type"
		<< std::setw(kNameWidth) << "Name"
		<< std::setw(kCallsWidth) << "Calls"
		<< std::setw(kSelfTimeWidth) << "Self Time"
		<< std::setw(kTotalTimeWidth) << "Total Time"
	<< std::endl;

	TimeInterval time_all = 0;
	std::for_each(stats.begin(), stats.end(), [&](const FunctionInfoPtr &info) {
		time_all += info->total_time() - info->child_time();
	});

	TimeInterval total_time_all = 0;
	std::for_each(stats.begin(), stats.end(), [&](const FunctionInfoPtr &info) {
		total_time_all += info->total_time();
	});

	std::for_each(stats.begin(), stats.end(), [&](const FunctionInfoPtr &info) {
		stream
			<< std::setw(kTypeWidth) << info->function()->type()
			<< std::setw(kNameWidth) << info->function()->name()
			<< std::setw(kCallsWidth) << info->num_calls()
			<< std::setw(kSelfTimeWidth) << std::setprecision(2) << std::fixed
				<< static_cast<double>((info->total_time() - info->child_time()) * 100) / time_all
			<< std::setw(kTotalTimeWidth) << std::setprecision(2) << std::fixed
				<< static_cast<double>(info->total_time() * 100) / total_time_all
		<< std::endl;
	});
}

} // namespace amx_profiler
