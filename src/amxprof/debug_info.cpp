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

#include <cstdio>
#include <cstring>
#include <string>
#include "debug_info.h"

namespace amxprof {

DebugInfo::DebugInfo()
 : amxdbg_(0),
   last_error_(AMX_ERR_NONE)
{
}

DebugInfo::DebugInfo(const AMX_DBG *amxdbg) 
 : amxdbg_(new AMX_DBG),
   last_error_(AMX_ERR_NONE)
{
  std::memcpy(amxdbg_, amxdbg, sizeof(AMX_DBG));
}

DebugInfo::DebugInfo(const std::string &filename)
 : amxdbg_(0),
   last_error_(AMX_ERR_NONE)
{
  Load(filename);
}

bool DebugInfo::Load(const std::string &filename) {
  std::FILE* fp = std::fopen(filename.c_str(), "rb");
  if (fp != 0) {
    AMX_DBG amxdbg;
    last_error_ = dbg_LoadInfo(&amxdbg, fp);
    if (last_error_ == AMX_ERR_NONE) {
      amxdbg_ = new AMX_DBG(amxdbg);
      return true;
    }
    fclose(fp);
  }
  return false;
}

void DebugInfo::Unload() {
  if (amxdbg_ != 0) {
    last_error_ = dbg_FreeInfo(amxdbg_);
    delete amxdbg_;
  }
}

long DebugInfo::LookupLine(Address address) const {
  long line = 0;
  last_error_ = dbg_LookupLine(amxdbg_, address, &line);
  return line;
}

std::string DebugInfo::LookupFile(Address address) const {
  std::string result;
  const char *file;
  last_error_ = dbg_LookupFile(amxdbg_, address, &file);
  if (last_error_ == AMX_ERR_NONE) {
    result.assign(file);
  }
  return result;
}

std::string DebugInfo::LookupFunction(Address address) const {
  std::string result;
  const char *function;
  last_error_ = dbg_LookupFunction(amxdbg_, address, &function);
  if (last_error_ == AMX_ERR_NONE) {
    result.assign(function);
  }
  return result;
}

std::string DebugInfo::LookupFunctionExact(Address address) const {
  std::string result;
  const char *function;
  last_error_ = dbg_LookupFunctionExact(amxdbg_, address, &function);
  if (last_error_ == AMX_ERR_NONE) {
    result.assign(function);
  }
  return result;
}

bool HasDebugInfo(AMX *amx) {
  uint16_t flags;
  amx_Flags(amx, &flags);
  return ((flags & AMX_FLAG_DEBUG) != 0);
}  

} // namespace amxprof

