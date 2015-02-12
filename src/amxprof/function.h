// Copyright (c) 2011-2015 Zeex
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

#ifndef AMXPROF_FUNCTION_FUNCTION_H
#define AMXPROF_FUNCTION_FUNCTION_H

#include <string>
#include "amx_types.h"

namespace amxprof {

class DebugInfo;

class Function {
 public:
  enum Type {
    NORMAL, // non-public functions
    PUBLIC, // public functions
    NATIVE  // native functions
  };

  // Caller is reponsible for deleting returned Function objects.
  static Function *Normal(Address address, DebugInfo *debug_info = 0);
  static Function *Public(AMX *amx, PublicTableIndex index);
  static Function *Native(AMX *amx, NativeTableIndex index);

  // Returns the type of the function.
  Type type() const {
    return type_;
  }

  // Returns type() as a string.
  const char *GetTypeString() const;

  // Returns address of the function. Addresses are unique among
  // all types of functions, i.e. there can't exist a public and
  // a native with the same address.
  Address address() const {
    return address_;
  }

  // Returns the name of the function. Public and native functions
  // always have a name. Ordinary functions' names are extract from
  // debugging symbols provided at construction time; if there was
  // no debug info provided or the function was not found among it
  // the name is built from the string "unknown@" followed by the
  // function address in hex.
  std::string name() const {
    return name_;
  }

  // Comparison operators.
  bool operator==(const Function &other) const {
    return address_ == other.address_;
  }
  bool operator!=(const Function &other) const {
    return !operator==(other);
  }
  bool operator<(const Function &other) const {
    return address_ < other.address_;
  }

 private:
  Function(Type type, Address address, std::string name);

 private:
  Type type_;
  Address address_;
  std::string name_;
};

} // namespace amxprof

#endif // !AMXPROF_FUNCTION_FUNCTION_H
