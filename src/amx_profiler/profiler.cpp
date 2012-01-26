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

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <functional>
#include <map>
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

Profiler::Profiler(AMX *amx, DebugInfo debug_info)
	: amx_(amx)
	, debug_info_(debug_info)
{
	// nothing
}

std::vector<std::shared_ptr<FunctionInfo>> Profiler::GetProfile() const {
	// I'm sure this can be done more elegant with the new C++11 features
	// but I just haven't figured out how...
	std::vector<std::shared_ptr<FunctionInfo>> profile;
	for (auto iterator = functions_.begin(); iterator != functions_.end(); ++iterator) {
		profile.push_back(iterator->second);
	}
	return profile;
}

void Profiler::WriteProfile(const std::string &script_name,
	                        ProfileWriter *writer,
	                        std::ostream &stream) const
{
	writer->Write(script_name, stream, GetProfile());
}

// Profile normal function call.
int Profiler::amx_Debug(int (AMXAPI *debug)(AMX *amx)) {
	// Check if the stack frame has changed.
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

// Get native function address by index.
ucell Profiler::GetNativeAddress(cell index) {
	if (index >= 0) {
		auto hdr = reinterpret_cast<AMX_HEADER*>(amx_->base);
		auto natives = reinterpret_cast<AMX_FUNCSTUBNT*>(amx_->base + hdr->natives);
		return natives[index].address;
	}
	assert(0 && "Invalid native index");
	return 0;
}

// Get public function address by index.
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

// BeginFunction() gets called when Profiler enters a function.
void Profiler::BeginFunction(ucell address, ucell frm) {
	assert(functions_.find(address) != functions_.end() && address != 0
			&& "BeginFunction() called with invalid address");
	std::shared_ptr<FunctionInfo> &info = functions_[address];
	info->num_calls()++;	
	call_stack_.Push(info->function(), frm);
	auto node = std::shared_ptr<CallGraphNode>(new CallGraphNode(info, call_graph_.root()));
	call_graph_.root()->AddCallee(node);
	call_graph_.set_root(node);	
}

// EndFunction() gets called when Profiler has left a function.
void Profiler::EndFunction(ucell address) {
	assert(!call_stack_.IsEmpty());
	assert(address == 0 || functions_.find(address) != functions_.end()
			&& "EndFunction() called with invalid address");
	// There still can remain a few "normal" functions in the call stack
	// though they have already finished. 
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
		assert(call_graph_.root() != call_graph_.sentinel().lock()
				&& "Call graph sentinel reached");
		call_graph_.set_root(call_graph_.root()->caller());
		if (address == 0 || (current->function()->address() == address)) {
			break;
		}
	}
}

} // namespace amx_profiler
