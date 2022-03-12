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

#ifndef AMXPROF_PROFILER_H
#define AMXPROF_PROFILER_H

#include <set>
#include "amx_types.h"
#include "call_graph.h"
#include "call_stack.h"
#include "debug_info.h"
#include "function_statistics.h"
#include "macros.h"
#include "statistics.h"

namespace amxprof {

class Profiler {
 public:
  Profiler(AMX *amx, bool enable_call_graph = false);
  ~Profiler();

 public:
  const Statistics *stats() const { return &stats_; }

  const CallStack *call_stack() const { return &call_stack_; }
  const CallGraph *call_graph() const { return &call_graph_; }

  // Debug info is needed for function names. If not set the functions
  // will be shown as "unknown@XXXXXXXX" where XXXXXXXX is the AMX code
  // offset (except for public functions, whose names are duplicated
  // in the AMX name table).
  void set_debug_info(DebugInfo *debug_info) {
    debug_info_ = debug_info;
  }

 public:
  // This method should be called from within your AMX debug hook (see
  // amx_SetDebugHook). It collects statistics for ordinary functions.
  int DebugHook(AMX_DEBUG debug = 0);

  // This method should be called instead of amx_Callback().
  // It collects statistics for native functions.
  int CallbackHook(cell index,
                   cell *result,
                   cell *params,
                   AMX_CALLBACK callback = 0);

  // This method should be called instead of amx_Exec().
  // It collects statistics for public functions.
  int ExecHook(cell *retval, int index, AMX_EXEC exec = 0);

 private:
  Profiler();

  // BeginFunction() and EndFunction() are called when entering
  // a function and returning from it respectively.
  void EnterFunction(Address address, Address frm);
  void LeaveFunction(Address address, Address frm);

 private:
  AMX *amx_;
  DebugInfo *debug_info_;
  bool call_graph_enabled_;
  CallStack call_stack_;
  CallGraph call_graph_;
  Statistics stats_;
  std::set<Function*> functions_;

 private:
  AMXPROF_DISALLOW_COPY_AND_ASSIGN(Profiler);
};

} // namespace amxprof

#endif // !AMXPROF_PROFILER_H
