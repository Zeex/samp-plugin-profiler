// Copyright (c) 2011-2012, Zeex
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <cassert>
#include "function.h"
#include "function_call.h"
#include "function_statistics.h"
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
}

Profiler::~Profiler() {
	for (auto function : functions_) {
		delete function;
	}
}

int Profiler::amx_Debug(int (AMXAPI *debug)(AMX *amx)) {
	cell prev_frame = amx_->stp;

	if (!call_stack_.IsEmpty()) {
		prev_frame = call_stack_.top()->frame();
	}

	if (amx_->frm < prev_frame) {
		if (call_stack_.top()->frame() != amx_->frm) {
			auto address = static_cast<ucell>(amx_->cip) - 2*sizeof(cell);
			auto fn = stats_.GetFunction(address);
			if (fn == 0) {
				fn = new NormalFunction(address, debug_info_);
				functions_.insert(fn);
				stats_.AddFunction(fn);
			}
			BeginFunction(address, amx_->frm);
		}
	} else if (amx_->frm > prev_frame) {
		if (typeid(*call_stack_.top()->function()) == typeid(NormalFunction)) {
			EndFunction();
		}
	}

	if (debug != 0) {
		return debug(amx_);
	}

	return AMX_ERR_NONE;
}

int Profiler::amx_Callback(cell index, cell *result, cell *params,
	int (AMXAPI *callback)(AMX *amx, cell index, cell *result, cell *params))
{
	if (callback == 0) {
		callback = ::amx_Callback;
	}

	if (index >= 0) {
		auto address = GetNativeAddress(index);
		if (address != 0) {
			auto fn = stats_.GetFunction(address);
			if (fn == 0) {
				fn = new NativeFunction(amx_, index);
				functions_.insert(fn);
				stats_.AddFunction(fn);
			}
			BeginFunction(address, amx_->frm);
		}
		int error = callback(amx_, index, result, params);
		if (address != 0) {
			EndFunction(address);
		}
		return error;
	}

	return callback(amx_, index, result, params);
}

int Profiler::amx_Exec(cell *retval, int index,
	int (AMXAPI *exec)(AMX *amx, cell *retval, int index))
{
	if (exec == 0) {
		exec = ::amx_Exec;
	}

	if (index >= 0 || index == AMX_EXEC_MAIN) {
		auto address = GetPublicAddress(index);
		if (address != 0) {
			auto fn = stats_.GetFunction(address);
			if (fn == 0) {
				fn = new PublicFunction(amx_, index);
				functions_.insert(fn);
				stats_.AddFunction(fn);
			}
			BeginFunction(address, amx_->stk - 3*sizeof(cell));
		}
		int error = exec(amx_, retval, index);
		if (address != 0) {
			EndFunction(address);
		}
		return error;
	}

	return exec(amx_, retval, index);
}

ucell Profiler::GetNativeAddress(cell index) {
	if (index >= 0) {
		auto hdr = reinterpret_cast<AMX_HEADER*>(amx_->base);
		auto natives = reinterpret_cast<AMX_FUNCSTUBNT*>(amx_->base + hdr->natives);
		return natives[index].address;
	}
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
	return 0;
}

void Profiler::BeginFunction(ucell address, ucell frm) {
	assert(address != 0);
	auto fn_stats = stats_.GetFunctionStatistis(address);

	assert(fn_stats != 0);
	fn_stats->AdjustNumCalls(1);

	call_stack_.Push(fn_stats->function(), frm);
	if (call_graph_enabled_) {
		call_graph_.root()->AddCallee(fn_stats)->MakeRoot();
	}
}

void Profiler::EndFunction(ucell address) {
	assert(!call_stack_.IsEmpty());
	assert(address == 0 || stats_.GetFunction(address) != 0);

	while (true) {
		auto old_top = call_stack_.Pop();

		auto old_top_fn_stats = stats_.GetFunctionStatistis(old_top.function()->address());
		assert(old_top_fn_stats != 0);

		if (old_top.IsRecursive()) {
			old_top_fn_stats->AdjustChildTime(-old_top.timer()->child_time());
		} else {
			old_top_fn_stats->AdjustTotalTime(old_top.timer()->total_time());
		}

		if (!call_stack_.IsEmpty()) {
			auto top = call_stack_.top();
			auto top_fn_stats = stats_.GetFunctionStatistis(top->function()->address());
			assert(top_fn_stats != 0);
			top_fn_stats->AdjustChildTime(old_top.timer()->total_time());
		}

		if (call_graph_enabled_) {
			assert(call_graph_.root() != call_graph_.sentinel());
			call_graph_.set_root(call_graph_.root()->caller());
		}

		if (address == 0 || (old_top.function()->address() == address)) {
			break;
		}
	}
}

} // namespace amx_profiler
