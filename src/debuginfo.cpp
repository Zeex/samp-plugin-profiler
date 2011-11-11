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

#include <cassert>
#include <cstdio>

#include "debuginfo.h"

namespace samp_profiler {

DebugSymbol::DebugSymbol(AMX_DBG_SYMBOL *sym) 
	: sym_(sym) 
{
}

AMX_DBG_SYMBOL *DebugSymbol::GetPlainData() const { 
	return sym_; 
}

DebugSymbol::operator bool() const { 
	return sym_ != 0; 
}

bool DebugSymbol::IsGlobal() const { 
	return GetVClass() == Global; 
}

bool DebugSymbol::IsLocal() const { 
	return GetVClass() == Local; 
}

bool DebugSymbol::IsStaticLocal() const { 
	return GetVClass() == StaticLocal; 
}

bool DebugSymbol::IsVariable() const { 
	return GetKind() == Variable; 
}

bool DebugSymbol::IsReference() const { 
	return GetKind() == Reference; 
}

bool DebugSymbol::IsArray() const { 
	return GetKind() == Array; 
}

bool DebugSymbol::IsArrayRef() const { 
	return GetKind() == ArrayRef; 
}

bool DebugSymbol::IsFunction() const { 
	return GetKind() == Function; 
}

bool DebugSymbol::IsFunctionRef() const { 
	return GetKind() == FunctionRef; 
}

ucell DebugSymbol::GetAddress() const { 
	return sym_->address; 
}

int16_t DebugSymbol::GetTag() const { 
	return sym_->tag; 
}

ucell DebugSymbol::GetCodeStartAddress() const { 
	return sym_->codestart; 
}

ucell DebugSymbol::GetCodeEndAddress() const { 
	return sym_->codeend; 
}

DebugSymbol::Kind DebugSymbol::GetKind() const { 
	return static_cast<DebugSymbol::Kind>(sym_->ident); 
}

DebugSymbol::VClass DebugSymbol::GetVClass() const { 
	return static_cast<DebugSymbol::VClass>(sym_->vclass); 
}

int16_t DebugSymbol::GetDimensions() const { 
	return sym_->dim; 
}

std::string DebugSymbol::GetName() const { 
	return sym_->name; 
}

bool DebugSymbol::operator<(const DebugSymbol &rhs) const {
	return GetAddress() < rhs.GetAddress();
}

cell DebugSymbol::GetValue(AMX *amx) const {
	AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx->base);

	unsigned char *data = reinterpret_cast<unsigned char*>(amx->base + hdr->dat);
	unsigned char *code = reinterpret_cast<unsigned char*>(amx->base + hdr->cod);

	ucell address = GetAddress();
	// Pawn Implementer's Guide:
	// The address is relative to either the code segment (cod), the data segment
	// (dat) or to the frame of the current function �whose address is in the frm
	// pseudo-register.	
	if (address > hdr->cod) {
		return *reinterpret_cast<cell*>(code + address);
	} else if (address > hdr->dat && address < hdr->cod) {
		return *reinterpret_cast<cell*>(data + address);
	} else {
		return *reinterpret_cast<cell*>(data + amx->frm + address);
	}	
}

DebugInfo::DebugInfo() {}

DebugInfo::DebugInfo(const std::string &filename) {
	Load(filename);
}

bool DebugInfo::HasDebugInfo(AMX *amx) {
	uint16_t flags;
	amx_Flags(amx, &flags);
	return ((flags & AMX_FLAG_DEBUG) != 0);
}

void DebugInfo::FreeAmxDbg(AMX_DBG *amxdbg) {
	if (amxdbg != 0) {
		dbg_FreeInfo(amxdbg);
		delete amxdbg;
	}
}

bool DebugInfo::IsLoaded() const {
	return (amxdbgPtr_.get() != 0);
}

void DebugInfo::Load(const std::string &filename) {
	std::FILE* fp = std::fopen(filename.c_str(), "rb");
	if (fp != 0) {
		amxdbgPtr_.reset(new AMX_DBG, FreeAmxDbg);
		if (dbg_LoadInfo(amxdbgPtr_.get(), fp) != AMX_ERR_NONE) 
			amxdbgPtr_.reset();
		fclose(fp);
	}
}

void DebugInfo::Free() {
	amxdbgPtr_.reset();
}

long DebugInfo::GetLineNumber(cell address) const {
	long line = 0;
	if (dbg_LookupLine(amxdbgPtr_.get(), address, &line) == AMX_ERR_NONE) {
		return line + 1;
	}
	return -1;
}

std::string DebugInfo::GetFileName(cell address) const {
	std::string result;
	const char *file;
	if (dbg_LookupFile(amxdbgPtr_.get(), address, &file) == AMX_ERR_NONE)
		result.assign(file);
	return result;
}

std::string DebugInfo::GetFunctionName(cell address) const {
	std::string result;
	AMX_DBG *amxdbg = amxdbgPtr_.get();
	for (int i = 0; i < amxdbg->hdr->symbols; ++i) {
		AMX_DBG_SYMBOL *symbol = amxdbg->symboltbl[i];
		if (symbol->ident == iFUNCTN
				&& symbol->codestart <= address
				&& symbol->codeend > address) {
			if (symbol->name[0] != '@') {
				result.assign(symbol->name);
			}
		}
	}
	return result;
}

ucell DebugInfo::GetFunctionAddress(const std::string &functionName, const std::string &fileName) const {
	ucell functionAddress;
	dbg_GetFunctionAddress(amxdbgPtr_.get(), functionName.c_str(), fileName.c_str(), &functionAddress);
	return functionAddress;
}

ucell DebugInfo::GetFunctionStartAddress(ucell address) const {
	AMX_DBG *amxdbg = amxdbgPtr_.get();
	for (int i = 0; i < amxdbg->hdr->symbols; ++i) {
		AMX_DBG_SYMBOL *symbol = amxdbg->symboltbl[i];
		if (symbol->ident == iFUNCTN
				&& symbol->codestart <= address
				&& symbol->codeend > address) {
			if (symbol->name[0] != '@') {
				return symbol->codestart;
			}
		}
	}
	return 0;
}

ucell DebugInfo::GetLineAddress(long line, const std::string &fileName) const {
	ucell lineAddress;
	dbg_GetLineAddress(amxdbgPtr_.get(), line, fileName.c_str(), &lineAddress);
	return lineAddress;
}

std::string DebugInfo::GetAutomatonName(int automaton) const {
	std::string result;
	const char *automationName;
	if (dbg_GetAutomatonName(amxdbgPtr_.get(), automaton, &automationName) == AMX_ERR_NONE) {
		result.assign(automationName);
	}
	return result;
}

std::string DebugInfo::GetStateName(int state) const {
	std::string result;
	const char *stateName;
	if (dbg_GetStateName(amxdbgPtr_.get(), state, &stateName) == AMX_ERR_NONE) {
		result.assign(stateName);
	}
	return result;
}

std::string DebugInfo::GetTagName(int tag) const {
	std::string result;
	const char *tagName;
	if (dbg_GetTagName(amxdbgPtr_.get(), tag, &tagName) == AMX_ERR_NONE) {
		result.assign(tagName);
	}
	return result;
}

std::vector<DebugSymbol> DebugInfo::GetSymbols() const {
	std::vector<DebugSymbol> symbols;

	AMX_DBG *amxdbg = amxdbgPtr_.get();
	for (int i = 0; i < amxdbg->hdr->symbols; ++i) {
		symbols.push_back(DebugSymbol(amxdbg->symboltbl[i]));
	}

	return symbols;
}

} // namespace samp_profiler
