// AMX profiler for SA-MP server: http://sa-mp.com
//
// Copyright (C) 2011-2012 Sergey Zolotarev
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

#ifndef AMX_PROFILER_PRINTER_H
#define AMX_PROFILER_PRINTER_H

#include <ostream>
#include <vector>

namespace amx_profiler {

class FunctionInfo;

class Printer {
public:
	virtual void Print(const std::string &script_name, std::ostream &stream, 
			const std::vector<const FunctionInfo*> &stats) = 0;
};

} // namespace amx_profiler

#endif // !AMX_PROFILER_PRINTER_H
