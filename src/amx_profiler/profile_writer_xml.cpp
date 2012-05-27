// Copyright (c) 2011-2012, Zeex
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met: 
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer. 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// // LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#ifdef HAVE_BOOST_DATE_TIME
	#include <boost/date_time.hpp>
#endif
#include "function.h"
#include "function_info.h"
#include "performance_counter.h"
#include "profile_writer_xml.h"

namespace amx_profiler {

ProfileWriterXml::ProfileWriterXml(std::ostream *stream, const std::string script_name)
	: stream_(stream)
	, script_name_(script_name)
{
}

void ProfileWriterXml::Write(const std::vector<std::shared_ptr<FunctionInfo>> &stats)
{
	*stream_ <<
	"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
	"<profile script=\"" << script_name_ << "\""
	#ifdef HAVE_BOOST_DATE_TIME
		<< "date=\"" << boost::posix_time::second_clock::local_time() << "\">";
	#else
		<< ">";
	#endif

	TimeInterval time_all = 0;
	std::for_each(stats.begin(), stats.end(), [&](const std::shared_ptr<FunctionInfo> &info) { 
		time_all += info->total_time() - info->child_time(); 
	});

	TimeInterval total_time_all = 0;
	std::for_each(stats.begin(), stats.end(), [&](const std::shared_ptr<FunctionInfo> &info) { 
		total_time_all += info->total_time(); 
	});

	std::for_each(stats.begin(), stats.end(), [&](const std::shared_ptr<FunctionInfo> &info) {
		*stream_ << "		<function";
		*stream_ << " type=\"" << info->function()->type() << "\"";
		*stream_ << " name=\"" << info->function()->name() << "\"";
		*stream_ << " calls=\"" << info->num_calls() << "\"";
		*stream_ << " self_time=\"" <<  std::fixed << std::setprecision(2)
			<< static_cast<double>(info->GetSelfTime() * 100) / time_all << "\"";
		*stream_ << " total_time=\"" <<  std::fixed << std::setprecision(2)
			<< static_cast<double>(info->total_time() * 100) / total_time_all << "\"";
		*stream_ << " />\n";
	});

	*stream_ << "</profile>";
}

} // namespace amx_profiler

