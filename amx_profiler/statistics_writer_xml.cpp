// Copyright (c) 2011-2013, Zeex
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

#include <iomanip>
#include <iostream>
#include "duration.h"
#include "function.h"
#include "function_statistics.h"
#include "performance_counter.h"
#include "statistics_writer_xml.h"
#include "statistics.h"
#include "time_utils.h"

namespace amx_profiler {

void StatisticsWriterXml::Write(const Statistics *stats)
{
	*stream() << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
	          << "<stats script=\"" << script_name() << "\"";
	
	if (print_date()) {
		*stream() << " timestamp=\"" << TimeStamp::Now() << "\"";
	}

	if (print_run_time()) {
		*stream() << " run_time=\"" << Seconds(stats->GetTotalRunTime()).count() << "\'";
	}

	*stream() << ">\n";

	std::vector<FunctionStatistics*> all_fn_stats;
	stats->GetStatistics(all_fn_stats);

	Nanoseconds time_all;
	for (std::vector<FunctionStatistics*>::const_iterator iterator = all_fn_stats.begin();
			iterator != all_fn_stats.end(); ++iterator)
	{
		const FunctionStatistics *fn_stats = *iterator;
		time_all += fn_stats->total_time() - fn_stats->child_time(); 
	}

	Nanoseconds total_time_all;
	for (std::vector<FunctionStatistics*>::const_iterator iterator = all_fn_stats.begin();
			iterator != all_fn_stats.end(); ++iterator)
	{
		const FunctionStatistics *fn_stats = *iterator;
		total_time_all += fn_stats->total_time(); 
	}

	for (std::vector<FunctionStatistics*>::const_iterator iterator = all_fn_stats.begin();
			iterator != all_fn_stats.end(); ++iterator)
	{
		const FunctionStatistics *fn_stats = *iterator;

		double self_time_percent = fn_stats->GetSelfTime().count() * 100 / time_all.count();
		double total_time_percent = fn_stats->total_time().count() * 100 / total_time_all.count();

		*stream() << "\t<function"
			<< " type=\"" << fn_stats->function()->GetTypeString() << "\""
			<< " name=\"" << fn_stats->function()->name() << "\""
			<< " calls=\"" << fn_stats->num_calls() << "\""
			<< " self_time=\"" << fn_stats->GetSelfTime().count() << "\""
			<< " self_time_percent=\"" <<  std::fixed << std::setprecision(2) << self_time_percent << "\""
			<< " total_time=\"" << fn_stats->total_time().count() << "\""
			<< " total_time_percent=\"" <<  std::fixed << std::setprecision(2) << total_time_percent << "\""
		<< "/>\n";
	}

	*stream() << "</stats>";
}

} // namespace amx_profiler

