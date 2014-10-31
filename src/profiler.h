// Copyright (c) 2011-2014 Zeex
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

#ifndef PROFILER_H
#define PROFILER_H

#include <exception>
#include <amxprof/debug_info.h>
#include <amxprof/profiler.h>
#include "amxservice.h"
#include "configreader.h"

enum ProfilerState {
  PROFILER_DISABLED,
  PROFILER_ATTACHED,
  PROFILER_STARTING,
  PROFILER_STARTED,
  PROFILER_STOPPING,
  PROFILER_STOPPED
};

class Profiler : public AMXService<Profiler> {
 friend class AMXService<Profiler>;

 public:
  int Load();
  int Unload();

  int Debug();
  int Callback(cell index, cell *result, cell *params);
  int Exec(cell *retval, int index);

 public:
  ProfilerState GetState() const;
  bool Start();
  bool Stop();
  bool Dump() const;

 private:
  Profiler(AMX *amx);

 private:
  AMX_DEBUG prev_debug_;
  AMX_CALLBACK prev_callback_;
  amxprof::Profiler profiler_;
  amxprof::DebugInfo debug_info_;
  ProfilerState state_;

 private:
  static ConfigReader server_cfg_;
  static bool profile_gamemode_;
  static std::string profile_filterscripts_;
  static std::string profile_format_;
  static bool call_graph_;
  static std::string call_graph_format_;
};

#endif // !PROFILER_H
