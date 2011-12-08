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

#ifndef SAMP_PROFILER_PRINTERS_H
#define SAMP_PROFILER_PRINTERS_H

#include <string>
#include <vector>

#include "profile.h"

namespace samp_profiler {

class Printer {
public:
	enum OutputSortMode {
		ORDER_NONE,
		SORT_BY_CALLS,
		SORT_BY_TIME,
		SORT_BY_TOTAL_TIME
	};

	Printer(const std::string &out_file, const std::string &script_name, bool sub_child_time, OutputSortMode sort_mode)
		: out_file_(out_file), script_name_(script_name), sub_child_time_(sub_child_time), sort_mode_(sort_mode)
	{}

	virtual void Print(Profile &profile) = 0;

protected:
	std::string out_file_;
	std::string script_name_;
	bool sub_child_time_;
	OutputSortMode sort_mode_;
};

class TextPrinter : public Printer {
public:
	static const int kFunctionTypeWidth  = 15;
	static const int kFunctionNameWidth  = 32;
	static const int kNumberOfCallsWidth = 20;
	static const int kAverageTimeWidth   = 20;
	static const int kOverallTimeWidth   = 20;
	static const int kPercentOfTimeWidth = 20;

	TextPrinter(const std::string &out_file, const std::string &script_name, bool sub_child_time, OutputSortMode sort_mode)
		: Printer(out_file, script_name, sub_child_time, sort_mode)
	{}

	virtual void Print(Profile &profile);
};

class HtmlPrinter : public Printer {
public:
	HtmlPrinter(const std::string &out_file, const std::string &script_name, bool sub_child_time, OutputSortMode sort_mode)
		: Printer(out_file, script_name, sub_child_time, sort_mode)
	{}

	virtual void Print(Profile &profile);
};

} // namespace samp_profiler

#endif // !SAMP_PROFILER_PRINTERS_H
