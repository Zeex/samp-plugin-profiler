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

#ifndef DEBUGINFO_H
#define DEBUGINFO_H

#include <string>
#include <memory>

#include "amx/amx.h"
#include "amx/amxdbg.h"

class DebugInfo {
public:
	static bool HasDebugInfo(AMX *amx);
	static void FreeAmxDbg(AMX_DBG *amxdbg);

	void Load(const std::string &filename);
	bool IsLoaded() const;
	void Free();

	long GetLine(cell address);
	std::string GetFile(cell address);
	std::string GetFunction(cell address);

private:
	bool isLoaded_;
	std::tr1::shared_ptr<AMX_DBG> amxdbgPtr_;
};

#endif // !DEBUGINFO_H