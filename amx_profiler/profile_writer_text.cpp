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
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "function.h"
#include "function_info.h"
#include "performance_counter.h"
#include "profile_writer_text.h"

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

ProfileWriterText::ProfileWriterText(std::ostream *stream, const std::string script_name)
	: stream_(stream)
	, script_name_(script_name)
{
}

void ProfileWriterText::Write(const std::vector<std::shared_ptr<FunctionInfo>> &stats)
{
	std::time_t now = std::time(0);

	*stream_ << "Profile of '" << script_name_ << "'" << 
		" generated on " << ctime(&now) << std::endl;

	auto DoHLine = [&]() {
		char fillch = stream_->fill();
		*stream_ << std::setw(kWidthAll + kNumColumns * 2 + 1) 
			<< std::setfill('-') << "" << std::setfill(fillch) << '\n';
	};

	DoHLine();
	*stream_ << std::left
		<< "| " << std::setw(kTypeWidth) << "Type"
		<< "| " << std::setw(kNameWidth) << "Name"
		<< "| " << std::setw(kCallsWidth) << "Calls"
		<< "| " << std::setw(kSelfTimePercentWidth) << "Self Time (%)"
		<< "| " << std::setw(kSelfTimeSecWidth) << "Self Time (sec.)"
		<< "| " << std::setw(kTotalTimePercentWidth) << "Total Time (%)"
		<< "| " << std::setw(kTotalTimeSecWidth) << "Total Time (sec.)"
		<< "|\n";
	DoHLine();

	TimeInterval time_all = 0;
	for (auto &info : stats) {
		time_all += info->total_time() - info->child_time(); 
	}

	TimeInterval total_time_all = 0;
	for (auto &info : stats) {
		total_time_all += info->total_time(); 
	}

	for (auto &info : stats) {
		double self_time_sec = static_cast<double>(info->GetSelfTime()) / 1E+9;
		double self_time_percent = static_cast<double>(info->GetSelfTime() * 100) / time_all;
		double total_time_sec = static_cast<double>(info->total_time()) / 1E+9;
		double total_time_percent =  static_cast<double>(info->total_time() * 100) / total_time_all;
		*stream_
			<< "| " << std::setw(kTypeWidth) << info->function()->type()
			<< "| " << std::setw(kNameWidth) << info->function()->name()
			<< "| " << std::setw(kCallsWidth) << info->num_calls()
			<< "| " << std::setw(kSelfTimePercentWidth) << std::fixed << std::setprecision(2) << self_time_percent
			<< "| " << std::setw(kSelfTimeSecWidth) << std::fixed << std::setprecision(3) << self_time_sec
			<< "| " << std::setw(kTotalTimePercentWidth) << std::fixed << std::setprecision(2) << total_time_percent
			<< "| " << std::setw(kTotalTimeSecWidth) << std::fixed << std::setprecision(3) << total_time_sec
			<< "|\n";
		DoHLine();
	}
}

} // namespace amx_profiler
