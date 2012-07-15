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

#ifndef AMX_PROFILER_CALL_STACK_H
#define AMX_PROFILER_CALL_STACK_H

#include <list>
#include <amx/amx.h>
#include "function_call.h"

namespace amx_profiler {

class Function;

class CallStack {
public:
	void Push(Function *function, ucell frame);
	void Push(const FunctionCall &call);

	FunctionCall Pop();

	bool IsEmpty() const
		{ return calls_.empty(); }

	FunctionCall *top()
		{ return &calls_.back(); }
	const FunctionCall *top() const
		{ return &calls_.back(); }

	FunctionCall *bottom()
		{ return &calls_.front(); }
	const FunctionCall *bottom() const
		{ return &calls_.front(); }

	// Iterator support
	typedef std::list<FunctionCall>::iterator iterator;
	typedef std::list<FunctionCall>::const_iterator const_iterator;
	typedef std::list<FunctionCall>::reverse_iterator reverse_iterator;
	typedef std::list<FunctionCall>::const_reverse_iterator const_reverse_iterator;

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
	std::list<FunctionCall> calls_;
};

} // namespace amx_profiler

#endif // !AMX_PROFILER_CALL_STACK_H
