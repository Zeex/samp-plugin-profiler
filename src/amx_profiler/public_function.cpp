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
	} else {
		std::stringstream ss;
		ss << index_;
		name_.append(std::string("unknown_public@")).append(ss.str());
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
