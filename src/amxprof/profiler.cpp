// Copyright (c) 2011-2019 Zeex
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

namespace amxprof {

Profiler::Profiler(AMX *amx, bool enable_call_graph)
 : amx_(amx),
   debug_info_(0),
   call_graph_enabled_(enable_call_graph)
{
}

Profiler::~Profiler() {
  for (std::set<Function*>::const_iterator iterator = functions_.begin();
       iterator != functions_.end(); ++iterator) {
    delete *iterator;
  }
}

int Profiler::DebugHook(AMX_DEBUG debug) {
  Address prev_frame = call_stack_.is_empty()
    ? amx_->stp
    : prev_frame = call_stack_.top()->frame();

  if (amx_->frm < prev_frame) {
    if (prev_frame != amx_->frm) {
      Address address = GetCalleeAddress(amx_, amx_->frm);
      if (address != 0) {
        Function *fn = stats_.GetFunction(address);
        if (fn == 0) {
          fn = Function::Normal(address, debug_info_);
          functions_.insert(fn);
          stats_.AddFunction(fn);
        }
        EnterFunction(address, amx_->frm);
      }
    }
  } else if (amx_->frm > prev_frame) {
    if (call_stack_.top()->function()->type() == Function::NORMAL) {
      LeaveFunction(0, amx_->frm);
    }
  }

  if (debug != 0) {
    return debug(amx_);
  }

  return AMX_ERR_NONE;
}

int Profiler::CallbackHook(cell index, cell *result, cell *params, AMX_CALLBACK callback) {
  if (callback == 0) {
    callback = ::amx_Callback;
  }

  if (index >= 0) {
    Address address = GetNativeAddress(amx_, index);
    if (address != 0) {
      Function *fn = stats_.GetFunction(address);
      if (fn == 0) {
        fn = Function::Native(amx_, index);
        functions_.insert(fn);
        stats_.AddFunction(fn);
      }
      EnterFunction(address, amx_->frm);
    }
    int error = callback(amx_, index, result, params);
    if (address != 0) {
      LeaveFunction(address, 0);
    }
    return error;
  }

  return callback(amx_, index, result, params);
}

int Profiler::ExecHook(cell *retval, int index, AMX_EXEC exec) {
  if (exec == 0) {
    exec = ::amx_Exec;
  }

  if (index >= 0 || index == AMX_EXEC_MAIN) {
    Address address = GetPublicAddress(amx_, index);
    if (address != 0) {
      Function *fn = stats_.GetFunction(address);
      if (fn == 0) {
        fn = Function::Public(amx_, index);
        functions_.insert(fn);
        stats_.AddFunction(fn);
      }
      EnterFunction(address, amx_->stk - 3 * sizeof(cell));
    }
    int error = exec(amx_, retval, index);
    if (address != 0) {
      LeaveFunction(address, 0);
    }
    return error;
  }

  return exec(amx_, retval, index);
}

void Profiler::EnterFunction(Address address, Address frame) {
  assert(address != 0);

  FunctionStatistics *fn_stats = stats_.GetFunctionStatistics(address);
  assert(fn_stats != 0);

  fn_stats->AdjustNumCalls(1);

  call_stack_.Push(fn_stats->function(), frame);
  if (call_graph_enabled_) {
    call_graph_.PushCall(fn_stats);
  }
}

void Profiler::LeaveFunction(Address address, Address frame) {
  assert(!call_stack_.is_empty());
  assert(address == 0 || stats_.GetFunction(address) != 0);

  while (!call_stack_.is_empty()) {
    FunctionCall call = call_stack_.Pop();
    FunctionCall *next_call = call_stack_.is_empty() ? 0 : call_stack_.top();

    FunctionStatistics *fn_stats =
      stats_.GetFunctionStatistics(call.function()->address());
    assert(fn_stats != 0);

    fn_stats->AdjustSelfTime(call.timer()->self_time());
    fn_stats->AdjustTotalTime(call.timer()->total_time());

    Nanoseconds total_time = call.timer()->latest_total_time();
    if (total_time > fn_stats->worst_total_time()) {
      fn_stats->set_worst_total_time(total_time);
    }

    Nanoseconds self_time = call.timer()->latest_self_time();
    if (self_time > fn_stats->worst_self_time()) {
      fn_stats->set_worst_self_time(self_time);
    }

    if (call_graph_enabled_) {
      call_graph_.PopCall();
    }

    if (call.function()->address() == address
        || (frame != 0 && next_call != 0 && next_call->frame() >= frame)) {
      break;
    }
  }
}

} // namespace amxprof
