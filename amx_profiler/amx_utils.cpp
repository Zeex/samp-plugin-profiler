// Copyright (c) 2013, Zeex
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "amx_types.h"

namespace amx_profiler {

Address GetNativeAddress(AMX *amx, NativeTableIndex index) {
	AMX_HEADER *amxhdr = reinterpret_cast<AMX_HEADER*>(amx->base);

	if (index >= 0) {
		AMX_FUNCSTUBNT *natives = reinterpret_cast<AMX_FUNCSTUBNT*>(amx->base + amxhdr->natives);
		return static_cast<Address>(natives[index].address);
	}

	return 0;
}

Address GetPublicAddress(AMX *amx, PublicTableIndex index) {
	AMX_HEADER *amxhdr = reinterpret_cast<AMX_HEADER*>(amx->base);

	if (index == AMX_EXEC_MAIN) {
		return amxhdr->cip;
	}

	if (index >= 0) {
		AMX_FUNCSTUBNT *publics = reinterpret_cast<AMX_FUNCSTUBNT*>(amx->base + amxhdr->publics);
		return static_cast<Address>(publics[index].address);
	}

	return 0;
}

const char *GetNativeName(AMX *amx, NativeTableIndex index) {
	AMX_HEADER *amxhdr = reinterpret_cast<AMX_HEADER*>(amx->base);

	NativeTableIndex num_natives = 0;
	amx_NumNatives(amx, &num_natives);

	if (index >= 0 && index < num_natives) {
		AMX_FUNCSTUBNT *natives = reinterpret_cast<AMX_FUNCSTUBNT*>(amx->base + amxhdr->natives);
		return reinterpret_cast<char*>(natives[index].nameofs + amx->base);
	}

	return "";
}

const char *GetPublicName(AMX *amx, PublicTableIndex index) {
	AMX_HEADER *amxhdr = reinterpret_cast<AMX_HEADER*>(amx->base);

	if (index == AMX_EXEC_MAIN) {
		return "main";
	}

	PublicTableIndex num_publics = 0;
	amx_NumPublics(amx, &num_publics);

	if (index >= 0 && index < num_publics) {
		AMX_FUNCSTUBNT *publics = reinterpret_cast<AMX_FUNCSTUBNT*>(amxhdr->publics + amx->base);
		return reinterpret_cast<char*>(publics[index].nameofs + amx->base);
	}

	return "";
}

} // naemspace amx_profiler
