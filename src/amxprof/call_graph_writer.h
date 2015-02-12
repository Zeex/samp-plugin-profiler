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

#ifndef AMXPROF_CALL_GRAPH_WIRTER_H
#define AMXPROF_CALL_GRAPH_WIRTER_H

#include <iosfwd>
#include <string>
#include "call_graph.h"

namespace amxprof {

class CallGraphWriter {
 public:
  class Visitor : public CallGraph::Visitor {
   public:
    Visitor(CallGraphWriter *writer) : writer_(writer) {}
    virtual void Visit(const CallGraphNode *node) = 0;
   protected:
    CallGraphWriter *writer_;
  };

  CallGraphWriter();
  virtual ~CallGraphWriter();

  virtual void Write(const CallGraph *graph) = 0;

  std::ostream *stream() const { return stream_; }
  void set_stream(std::ostream *stream) { stream_ = stream; }
  
  std::string script_name() const { return script_name_; }
  void set_script_name(std::string script_name) { script_name_ = script_name; }

  std::string root_node_name() const { return root_node_name_; }
  void set_root_node_name(std::string root_node_name) { root_node_name_ = root_node_name; }

 private:
  std::ostream *stream_;
  std::string script_name_;
  std::string root_node_name_;
};

} // namespace amxprof

#endif // !AMXPROF_CALL_GRAPH_WIRTER_H
