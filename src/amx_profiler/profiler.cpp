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

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <functional>
#include <map>
#include <ostream>
#include <string>
#include <vector>
#include <amx/amx.h>
#include "function.h"
#include "function_call.h"
#include "function_info.h"
#include "native_function.h"
#include "normal_function.h"
#include "profile_writer.h"
#include "profiler.h"
#include "public_function.h"

namespace amx_profiler {

Profiler::Profiler(AMX *amx, DebugInfo debug_info, bool enable_call_graph)
	: amx_(amx)
	, debug_info_(debug_info)
	, call_graph_enabled_(enable_call_graph)
{
	// nothing
}

std::vector<std::shared_ptr<FunctionInfo>> Profiler::GetProfile() const {
	std::vector<std::shared_ptr<FunctionInfo>> profile;
	for (auto iterator = functions_.begin(); iterator != functions_.end(); ++iterator) {
		profile.push_back(iterator->second);
	}
	return profile;
}

void Profiler::WriteProfile(ProfileWriter *writer) const {
	writer->Write(GetProfile());
}

// Profile normal function call.
int Profiler::amx_Debug(int (AMXAPI *debug)(AMX *amx)) {
	cell prevFrame = amx_->stp;
	if (!call_stack_.IsEmpty()) {
		prevFrame = call_stack_.GetTop()->frame();
	}
	if (amx_->frm < prevFrame) {
		if (call_stack_.GetTop()->frame() != amx_->frm) {
			auto address = static_cast<ucell>(amx_->cip) - 2*sizeof(cell);
			if (functions_.find(address) == functions_.end()) {
				std::shared_ptr<Function> fn(new NormalFunction(address, debug_info_));
				functions_.insert(std::make_pair(address,
						std::shared_ptr<FunctionInfo>(new FunctionInfo(fn))));
			}
			BeginFunction(address, amx_->frm);
		}
	} else if (amx_->frm > prevFrame) {
		assert(call_stack_.GetTop()->function()->type() == "normal");
		EndFunction();
	}
	if (debug != 0) {
		return debug(amx_);
	}
	return AMX_ERR_NONE;
}

// Profile a native function.
int Profiler::amx_Callback(cell index, cell *result, cell *params,
	int (AMXAPI *callback)(AMX *amx, cell index, cell *result, cell *params)) {
	if (callback == 0) {
		callback = ::amx_Callback;
	}
	if (index >= 0) {
		auto address = GetNativeAddress(index);
		if (functions_.find(address) == functions_.end()) {
			std::shared_ptr<Function> fn(new NativeFunction(amx_, index));
			functions_.insert(std::make_pair(address,
					std::shared_ptr<FunctionInfo>(new FunctionInfo(fn))));
		}
		BeginFunction(address, amx_->frm);
		int error = callback(amx_, index, result, params);
		EndFunction(address);
		return error;
	} else {
		return callback(amx_, index, result, params);
	}
}

// Profile a public function.
int Profiler::amx_Exec(cell *retval, int index,
	int (AMXAPI *exec)(AMX *amx, cell *retval, int index)) {
	if (exec == 0) {
		exec = ::amx_Exec;
	}
	if (index >= 0 || index == AMX_EXEC_MAIN) {
		auto address = GetPublicAddress(index);
		if (functions_.find(address) == functions_.end()) {
			std::shared_ptr<Function> fn(new PublicFunction(amx_, index));
			functions_.insert(std::make_pair(address,
					std::shared_ptr<FunctionInfo>(new FunctionInfo(fn))));
		}
		BeginFunction(address, amx_->stk - 3*sizeof(cell));
		int error = exec(amx_, retval, index);
		EndFunction(address);
		return error;
	} else {
		return exec(amx_, retval, index);
	}
}

// Get address of a native function by its index.
ucell Profiler::GetNativeAddress(cell index) {
	if (index >= 0) {
		auto hdr = reinterpret_cast<AMX_HEADER*>(amx_->base);
		auto natives = reinterpret_cast<AMX_FUNCSTUBNT*>(amx_->base + hdr->natives);
		return natives[index].address;
	}
	assert(0 && "Invalid native index");
	return 0;
}

// Get address of a public function by its index.
ucell Profiler::GetPublicAddress(cell index) {
	auto hdr = reinterpret_cast<AMX_HEADER*>(amx_->base);
	if (index >= 0) {
		auto publics = reinterpret_cast<AMX_FUNCSTUBNT*>(amx_->base + hdr->publics);
		return publics[index].address;
	} else if (index == AMX_EXEC_MAIN) {
		return hdr->cip;
	}
	assert(0 && "Invalid public index");
	return 0;
}

// BeginFunction() is called when Profiler enters a function.
void Profiler::BeginFunction(ucell address, ucell frm) {
	assert(functions_.find(address) != functions_.end() && address != 0
			&& "BeginFunction() called with invalid address");
	std::shared_ptr<FunctionInfo> &info = functions_[address];
	info->num_calls()++;	
	call_stack_.Push(info->function(), frm);
	if (call_graph_enabled_) {
		auto node = std::shared_ptr<CallGraphNode>(new CallGraphNode(info, call_graph_.root()));
		call_graph_.root()->AddCallee(node);
		call_graph_.set_root(node);	
	}
}

// EndFunction() is called when Profiler leaves a function.
void Profiler::EndFunction(ucell address) {
	assert(!call_stack_.IsEmpty());
	assert(address == 0 || functions_.find(address) != functions_.end()
			&& "EndFunction() called with invalid address");
	// There still can remain a few "normal" functions at the top of the call stack
	// although they have already returned.
	while (true) {
		auto current = call_stack_.Pop();
		auto current_it = functions_.find(current->function()->address());
		assert(current_it != functions_.end());
		if (current->IsRecursive()) {
			current_it->second->child_time() -= current->timer().child_time<Nanoseconds>();
		} else {
			current_it->second->total_time() += current->timer().total_time<Nanoseconds>();
		}
		if (!call_stack_.IsEmpty()) {
			auto top = call_stack_.GetTop();
			auto top_it = functions_.find(top->function()->address());
			assert(top_it != functions_.end());
			top_it->second->child_time() += current->timer().total_time<Nanoseconds>();
		}
		if (call_graph_enabled_) {
			assert(call_graph_.root() != call_graph_.sentinel());
			call_graph_.set_root(call_graph_.root()->caller());
		}
		if (address == 0 || (current->function()->address() == address)) {
			break;
		}
	}
}

} // namespace amx_profiler
