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

#ifndef SAMP_PROFILER_TEXT_PRINTER_H
#define SAMP_PROFILER_TEXT_PRINTER_H

#include <string>

#include "printer.h"
#include "profile.h"

namespace samp_profiler {

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

} // namespace samp_profiler

#endif // !SAMP_PROFILER_TEXT_PRINTER_H
