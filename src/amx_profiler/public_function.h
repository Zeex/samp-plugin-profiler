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

#ifndef AMX_PROFILER_PUBLIC_FUNCTION_H
#define AMX_PROFILER_PUBLIC_FUNCTION_H

#include "function.h"

namespace amx_profiler {

class PublicFunction : public Function {
public:
	PublicFunction(AMX *amx, cell index);

	virtual std::string name() const;
	virtual std::string type() const;	
	virtual ucell address() const;

	virtual Function *Clone() const;

	cell index() const { return index_; }

private:
	cell index_;
	ucell address_;
	std::string name_;
};

} // namespace amx_profiler

#endif // !AMX_PROFILER_PUBLIC_FUNCTION_H
