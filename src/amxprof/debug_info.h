// Copyright (c) 2011-2016 Zeex
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

#ifndef AMXPROF_DEBUG_INFO_H
#define AMXPROF_DEBUG_INFO_H

#include <string>
#include <amx/amx.h>
#include <amx/amxdbg.h>
#include "amx_types.h"
#include "macros.h"

namespace amxprof {

class DebugInfo {
 public:
  DebugInfo();
  explicit DebugInfo(const AMX_DBG *amxdbg);
  explicit DebugInfo(const std::string &filename);

  bool Load(const std::string &filename);
  void Unload();

  bool is_loaded() const { return amxdbg_ != 0; }

  long LookupLine(Address address) const;
  std::string LookupFile(Address address) const;
  std::string LookupFunction(Address address) const;
  std::string LookupFunctionExact(Address address) const;

  int last_error() const { return last_error_; }

 private:
  AMX_DBG *amxdbg_;
  mutable int last_error_;

 private:
  AMXPROF_DISALLOW_COPY_AND_ASSIGN(DebugInfo);
};

bool HasDebugInfo(AMX *amx);

} // namespace amxprof

#endif // !AMXPROF_DEBUGINFO_H
