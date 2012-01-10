// AMX profiler for SA-MP server: http://sa-mp.com
//
// Copyright (C) 2011 Sergey Zolotarev
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

#ifndef SAMP_PROFILER_XML_PRINTER_H
#define SAMP_PROFILER_XML_PRINTER_H

#include <string>

#include "abstract_printer.h"

namespace samp_profiler {

class XmlPrinter : public AbstractPrinter {
public:
	virtual void Print(const std::string &script_name, std::ostream &stream, 
			const std::vector<FunctionProfile> &stats);
};

} // namespace samp_profiler

#endif // !SAMP_PROFILER_XML_PRINTER_H
