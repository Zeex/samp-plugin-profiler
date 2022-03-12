// Copyright (c) 2013-2016 Zeex
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

#include "function.h"
#include "function_statistics.h"
#include "statistics.h"

namespace amxprof {

Statistics::Statistics() {
  run_time_counter_.Start();
}

Statistics::~Statistics() {
  for (AddressToFuncStatsMap::const_iterator iterator = address_to_fn_stats_.begin();
       iterator != address_to_fn_stats_.end(); ++iterator)
  {
    delete iterator->second;
  }
}

Function *Statistics::GetFunction(Address address) {
  AddressToFuncStatsMap::const_iterator iterator = address_to_fn_stats_.find(address);
  if (iterator != address_to_fn_stats_.end()) {
    return iterator->second->function();
  }
  return 0;
}

void Statistics::AddFunction(Function *fn) {
  FunctionStatistics *fn_stats = new FunctionStatistics(fn);
  address_to_fn_stats_.insert(std::make_pair(fn->address(), fn_stats));
}

FunctionStatistics *Statistics::GetFunctionStatistics(Address address) const {
  AddressToFuncStatsMap::const_iterator iterator = address_to_fn_stats_.find(address);
  if (iterator != address_to_fn_stats_.end()) {
    return iterator->second;
  }
  return 0;
}

void Statistics::GetStatistics(std::vector<FunctionStatistics*> &stats) const {
  for (AddressToFuncStatsMap::const_iterator iterator = address_to_fn_stats_.begin();
       iterator != address_to_fn_stats_.end(); ++iterator) {
    stats.push_back(iterator->second);
  }
}

} // namespace amxprof
