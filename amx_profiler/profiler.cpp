// Copyright (c) 2011-2013, Zeex
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
#include "amx_utils.h"
#include "function.h"
#include "function_call.h"
#include "function_statistics.h"
#include "profiler.h"

namespace amx_profiler {

Profiler::Profiler(AMX *amx, DebugInfo *debug_info, bool enable_call_graph)
	: amx_(amx)
	, debug_info_(debug_info)
	, call_graph_enabled_(enable_call_graph)
{
}

Profiler::~Profiler() {
	for (FunctionSet::const_iterator iterator = functions_.begin();
			iterator != functions_.end(); ++iterator) {
		delete *iterator;
	}
}

int Profiler::DebugHook(DebugHookFunc debug) {
	cell prev_frame = amx_->stp;

	if (!call_stack_.IsEmpty()) {
		prev_frame = call_stack_.top()->frame();
	}

	if (amx_->frm < prev_frame) {
		if (call_stack_.top()->frame() != amx_->frm) {
			ucell address = static_cast<ucell>(amx_->cip) - 2*sizeof(cell);
			Function *fn = stats_.GetFunction(address);
			if (fn == 0) {
				fn = Function::Normal(address, debug_info_);
				functions_.insert(fn);
				stats_.AddFunction(fn);
			}
			BeginFunction(address, amx_->frm);
		}
	} else if (amx_->frm > prev_frame) {
		if (call_stack_.top()->function()->type() == Function::NORMAL) {
			EndFunction();
		}
	}

	if (debug != 0) {
		return debug(amx_);
	}

	return AMX_ERR_NONE;
}

int Profiler::CallbackHook(cell index, cell *result, cell *params, CallbackHookFunc callback) {
	if (callback == 0) {
		callback = ::amx_Callback;
	}

	if (index >= 0) {
		ucell address = GetNativeAddress(amx_, index);
		if (address != 0) {
			Function *fn = stats_.GetFunction(address);
			if (fn == 0) {
				fn = Function::Native(amx_, index);
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

int Profiler::ExecHook(cell *retval, int index, ExecHookFunc exec) {
	if (exec == 0) {
		exec = ::amx_Exec;
	}

	if (index >= 0 || index == AMX_EXEC_MAIN) {
		ucell address = GetPublicAddress(amx_, index);
		if (address != 0) {
			Function *fn = stats_.GetFunction(address);
			if (fn == 0) {
				fn = Function::Public(amx_, index);
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

void Profiler::BeginFunction(ucell address, ucell frm) {
	assert(address != 0);
	FunctionStatistics *fn_stats = stats_.GetFunctionStatistis(address);

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
		FunctionCall old_top = call_stack_.Pop();

		FunctionStatistics *old_top_fn_stats = stats_.GetFunctionStatistis(old_top.function()->address());
		assert(old_top_fn_stats != 0);

		if (old_top.IsRecursive()) {
			// Don't count total time twice if the function calls itself directly
			// or indirectly. Because PerformanceCounter isn't aware of recursion
			// and includes both times we have to subtract the total time of the
			// callee manually.
			old_top_fn_stats->AdjustTotalTime(-old_top.timer()->total_time());
		} else {
			old_top_fn_stats->AdjustTotalTime(old_top.timer()->total_time());
		}

		// Unlike total time, self time includes both parent and child calls
		// because it's actually not counted twice.
		old_top_fn_stats->AdjustSelfTime(old_top.timer()->total_time());

		if (!call_stack_.IsEmpty()) {
			FunctionCall *top = call_stack_.top();
			FunctionStatistics *top_fn_stats = stats_.GetFunctionStatistis(top->function()->address());
			assert(top_fn_stats != 0);
			top_fn_stats->AdjustSelfTime(-old_top.timer()->total_time());
		}

		if (call_graph_enabled_) {
			assert(call_graph_.root() != call_graph_.sentinel());
			call_graph_.set_root(call_graph_.root()->caller());
		}

		if (address == 0 || old_top.function()->address() == address) {
			break;
		}
	}
}

} // namespace amx_profiler
