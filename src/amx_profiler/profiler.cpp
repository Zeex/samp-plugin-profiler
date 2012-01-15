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
#include "function_info.h"
#include "native_function.h"
#include "normal_function.h"
#include "profile_writer.h"
#include "profiler.h"
#include "public_function.h"

namespace amx_profiler {

// statics
std::unordered_map<AMX*, std::shared_ptr<Profiler>> Profiler::instances_;

Profiler::Profiler(AMX *amx, DebugInfo debug_info)
	: amx_(amx)
	, debug_info_(debug_info)
{
}

Profiler::~Profiler() {}

// static
void Profiler::Attach(AMX *amx, DebugInfo debug_info) {
	instances_[amx] = std::shared_ptr<Profiler>(new Profiler(amx, debug_info));
}

// static
void Profiler::Detach(AMX *amx) {
	instances_.erase(amx);
}

// static
std::shared_ptr<Profiler> Profiler::GetInstance(AMX *amx) {
	return instances_[amx];
}

std::vector<std::shared_ptr<FunctionInfo>> Profiler::GetProfile() const {
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


int Profiler::AmxDebugHook() {
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
			EnterFunction(address, amx_->frm);
		}
	} else if (amx_->frm > prevFrame) {
		assert(call_stack_.GetTop()->function()->type() == "normal");
		LeaveFunction();
	}
	return AMX_ERR_NONE;
}

int Profiler::AmxCallbackHook(cell index, cell *result, cell *params) {
	if (index >= 0) {
		auto address = GetNativeAddress(index);
		if (functions_.find(address) == functions_.end()) {
			std::shared_ptr<Function> fn(new NativeFunction(amx_, index));
			functions_.insert(std::make_pair(address,
					std::shared_ptr<FunctionInfo>(new FunctionInfo(fn))));
		}
		EnterFunction(address, amx_->frm);
		int error = amx_Callback(amx_, index, result, params);
		assert(call_stack_.GetTop()->function()->type() == "native");
		LeaveFunction(address);
		return error;
	} else {
		return amx_Callback(amx_, index, result, params);
	}
}

int Profiler::AmxExecHook(cell *retval, int index) {
	if (index >= 0 || index == AMX_EXEC_MAIN) {
		auto address = GetPublicAddress(index);
		if (functions_.find(address) == functions_.end()) {
			std::shared_ptr<Function> fn(new PublicFunction(amx_, index));
			functions_.insert(std::make_pair(address,
					std::shared_ptr<FunctionInfo>(new FunctionInfo(fn))));
		}
		EnterFunction(address, amx_->stk - 3*sizeof(cell));
		int error = amx_Exec(amx_, retval, index);
		assert(call_stack_.GetTop()->function()->type() == "public");
		LeaveFunction(address);
		return error;
	} else {
		return amx_Exec(amx_, retval, index);
	}
}

ucell Profiler::GetNativeAddress(cell index) {
	if (index >= 0) {
		auto hdr = reinterpret_cast<AMX_HEADER*>(amx_->base);
		auto natives = reinterpret_cast<AMX_FUNCSTUBNT*>(amx_->base + hdr->natives);
		return natives[index].address;
	}
	assert(0 && "Invalid native index");
	return 0;
}

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

void Profiler::EnterFunction(ucell address, ucell frm) {
	assert(functions_.find(address) != functions_.end() && address != 0
			&& "EnterFunction() called with invalid address");
	std::shared_ptr<FunctionInfo> &info = functions_[address];
	call_stack_.Push(info->function(), frm);
	info->num_calls()++;
}

void Profiler::LeaveFunction(ucell address) {
	assert(!call_stack_.IsEmpty());
	assert(address == 0 || functions_.find(address) != functions_.end()
			&& "LeaveFunction() called with invalid address");
	while (true) {
		auto current = call_stack_.Pop();
		auto current_it = functions_.find(current->function()->address());
		assert(current_it != functions_.end());
		if (current->IsRecursive()) {
			current_it->second->child_time() -= current->timer().child_time<Microseconds>();
		} else {
			current_it->second->total_time() += current->timer().total_time<Microseconds>();
		}
		if (!call_stack_.IsEmpty()) {
			auto top = call_stack_.GetTop();
			auto top_it = functions_.find(top->function()->address());
			assert(top_it != functions_.end());
			top_it->second->child_time() += current->timer().total_time<Microseconds>();
		}
		if (address == 0 || (current->function()->address() == address)) {
			break;
		}
	}
}

} // namespace amx_profiler
