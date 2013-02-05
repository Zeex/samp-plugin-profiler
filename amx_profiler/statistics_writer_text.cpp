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
#include <string>
#include "duration.h"
#include "function.h"
#include "function_statistics.h"
#include "performance_counter.h"
#include "statistics_writer_text.h"
#include "statistics.h"
#include "time_utils.h"

static const int kTypeWidth = 7;
static const int kNameWidth = 32;
static const int kCallsWidth = 10;
static const int kSelfTimeSecWidth = 16;
static const int kSelfTimePercentWidth = 13;
static const int kTotalTimeSecWidth = 17;
static const int kTotalTimePercentWidth = 14;

static const int kWidthAll = kTypeWidth + kNameWidth + kCallsWidth 
	+ kSelfTimeSecWidth + kSelfTimePercentWidth + kTotalTimeSecWidth + kTotalTimePercentWidth;

static const int kNumColumns = 7;

namespace amx_profiler {

void StatisticsWriterText::Write(const Statistics *stats)
{
	*stream() << "Profile of '" << script_name() << "'";

	if (print_date()) {
		*stream() << " generated on " << CTime();
	}

	if (print_run_time()) {
		*stream() << " (duration: " << TimeSpan(stats->GetTotalRunTime()) << ")\n";
	}

	auto DoHLine = [&]() {
		char fillch = stream()->fill();
		*stream() << std::setw(kWidthAll + kNumColumns * 2 + 1) 
		          << std::setfill('-') << "" << std::setfill(fillch) << '\n';
	};

	DoHLine();
	*stream() << std::left
		<< "| " << std::setw(kTypeWidth) << "Type"
		<< "| " << std::setw(kNameWidth) << "Name"
		<< "| " << std::setw(kCallsWidth) << "Calls"
		<< "| " << std::setw(kSelfTimePercentWidth) << "Self Time (%)"
		<< "| " << std::setw(kSelfTimeSecWidth) << "Self Time (sec.)"
		<< "| " << std::setw(kTotalTimePercentWidth) << "Total Time (%)"
		<< "| " << std::setw(kTotalTimeSecWidth) << "Total Time (sec.)"
		<< "|\n";
	DoHLine();

	Duration time_all;
	stats->EnumerateFunctions([&](const FunctionStatistics *fn_stats) {
		time_all += fn_stats->total_time() - fn_stats->child_time(); 
	});

	Duration total_time_all;
	stats->EnumerateFunctions([&](const FunctionStatistics *fn_stats) {
		total_time_all += fn_stats->total_time(); 
	});

	stats->EnumerateFunctions([&](const FunctionStatistics *fn_stats) {
		double self_time_sec = Seconds(fn_stats->GetSelfTime()).count();
		double self_time_percent = fn_stats->GetSelfTime().count() * 100 / time_all.count();
		double total_time_sec = Seconds(fn_stats->total_time()).count();
		double total_time_percent = fn_stats->total_time().count() * 100 / total_time_all.count();
		*stream()
			<< "| " << std::setw(kTypeWidth) << fn_stats->function()->type()
			<< "| " << std::setw(kNameWidth) << fn_stats->function()->name()
			<< "| " << std::setw(kCallsWidth) << fn_stats->num_calls()
			<< "| " << std::setw(kSelfTimePercentWidth) << std::fixed << std::setprecision(2) << self_time_percent
			<< "| " << std::setw(kSelfTimeSecWidth) << std::fixed << std::setprecision(3) << self_time_sec
			<< "| " << std::setw(kTotalTimePercentWidth) << std::fixed << std::setprecision(2) << total_time_percent
			<< "| " << std::setw(kTotalTimeSecWidth) << std::fixed << std::setprecision(3) << total_time_sec
			<< "|\n";
		DoHLine();
	});
}

} // namespace amx_profiler
