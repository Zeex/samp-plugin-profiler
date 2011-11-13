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
	virtual void Print(const Profile &profile) = 0;
};

class TextPrinter : public Printer {
public:
	static const int kFunctionTypeWidth  = 15;
	static const int kFunctionNameWidth  = 32;
	static const int kNumberOfCallsWidth = 20;
	static const int kAverageTimeWidth   = 20;
	static const int kOverallTimeWidth   = 20;
	static const int kPercentOfTimeWidth = 20;

	TextPrinter(const std::string &out_file, const std::string &header = "");
	virtual void Print(const Profile &profile);

private:
	std::string out_file_;
	std::string header_;
};

class HtmlPrinter : public Printer {
public:
	HtmlPrinter(const std::string &out_file, const std::string &title = "");
	virtual void Print(const Profile &profile);

private:
	std::string out_file_;
	std::string title_;
};

} // namespace samp_profiler

#endif // !SAMP_PROFILER_PRINTERS_H
