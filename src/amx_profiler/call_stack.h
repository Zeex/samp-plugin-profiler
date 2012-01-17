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

#ifndef AMX_PROFILER_CALL_STACK_H
#define AMX_PROFILER_CALL_STACK_H

#include <list>
#include <memory>
#include <amx/amx.h>

namespace amx_profiler {

class Function;
class FunctionCall;

class CallStack {
public:
	void Push(const std::shared_ptr<Function> &function, ucell frame);
	void Push(const std::shared_ptr<FunctionCall> &info);

	std::shared_ptr<FunctionCall> Pop();

	bool IsEmpty() const
		{ return calls_.empty(); }

	std::shared_ptr<FunctionCall> &GetTop()
		{ return calls_.back(); }
	const std::shared_ptr<FunctionCall> &GetTop() const
		{ return calls_.back(); }

	std::shared_ptr<FunctionCall> &GetBottom()
		{ return calls_.front(); }
	const std::shared_ptr<FunctionCall> &GetBottom() const
		{ return calls_.front(); }

	typedef std::list<std::shared_ptr<FunctionCall>>::iterator iterator;
	typedef std::list<std::shared_ptr<FunctionCall>>::const_iterator const_iterator;
	typedef std::list<std::shared_ptr<FunctionCall>>::reverse_iterator reverse_iterator;
	typedef std::list<std::shared_ptr<FunctionCall>>::const_reverse_iterator const_reverse_iterator;

	iterator begin()
		{ return calls_.begin(); }
	const_iterator begin() const
		{ return calls_.begin(); }
	iterator end()
		{ return calls_.end(); }
	const_iterator end() const
		{ return calls_.end(); }
	reverse_iterator rbegin()
		{ return calls_.rbegin(); }
	const_reverse_iterator rbegin() const 
		{ return calls_.rbegin(); }
	reverse_iterator rend()
		{ return calls_.rend(); }
	const_reverse_iterator rend() const
		{ return calls_.rend(); }
	
private:
	std::list<std::shared_ptr<FunctionCall>> calls_;
};

} // namespace amx_profiler

#endif // !AMX_PROFILER_CALL_STACK_H
