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

#ifndef SAMP_PROFILER_FUNCTION_FUNCTION_H
#define SAMP_PROFILER_FUNCTION_FUNCTION_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "cloneable.h"
#include "amx/amx.h"

namespace samp_profiler {

class Function : public Cloneable<Function> {
public:
	friend class Profiler;

	Function(AMX *amx);
	virtual ~Function();

	AMX *amx() const;

	virtual std::string name() const = 0;
	virtual std::string type() const = 0;
	
	virtual int Compare(const Function *other) const = 0;	

private:
	AMX *amx_;
};

} // namespace samp_profiler

#endif // !SAMP_PROFILER_FUNCTION_FUNCTION_H
