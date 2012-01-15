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

#include <sstream>
#include "native_function.h"

namespace amx_profiler {

NativeFunction::NativeFunction(AMX *amx, cell index)
	: index_(index), address_(0), name_()
{
	AMX_HEADER *amxhdr = reinterpret_cast<AMX_HEADER*>(amx->base);
	int num_natives = (amxhdr->libraries - amxhdr->natives) / amxhdr->defsize;
	if (index_ >= 0 && index_ < num_natives) {
		AMX_FUNCSTUBNT *natives = reinterpret_cast<AMX_FUNCSTUBNT*>(amxhdr->natives + amx->base);
		address_ = natives[index].address;
		name_.assign(reinterpret_cast<char*>(natives[index_].nameofs + amx->base));
	} else {
		std::stringstream ss;
		ss << index_;
		name_.append("unknown_native@").append(ss.str());
	}
}

std::string NativeFunction::type() const {
	return std::string("native");
}

std::string NativeFunction::name() const {
	return name_;
}

ucell NativeFunction::address() const {
	return address_;
}

Function *NativeFunction::Clone() const {
	return new NativeFunction(*this);
}

} // namespace amx_profiler
