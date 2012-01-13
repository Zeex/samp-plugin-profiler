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
#include <map>
#include <string>
#include <vector>
#include <amx/amx.h>
#include "function.h"
#include "function_profile.h"
#include "native_function.h"
#include "normal_function.h"
#include "profiler.h"
#include "public_function.h"

namespace amx_profiler {

// statics
std::map<AMX*, Profiler*> Profiler::instances_;

Profiler::Profiler(AMX *amx, DebugInfo debug_info)
	: amx_(amx)
	, debug_info_(debug_info)
{
}

Profiler::~Profiler() {
	for (Functions::const_iterator iterator = functions_.begin(); 
			iterator != functions_.end(); ++iterator) {
		delete iterator->first;
	}
}

// static
void Profiler::Attach(AMX *amx, DebugInfo debug_info) {
	instances_[amx] = new Profiler(amx, debug_info);
}

// static
void Profiler::Detach(AMX *amx) {
	Profiler *prof = Profiler::GetInstance(amx);
	if (prof != 0) {
		delete prof;
	}
	instances_.erase(amx);
}

// static
Profiler *Profiler::GetInstance(AMX *amx) {
	std::map<AMX*, Profiler*>::iterator iterator = instances_.find(amx);
	if (iterator != instances_.end()) {
		return iterator->second;
	}
	return 0;
}

std::vector<const FunctionProfile*> Profiler::GetProfile() const {
	std::vector<const FunctionProfile*> profile;
	for (Functions::const_iterator iterator = functions_.begin(); 
			iterator != functions_.end(); ++iterator) {
		profile.push_back(&iterator->second);
	}
	return profile;
}

int Profiler::AmxDebugHook() {
	cell prevFrame = amx_->stp;
	if (!call_stack_.IsEmpty()) {
		prevFrame = call_stack_.GetTop().frame();
	}
	if (amx_->frm < prevFrame) {
		cell address = amx_->cip - 2*sizeof(cell);   
		if (call_stack_.GetTop().frame() != amx_->frm) {
			NormalFunction fn(address, &debug_info_);
			EnterFunction(&fn, amx_->frm);
		}
	} else if (amx_->frm > prevFrame) {
		Function *fn = call_stack_.GetTop().function();
		assert(fn->type() == "normal" && "Call stack messed up");
		LeaveFunction(fn);
	}
	return AMX_ERR_NONE;      
}

int Profiler::AmxCallbackHook(cell index, cell *result, cell *params) {
	NativeFunction fn(amx_, index);
	EnterFunction(&fn, amx_->frm);
	int error = amx_Callback(amx_, index, result, params);
	LeaveFunction(&fn);
	return error;
}

int Profiler::AmxExecHook(cell *retval, int index) {	
	if (index >= 0 || index == AMX_EXEC_MAIN) {		
		AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx_->base);
		cell address = 0;
		if (index == AMX_EXEC_MAIN) {
			address = hdr->cip;
		} else {
			AMX_FUNCSTUBNT *publics = reinterpret_cast<AMX_FUNCSTUBNT*>(amx_->base + hdr->publics);
			address = publics[index].address;
		}        
		PublicFunction fn(amx_, index);
		EnterFunction(&fn, amx_->stk - 3*sizeof(cell));
		int error = amx_Exec(amx_, retval, index);
		LeaveFunction(&fn);
		return error;
	} else {
		return amx_Exec(amx_, retval, index);
	}
}

void Profiler::EnterFunction(const Function *fn, ucell frame) {
	Functions::iterator iterator = functions_.find(const_cast<Function*>(fn));
	if (iterator == functions_.end()) {	
		Function *new_fn = fn->Clone();
		functions_.insert(std::make_pair(new_fn, FunctionProfile(new_fn)));
		call_stack_.Push(new_fn, frame);
	} else {
		iterator->second.num_calls()++;
		call_stack_.Push(iterator->second.function(), frame);
	}
}

void Profiler::LeaveFunction(const Function *fn) {
	assert(!call_stack_.IsEmpty());
	while (true) {
		FunctionCall current = call_stack_.Pop();
		Functions::iterator current_it = functions_.find(current.function());
		if (current.IsRecursive()) {			
			current_it->second.child_time() -= current.timer().child_time();
		} else {
			current_it->second.total_time() += current.timer().total_time();
		}	
		if (!call_stack_.IsEmpty()) {
			FunctionCall &top = call_stack_.GetTop();
			functions_.find(top.function())->second.child_time() += current.timer().total_time();
		}
		if (fn == 0 || (current.function()->address() == fn->address())) {
			break;
		}
	}
}

} // namespace amx_profiler
