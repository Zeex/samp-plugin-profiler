// SA:MP Profiler plugin
//
// Copyright (c) 2011 Zeex
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

#include <algorithm>
#include <cassert>
#include <map>
#include <numeric>
#include <sstream>
#include <string>

#include "abstract_printer.h"
#include "function.h"
#include "profile.h"
#include "profiler.h"

#include "amx/amx.h"
#include "amx/amxdbg.h"

namespace {

int AMXAPI Debug(AMX *amx) {
	return samp_profiler::Profiler::Get(amx)->Debug();
}

// Reads from a code section at a given location.
inline cell ReadAmxCode(AMX *amx, cell where) {
	AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx->base);
	return *reinterpret_cast<cell*>(amx->base + hdr->cod + where);
}

} // anonymous namespace

namespace samp_profiler {

// statics
std::map<AMX*, Profiler*> Profiler::instances_;

Profiler::Profiler() {}

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
	// Since PrintStats is done in AmxUnload and amx->base is already freed before
	// AmxUnload gets called, therefore both native and public tables are not accessible, 
	// from there, so they must be stored separately in some global place.
	AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx->base);
	AMX_FUNCSTUBNT *publics = reinterpret_cast<AMX_FUNCSTUBNT*>(amx->base + hdr->publics);
	AMX_FUNCSTUBNT *natives = reinterpret_cast<AMX_FUNCSTUBNT*>(amx->base + hdr->natives);
	int num_publics;
	amx_NumPublics(amx_, &num_publics);
	for (int i = 0; i < num_publics; i++) {
		public_names_.push_back(reinterpret_cast<char*>(amx->base + publics[i].nameofs));
	}
	int num_natives;
	amx_NumNatives(amx_, &num_natives);
	for (int i = 0; i < num_natives; i++) {
		native_names_.push_back(reinterpret_cast<char*>(amx->base + natives[i].nameofs));
	}
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

void Profiler::PrintStats(const std::string &script_name, std::ostream &stream, AbstractPrinter *printer) const {
	Profile profile;

	for (std::set<Function>::const_iterator iterator = functions_.begin(); 
			iterator != functions_.end(); ++iterator) 
	{
		std::string name;
		std::string type;

		switch (iterator->type()) {
		case Function::NATIVE: {
			name = native_names_[iterator->index()];
			type = "native";
			break;
		}
		case Function::PUBLIC: {
			if (iterator->index() >= 0) {
				name = public_names_[iterator->index()];
				type = "public";
			} else if (iterator->index() == AMX_EXEC_MAIN) {
				name = "main";
				type = "main";
			} else {
				std::stringstream ss;
				ss << "unknown_public@0x" << std::hex << iterator->address();
				type = "public";
			}
			break;
		}
		case Function::NORMAL: {
			type = "normal";
			bool name_found = false;
			if (debug_info_.IsLoaded()) {
				name = debug_info_.GetFunction(iterator->address());
				if (!name.empty()) {							
					name_found = true;
				}
			}
			if (!name_found) {
				std::stringstream ss;
				ss << "unknown@0x" << std::hex << iterator->address();
				ss >> name;
			}				
			break;
		}
		default:
			assert(0 && "Invalid function type");
		} 

		profile.push_back(ProfileEntry(name, type, iterator->time(), iterator->child_time(), 
				iterator->num_calls()));
	}

	printer->Print(script_name, stream, profile);
}

int Profiler::Debug() {
	cell prevFrame = amx_->stp;
	if (!call_stack_.IsEmpty()) {
		prevFrame = call_stack_.GetTop().frame();
	}

	if (amx_->frm < prevFrame) {
		cell address = amx_->cip - 2*sizeof(cell);   
		if (call_stack_.GetTop().frame() != amx_->frm) {
			EnterFunction(CallInfo(Function::Normal(address), amx_->frm));
		}
	} else if (amx_->frm > prevFrame) {
		Function top_fn = call_stack_.GetTop().function();
		if (top_fn.type() == Function::NORMAL) {
			LeaveFunction(top_fn);
		}
	}

	if (debug_ != 0) {
		return debug_(amx_);
	}   
	return AMX_ERR_NONE;      
}

int Profiler::Callback(cell index, cell *result, cell *params) {
	EnterFunction(CallInfo(Function::Native(index), amx_->frm));
	int error = amx_Callback(amx_, index, result, params);
	LeaveFunction(Function::Native(index));
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
		EnterFunction(CallInfo(Function::Public(index), 
			amx_->stk - 3*sizeof(cell)));
		// Why 3?
		// - one for parameter count (amx->paramcount)
		// - another is zero return address
		// - stk is the third, pushed by PROC opcode
		int error = amx_Exec(amx_, retval, index);
		LeaveFunction(Function::Public(index));
		return error;
	} else {
		return amx_Exec(amx_, retval, index);
	}
}

void Profiler::EnterFunction(const CallInfo &call) {
	call_stack_.Push(call);
	std::set<Function>::iterator iterator = functions_.find(call.function());
	if (iterator == functions_.end()) {
		Function f = call.function();
		f.IncreaseCalls();
		functions_.insert(f);
	} else {
		iterator->IncreaseCalls();
	}
}

void Profiler::LeaveFunction(const Function &function) {
	while (true) {
		CallInfo current = call_stack_.Pop();
		std::set<Function>::iterator current_it = functions_.find(current.function());
		if (current_it != functions_.end()) {
			current_it->AdjustTime(current.timer().total_time());
		}
		if (!call_stack_.IsEmpty()) {
			// Adjust caller's child_time
			CallInfo top = call_stack_.GetTop();
			std::set<Function>::iterator top_it = functions_.find(top.function());
			if (top_it != functions_.end()) {
				top_it->AdjustChildTime(current.timer().total_time());
			}
		}
		if (current.function() == function) {
			break;
		}
	}
}

} // namespace samp_profiler
