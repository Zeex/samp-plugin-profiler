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
#include <vector>

#include <boost/shared_ptr.hpp>

#include "amx/amx.h"
#include "amx/amxdbg.h"

namespace samp_profiler {

class DebugSymbol {
public:
	enum VClass {
		Global      = 0,
		Local       = 1,
		StaticLocal = 2
	};

	enum Kind {
		Variable    = 1,
		Reference   = 2,
		Array       = 3,
		ArrayRef    = 4,
		Function    = 9,
		FunctionRef = 10
	};

	explicit DebugSymbol(AMX_DBG_SYMBOL *sym);

	AMX_DBG_SYMBOL *GetPlainData() const;
	operator bool() const;

	bool IsGlobal() const;
	bool IsLocal() const;
	bool IsStaticLocal() const;

	bool IsVariable() const;
	bool IsReference() const;
	bool IsArray() const;
	bool IsArrayRef() const;
	bool IsFunction() const;
	bool IsFunctionRef() const;

	ucell GetAddress() const;
	int16_t GetTag() const;
	ucell GetCodeStartAddress() const;
	ucell GetCodeEndAddress() const;
	Kind GetKind() const;
	VClass GetVClass() const;
	int16_t GetDimensions() const;
	std::string GetName() const;

	bool operator<(const DebugSymbol &rhs) const;

	cell GetValue(AMX *amx) const;

private:
	AMX_DBG_SYMBOL *sym_;
};

class DebugInfo {
public:
	DebugInfo();
	explicit DebugInfo(const std::string &filename);

	static bool HasDebugInfo(AMX *amx);
	static void FreeAmxDbg(AMX_DBG *amxdbg);

	void Load(const std::string &filename);
	bool IsLoaded() const;
	void Free();

	long GetLineNumber(cell address) const;
	std::string GetFileName(cell address) const;
	std::string GetFunctionName(cell address) const;

	ucell GetFunctionAddress(const std::string &functionName, const std::string &fileName) const;
	ucell GetFunctionStartAddress(ucell address) const;
	ucell GetLineAddress(long line, const std::string &fileName) const;
	std::string GetAutomatonName(int automaton) const;
	std::string GetStateName(int state) const;
	std::string GetTagName(int tag) const;

	// Get all symbols
	std::vector<DebugSymbol> GetSymbols() const;

private:
	boost::shared_ptr<AMX_DBG> amxdbgPtr_;
};

} // namespace samp_profiler

#endif // !SAMP_PROFILER_DEBUGINFO_H
