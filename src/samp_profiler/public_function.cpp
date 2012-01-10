// AMX profiler for SA-MP server: http://sa-mp.com
//
// Copyright (C) 2011 Sergey Zolotarev
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
#include <boost/lexical_cast.hpp>

#include "public_function.h"

namespace samp_profiler {

PublicFunction::PublicFunction(AMX *amx, cell index) 
	: Function(amx), index_(index), name_()
{
	AMX_HEADER *amxhdr = reinterpret_cast<AMX_HEADER*>(amx->base);
	int num_publics = (amxhdr->natives - amxhdr->publics) / amxhdr->defsize;
	if (index_ >= 0 && index_ < num_publics) {
		AMX_FUNCSTUBNT *publics = reinterpret_cast<AMX_FUNCSTUBNT*>(amxhdr->publics + amx->base);
		name_.assign(reinterpret_cast<char*>(publics[index_].nameofs + amx->base));
	} else if (index_ == AMX_EXEC_MAIN) {
		name_.assign("main");
	} else {
		name_.append(std::string("unknown_public@")).append(boost::lexical_cast<std::string>(index_));
	}
}

std::string PublicFunction::type() const {
	return std::string("public");
}

std::string PublicFunction::name() const {
	return name_;
}

int PublicFunction::Compare(const Function *that) const {
	assert(that->type() == this->type() && "Function types do not match");
	return this->index() - dynamic_cast<const PublicFunction*>(that)->index();
}

Function *PublicFunction::Clone() const {
	return new PublicFunction(*this);
}

} // namespace samp_profiler
