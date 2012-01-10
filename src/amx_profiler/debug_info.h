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
//
// Lightweight wrapper for some of the dbg_* routines from the amxdbg module.

#ifndef AMX_PROFILER_DEBUG_INFO_H
#define AMX_PROFILER_DEBUG_INFO_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <amx/amx.h>
#include <amxdbg.h>

namespace amx_profiler {

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

} // namespace amx_profiler

#endif // !AMX_PROFILER_DEBUGINFO_H

