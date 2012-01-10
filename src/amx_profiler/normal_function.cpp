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

#include <cassert>
#include <boost/lexical_cast.hpp>
#include "debug_info.h"
#include "normal_function.h"

namespace amx_profiler {

NormalFunction::NormalFunction(ucell address, DebugInfo *debug_info) 
	: address_(address), name_()
{
	if (debug_info != 0 && debug_info->IsLoaded()) {
		name_ = debug_info->GetFunction(address);
	}
	if (name_.empty()) {
		name_.append("unknown_function@").append(boost::lexical_cast<std::string>(address));
	}
}

std::string NormalFunction::type() const {
	return std::string("normal");
}

std::string NormalFunction::name() const {
	return name_;
}

ucell NormalFunction::address() const {
	return address_;
}

Function *NormalFunction::Clone() const {
	return new NormalFunction(*this);
}

} // namespace amx_profiler
