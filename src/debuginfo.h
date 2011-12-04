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

#ifndef SAMP_PROFILER_DEBUGINFO_H
#define SAMP_PROFILER_DEBUGINFO_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "amx/amx.h"
#include "amx/amxdbg.h"

namespace samp_profiler {

class DebugInfo {
public:
	DebugInfo();
	explicit DebugInfo(const std::string &filename);

	static bool HasDebugInfo(AMX *amx);
	static void FreeAmxDbg(AMX_DBG *amxdbg);

	void Load(const std::string &filename);
	bool IsLoaded() const;
	void Free();

	long GetLine(cell address) const;
	std::string GetFile(cell address) const;
	std::string GetFunction(cell address) const;

private:
	boost::shared_ptr<AMX_DBG> amxdbgPtr_;
};

} // namespace samp_profiler

#endif // !SAMP_PROFILER_DEBUGINFO_H
