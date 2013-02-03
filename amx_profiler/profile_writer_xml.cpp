// Copyright (c) 2011-2012, Zeex
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "duration.h"
#include "function.h"
#include "function_statistics.h"
#include "performance_counter.h"
#include "profile_writer_xml.h"

namespace amx_profiler {

ProfileWriterXml::ProfileWriterXml(std::ostream *stream, const std::string script_name)
	: stream_(stream)
	, script_name_(script_name)
{
}

void ProfileWriterXml::Write(const std::vector<FunctionStatistics*> &profile)
{
	*stream_ << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
	         << "<profile script=\"" << script_name_ << "\"";
	
	if (print_date()) {
		*stream_ << " timestamp=\"" << std::time(0) << "\"";
	}

	*stream_ << ">\n";

	Duration time_all;
	for (auto stats : profile) {
		time_all += stats->total_time() - stats->child_time(); 
	}

	Duration total_time_all;
	for (auto stats : profile) {
		total_time_all += stats->total_time(); 
	}

	for (auto stats : profile) {
		double self_time_sec = Seconds(stats->GetSelfTime()).count();
		double self_time_percent = stats->GetSelfTime().count() * 100 / time_all.count();
		double total_time_sec = Seconds(stats->total_time()).count();
		double total_time_percent = stats->total_time().count() * 100 / total_time_all.count();
		*stream_ << "\t<function"
			<< " type=\"" << stats->function()->type() << "\""
			<< " name=\"" << stats->function()->name() << "\""
			<< " calls=\"" << stats->num_calls() << "\""
			<< " self_time=\"" << stats->GetSelfTime().count() << "\""
			<< " self_time_sec=\"" << self_time_sec << "\""
			<< " self_time_percent=\"" <<  std::fixed << std::setprecision(2) << self_time_percent << "\""
			<< " total_time=\"" << stats->total_time().count() << "\""
			<< " total_time_sec=\"" << total_time_sec << "\""
			<< " total_time_percent=\"" <<  std::fixed << std::setprecision(2) << total_time_percent << "\""
		<< "/>\n";
	}

	*stream_ << "</profile>";
}

} // namespace amx_profiler

