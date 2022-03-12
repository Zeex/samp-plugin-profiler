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

#ifndef AMXPROF_CALL_GRAPH_WRITER_DOT_H
#define AMXPROF_CALL_GRAPH_WRITER_DOT_H

#include "call_graph_writer.h"
#include "duration.h"

namespace amxprof {

class CallGraphNode;

class CallGraphWriterDot : public CallGraphWriter {
 public:
  virtual void Write(const CallGraph *graph);

 private:
  class WriteNode : public CallGraphWriter::Visitor {
   public:
    WriteNode(CallGraphWriter *writer)
     : CallGraphWriter::Visitor(writer)
    {}
    virtual void Visit(const CallGraphNode *node);
  };

  class WriteNodeColor : public CallGraphWriter::Visitor {
   public:
    WriteNodeColor(CallGraphWriter *writer, Nanoseconds max_time)
     : CallGraphWriter::Visitor(writer),
       max_time_(max_time)
    {}
    virtual void Visit(const CallGraphNode *node);
   private:
    Nanoseconds max_time_;
  };

  class ComputeMaxTime : public CallGraphWriter::Visitor {
   public:
    ComputeMaxTime(CallGraphWriter *writer)
     : CallGraphWriter::Visitor(writer)
    {}
    virtual void Visit(const CallGraphNode *node);
    Nanoseconds max_time() const { return max_time_; }
   private:
    Nanoseconds max_time_;
  };
};

} // namespace amxprof

#endif // !AMXPROF_CALL_GRAPH_WRITER_DOT_H


