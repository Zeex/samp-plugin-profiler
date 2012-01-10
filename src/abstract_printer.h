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

#ifndef SAMP_PROFILER_ABSTRACT_PRINTER_H
#define SAMP_PROFILER_ABSTRACT_PRINTER_H

#include <ostream>
#include <vector>

namespace samp_profiler {

class FunctionProfile;

class AbstractPrinter {
public:
	virtual void Print(const std::string &script_name, std::ostream &stream, 
			const std::vector<FunctionProfile> &stats) = 0;
};

} // namespace samp_profiler

#endif // !SAMP_PROFILER_PRINTERS_H
