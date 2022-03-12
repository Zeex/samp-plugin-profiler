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

#include <cassert>
#include <iomanip>
#include <sstream>
#include <string>
#include "amx_utils.h"
#include "debug_info.h"
#include "function.h"

namespace amxprof {

Function::Function(Type type, Address address, std::string name)
 : type_(type),
   address_(address),
   name_(name)
{
}

// static
Function *Function::Normal(Address address, DebugInfo *debug_info) {
  std::string name;

  if (address != 0 && debug_info != 0 && debug_info->is_loaded()) {
    name = debug_info->LookupFunctionExact(address);
  }

  if (name.empty()) {
    std::stringstream ss;
    ss << std::setw(8) << std::setfill('0') << std::hex << address;
    name.append("unknown@").append(ss.str());
  }

  return new Function(NORMAL, address, name);
}

// static
Function *Function::Public(AMX *amx, PublicTableIndex index) {
  return new Function(PUBLIC, GetPublicAddress(amx, index), GetPublicName(amx, index));
}

// static
Function *Function::Native(AMX *amx, NativeTableIndex index) {
  return new Function(NATIVE, GetNativeAddress(amx, index), GetNativeName(amx, index));
}

const char *Function::GetTypeString() const {
  switch (type_) {
    case NORMAL:
      return "normal";
    case PUBLIC:
      return "public";
    case NATIVE:
      return "native";
    default:
      return "unknown";
  }
}

} // namespace amxprof
