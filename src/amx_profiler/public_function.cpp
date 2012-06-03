// Copyright (c) 2011-2012, Zeex
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met: 
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer. 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <cassert>
#include "public_function.h"

namespace amx_profiler {

PublicFunction::PublicFunction(AMX *amx, cell index)
	: index_(index), address_(0), name_()
{
	AMX_HEADER *amxhdr = reinterpret_cast<AMX_HEADER*>(amx->base);
	int num_publics = (amxhdr->natives - amxhdr->publics) / amxhdr->defsize;
	if (index_ >= 0 && index_ < num_publics) {
		AMX_FUNCSTUBNT *publics = reinterpret_cast<AMX_FUNCSTUBNT*>(amxhdr->publics + amx->base);
		address_ = publics[index].address;
		name_.assign(reinterpret_cast<char*>(publics[index_].nameofs + amx->base));
	} else if (index_ == AMX_EXEC_MAIN) {
		name_.assign("main");
		address_ = amxhdr->cip;
	} else {
		assert(0 && "Invalid public index");
	}
}

std::string PublicFunction::type() const {
	return std::string("public");
}

std::string PublicFunction::name() const {
	return name_;
}

ucell PublicFunction::address() const {
	return address_;
}

} // namespace amx_profiler
