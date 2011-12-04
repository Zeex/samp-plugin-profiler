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

#include <string>
#include <vector>

#include "debuginfo.h"
#include "logprintf.h"

namespace samp_profiler {

static std::string GetKindString(DebugSymbol::Kind kind) {
	std::string result;

	switch (kind) {
	case DebugSymbol::Array:
		result = "Array";
		break;
	case DebugSymbol::ArrayRef:
		result = "ArrayRef";
		break;
	case DebugSymbol::Function:
		result = "Function";
		break;
	case DebugSymbol::FunctionRef:
		result = "FunctionRef";
		break;
	case DebugSymbol::Reference:
		result = "Reference";
		break;
	case DebugSymbol::Variable:
		result = "Variable";
		break;
	default:
		result = "Unknown";
		break;
	}

	return result;
}

static std::string GetVClassString(DebugSymbol::VClass vclass) {
	std::string result;

	switch (vclass) {
	case DebugSymbol::Local:
		result = "Local";
		break;
	case DebugSymbol::Global:
		result = "Global";
		break;
	case DebugSymbol::StaticLocal:
		result = "StaticLocal";
		break;
	default:
		result = "Unknown";
		break;
	}

	return result;
}

void DumpSymbolTable(const DebugInfo &debugInfo) {
	std::vector<DebugSymbol> symbols = debugInfo.GetSymbols();

	// Get max. tag name length
	size_t max_tag_length = 0;
	for (std::vector<DebugSymbol>::const_iterator it = symbols.begin(); it != symbols.end(); ++it) {
		size_t length = debugInfo.GetTagName(it->GetTag()).length();
		if (length > max_tag_length) {
			max_tag_length = length;
		}
	}
	max_tag_length = std::max(3u, max_tag_length);

	// Get max. symbol name length
	size_t max_name_length = 0;
	for (std::vector<DebugSymbol>::iterator it = symbols.begin(); it != symbols.end(); ++it) {
		size_t length = it->GetName().length();
		if (length > max_name_length) {
			max_name_length = length;
		}
	}
	max_name_length = std::max(4u, max_name_length);

	logprintf("%11s %11s %*s %*s %10s %10s %10s",
		"VClass",
		"Kind",
		max_tag_length,
		"Tag",
		max_name_length,
		"Name",
		"Addr",
		"CSAddr",
		"CEAddr"
	);

	for (std::vector<DebugSymbol>::const_iterator it = symbols.begin(); it != symbols.end(); ++it) {
		logprintf("%11s %11s %*s %*s 0x%08x 0x%08x 0x%08x", 
			GetVClassString(it->GetVClass()).c_str(),
			GetKindString(it->GetKind()).c_str(),
			max_tag_length,
			debugInfo.GetTagName(it->GetTag()).c_str(),
			max_name_length,
			it->GetName().c_str(),
			it->GetAddress(),
			it->GetCodeStartAddress(),
			it->GetCodeEndAddress()
		);
	}
}

} // namespace samp_profiler