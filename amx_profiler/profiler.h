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

#ifndef AMX_PROFILER_PROFILER_H
#define AMX_PROFILER_PROFILER_H

#include <set>
#include "amx_types.h"
#include "call_graph.h"
#include "call_stack.h"
#include "debug_info.h"
#include "function_statistics.h"
#include "macros.h"
#include "statistics.h"

namespace amx_profiler {

class Profiler {
 public:
  typedef std::set<Function*> FunctionSet;

 public:
  Profiler(AMX *amx, DebugInfo *debug_info = 0);
  ~Profiler();

  bool call_graph_enabled() const { return call_graph_enabled_; }
  void set_call_graph_enabled(bool enabled) { call_graph_enabled_ = enabled; }

  const CallStack *call_stack() const { return &call_stack_; }
  const CallGraph *call_graph() const { return &call_graph_; }

  // Retruns collected runtime statistics.
  const Statistics *stats() const { return &stats_;  }

  // This method should be called instead of amx_Exec(). It
  // collects information about public function calls.
  int ExecHook(cell *retval, int index, AMX_EXEC exec = 0);

  // This method should be called from within AMX debug hook (see
  // amx_SetDebugHook). It collects information about ordinary
  // function calls.
  int DebugHook(AMX_DEBUG debug = 0);

  // This method should be called instead of amx_Callback().
  // It collects information about native function calls.
  int CallbackHook(cell index, cell  *result, cell *params, AMX_CALLBACK callback = 0);

 private:
  Profiler();

  // BeginFunction() and EndFunction() are called when entering a function
  // (of either type) and returning from it respectively.
  void BeginFunction(Address address, Address frm);
  void EndFunction(Address address = 0);

 private:
  AMX *amx_;
  DebugInfo *debug_info_;

  bool call_graph_enabled_;

  CallStack call_stack_;
  CallGraph call_graph_;

  Statistics stats_;
  FunctionSet functions_;

 private:
  DISALLOW_COPY_AND_ASSIGN(Profiler);
};

} // namespace amx_profiler

#endif // !AMX_PROFILER_PROFILER_H
