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
#include <numeric>
#include <sstream>
#include <string>

#include "abstract_printer.h"
#include "profile.h"
#include "profiler.h"

#include "amx/amx.h"
#include "amx/amxdbg.h"

namespace {

int AMXAPI Debug(AMX *amx) {
	return samp_profiler::Profiler::Get(amx)->Debug();
}

} // anonymous namespace

namespace samp_profiler {

// statics
std::map<AMX*, Profiler*> Profiler::instances_;

Profiler::Profiler() {}

// Extracts the names of native functions from the native table.
static void GetNatives(AMX *amx, std::vector<Profiler::Function> &natives) {
	AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx->base);
	AMX_FUNCSTUBNT *nativeTable = reinterpret_cast<AMX_FUNCSTUBNT*>(amx->base + hdr->natives);

	int numberOfNatives;
	amx_NumNatives(amx, &numberOfNatives);

	for (int i = 0; i < numberOfNatives; i++) {
		natives.push_back(Profiler::Function(nativeTable[i].address,
			reinterpret_cast<char*>(amx->base + nativeTable[i].nameofs)));
	}
}

// Extracts the names of public functions from the native table.
static void GetPublics(AMX *amx, std::vector<Profiler::Function> &publics) {
	AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx->base);
	AMX_FUNCSTUBNT *publicTable = reinterpret_cast<AMX_FUNCSTUBNT*>(amx->base + hdr->publics);

	int numberOfPublics;
	amx_NumPublics(amx, &numberOfPublics);

	for (int i = 0; i < numberOfPublics; i++) {
		publics.push_back(Profiler::Function(publicTable[i].address, 
			reinterpret_cast<char*>(amx->base + publicTable[i].nameofs)));
	}
}

// Reads from a code section at a given location.
static inline cell ReadAmxCode(AMX *amx, cell where) {
	AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx->base);
	return *reinterpret_cast<cell*>(amx->base + hdr->cod + where);
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
void Profiler::Attach(AMX *amx, const DebugInfo &debugInfo) {
	Attach(amx);
	Get(amx)->SetDebugInfo(debugInfo);
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
	GetNatives(amx, natives_);
	GetPublics(amx, publics_);
}

void Profiler::SetDebugInfo(const DebugInfo &info) {
	debugInfo_ = info;
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
	counters_.clear();
}

void Profiler::PrintStats(AbstractPrinter &printer) {
	std::vector<std::pair<cell, PerformanceCounter> > stats(
		counters_.begin(), counters_.end());

	Profile profile;

	for (std::vector<std::pair<cell, PerformanceCounter> >::iterator stat_it = stats.begin(); 
			stat_it != stats.end(); ++stat_it) 
	{
		cell address = stat_it->first;
		PerformanceCounter &counter = stat_it->second;

		if (address < 0) {
			profile.push_back(ProfileEntry(natives_[-address-1].name(), "native", counter));
		} else {
			bool found = false;
			// Search in public table
			for (std::vector<Profiler::Function>::iterator pub_it = publics_.begin(); 
					pub_it != publics_.end(); ++pub_it) 
			{
				if (pub_it->address() == address)  {
					profile.push_back(ProfileEntry(pub_it->name(), "public", counter));
					found = true;
					break;
				}
			}
			// Search in symbol table
			if (!found) {
				if (debugInfo_.IsLoaded()) {
					std::string name = debugInfo_.GetFunction(address);
					if (!name.empty()) {	
						profile.push_back(ProfileEntry(debugInfo_.GetFunction(address), "ordinary", counter));
						found = true;
					}
				}
			}
			// Not found
			if (!found) {
				std::stringstream ss;
				ss << "0x" << std::hex << address;
				profile.push_back(ProfileEntry(ss.str(), "unknown", counter));
			}
		}
	}

	printer.Print(profile);
}

int Profiler::Debug() {
	// Get previous stack frame.
	cell prevFrame = amx_->stp;

	if (!call_stack_.empty()) {
		prevFrame = call_stack_.top().frame();
	}

	// Check whether current frame is different.
	if (amx_->frm < prevFrame) {
		// Probably entered a function body (first BREAK after PROC).
		cell address = amx_->cip - 2*sizeof(cell);            
		// Check if we have a PROC opcode behind.
		if (ReadAmxCode(amx_, address) == 46) {
			EnterFunction(CallInfo(amx_->frm, address, CallInfo::ORDINARY));
		}
	} else if (amx_->frm > prevFrame) {
		if (call_stack_.top().functionType() != CallInfo::PUBLIC) { // entry points are handled by Exec
			// Left the function
			cell address = call_stack_.top().address();
			LeaveFunction(address);
		}
	}

	if (debug_ != 0) {
		// Others could set their own debug hooks
		return debug_(amx_);
	}   

	return AMX_ERR_NONE;      
}

int Profiler::Callback(cell index, cell *result, cell *params) {
	cell address = -index - 1;

	EnterFunction(CallInfo(amx_->frm, address, CallInfo::NATIVE));
	int error = amx_Callback(amx_, index, result, params);
	LeaveFunction(address);

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

		EnterFunction(CallInfo(amx_->stk - 3*sizeof(cell), address, CallInfo::PUBLIC));
		int error = amx_Exec(amx_, retval, index);
		LeaveFunction(address);

		return error;
	} else {
		return amx_Exec(amx_, retval, index);
	}
}

void Profiler::EnterFunction(const CallInfo &info) {
	if (active_) {
		PerformanceCounter &counter = counters_[info.address()];
		if (call_stack_.empty()) {
			counter.Start();
		} else {
			counter.Start(&counters_[call_stack_.top().address()]);
		}
	}
	call_stack_.push(info);
}

void Profiler::LeaveFunction(cell address) {
	while (true) {
		cell topAddress = call_stack_.top().address();
		if (active_) {
			counters_[topAddress].Stop();
		}
		call_stack_.pop();		
		if (topAddress == address) {
			break;
		}
	}
}

bool Profiler::GetLastCall(CallInfo &call) const {
	if (!call_stack_.empty()) {
		call = call_stack_.top();
		return true;
	}
	return false;
}

} // namespace samp_profiler
