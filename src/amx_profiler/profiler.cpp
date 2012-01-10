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
#include <map>
#include <string>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include <amx/amx.h>
#include "function.h"
#include "function_profile.h"
#include "native_function.h"
#include "normal_function.h"
#include "printer.h"
#include "profiler.h"
#include "public_function.h"

namespace {

int AMXAPI Debug(AMX *amx) {
	return amx_profiler::Profiler::Get(amx)->Debug();
}

} // anonymous namespace

namespace amx_profiler {

// statics
std::map<AMX*, Profiler*> Profiler::instances_;

Profiler::Profiler() {
}

Profiler::~Profiler() {
	for (Functions::const_iterator iterator = functions_.begin(); 
			iterator != functions_.end(); ++iterator) {
		delete iterator->first;
	}
}

bool Profiler::IsScriptProfilable(AMX *amx) {
	uint16_t flags;
	amx_Flags(amx, &flags);

	if ((flags & AMX_FLAG_DEBUG) != 0) {
		return true;
	}

	if ((flags & AMX_FLAG_NOCHECKS) == 0) {
		return true;
	}

	return false;
}

// static
void Profiler::Attach(AMX *amx) {
	Profiler *prof = new Profiler(amx);
	instances_[amx] = prof;
	prof->Activate();
}

// static
void Profiler::Attach(AMX *amx, const DebugInfo &debug_info) {
	Attach(amx);
	Get(amx)->SetDebugInfo(debug_info);
}

// static
void Profiler::Detach(AMX *amx) {
	Profiler *prof = Profiler::Get(amx);
	if (prof != 0) {
		prof->Deactivate();
		delete prof;
	}
	instances_.erase(amx);
}

// static
Profiler *Profiler::Get(AMX *amx) {
	std::map<AMX*, Profiler*>::iterator it = instances_.find(amx);
	if (it != instances_.end()) {
		return it->second;
	}
	return 0;
}

Profiler::Profiler(AMX *amx) 
	: active_(false)
	, amx_(amx)
	, debug_(amx->debug)
{
}

void Profiler::SetDebugInfo(const DebugInfo &info) {
	debug_info_ = info;
}

void Profiler::Activate() {
	if (!active_) {
		active_ = true;
		amx_SetDebugHook(amx_, ::Debug);
	}
}

bool Profiler::IsActive() const {
	return active_;
}

void Profiler::Deactivate() {
	if (active_) {
		active_ = false;
		amx_SetDebugHook(amx_, debug_);
	}
}

void Profiler::ResetStats() {
	functions_.clear();
}

void Profiler::PrintStats(const std::string &script_name, std::ostream &stream, Printer *printer) const {
	std::vector<const FunctionProfile*> stats;
	for (Functions::const_iterator iterator = functions_.begin(); 
			iterator != functions_.end(); ++iterator) {
		stats.push_back(&iterator->second);
	}
	printer->Print(script_name, stream, stats);
}

int Profiler::Debug() {
	cell prevFrame = amx_->stp;
	if (!call_stack_.IsEmpty()) {
		prevFrame = call_stack_.GetTop().frame();
	}
	if (amx_->frm < prevFrame) {
		cell address = amx_->cip - 2*sizeof(cell);   
		if (call_stack_.GetTop().frame() != amx_->frm) {
			boost::scoped_ptr<NormalFunction> fn(new NormalFunction(address, &debug_info_));
			EnterFunction(fn.get(), amx_->frm);
		}
	} else if (amx_->frm > prevFrame) {
		Function *fn = call_stack_.GetTop().function();
		assert(fn->type() == "normal" && "Call stack messed up");
		LeaveFunction(fn);
	}
	if (debug_ != 0) {
		return debug_(amx_);
	}   
	return AMX_ERR_NONE;      
}

int Profiler::Callback(cell index, cell *result, cell *params) {
	boost::scoped_ptr<NativeFunction> fn(new NativeFunction(amx_, index));
	EnterFunction(fn.get(), amx_->frm);
	int error = amx_Callback(amx_, index, result, params);
	LeaveFunction(fn.get());
	return error;
}

int Profiler::Exec(cell *retval, int index) {	
	if (index >= 0 || index == AMX_EXEC_MAIN) {		
		AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx_->base);
		cell address = 0;
		if (index == AMX_EXEC_MAIN) {
			address = hdr->cip;
		} else {
			AMX_FUNCSTUBNT *publics = reinterpret_cast<AMX_FUNCSTUBNT*>(amx_->base + hdr->publics);
			address = publics[index].address;
		}        
		boost::scoped_ptr<PublicFunction> fn(new PublicFunction(amx_, index));
		EnterFunction(fn.get(), amx_->stk - 3*sizeof(cell));
		int error = amx_Exec(amx_, retval, index);
		LeaveFunction(fn.get());
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