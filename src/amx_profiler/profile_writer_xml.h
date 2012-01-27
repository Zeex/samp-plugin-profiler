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

#ifndef AMX_PROFILER_PROFILE_WRITER_XML_H
#define AMX_PROFILER_PROFILE_WRITER_XML_H

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>
#include "profile_writer.h"

namespace amx_profiler {

class ProfileWriterXml : public ProfileWriter {
public:
	ProfileWriterXml(std::ostream *stream, const std::string script_name);

	virtual void Write(const std::vector<std::shared_ptr<FunctionInfo>> &stats);

private:
	std::ostream *stream_;
	std::string   script_name_;
};

} // namespace amx_profiler

#endif // !AMX_PROFILER_PROFILE_WRITER_XML_H
