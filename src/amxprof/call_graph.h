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

#ifndef AMXPROF_CALL_GRAPH_H
#define AMXPROF_CALL_GRAPH_H

#include <map>
#include <set>
#include <stack>
#include "macros.h"

namespace amxprof {

class CallGraphNode;
class FunctionStatistics;

class CallGraph {
  friend class CallGraphNode;

 public:
  class Visitor {
   public:
    virtual void Visit(const CallGraphNode *node) = 0;
  };

  class CompareStats {
   public:
     bool operator()(const FunctionStatistics *lhs,
                     const FunctionStatistics *rhs) const;
  };

  typedef std::map<FunctionStatistics*, CallGraphNode*, CompareStats> NodeMap;
  typedef std::stack<CallGraphNode*> NodeStack;

  CallGraph();
  ~CallGraph();

  CallGraphNode *sentinel() const { return sentinel_; }

  CallGraphNode *PushCall(FunctionStatistics *stats);
  CallGraphNode *PopCall();

  void Traverse(Visitor *visitor) const;

 private:
  CallGraphNode *sentinel_;
  NodeMap nodes_;
  NodeStack call_stack_;

 private:
  AMXPROF_DISALLOW_COPY_AND_ASSIGN(CallGraph);
};

class CallGraphNode {
 public:
  class CompareNodes {
   public:
     bool operator()(const CallGraphNode *lhs,
                     const CallGraphNode *rhs) const {
       return CallGraph::CompareStats()(lhs->stats(), rhs->stats());
     }
  };

  CallGraphNode(CallGraph *graph, FunctionStatistics *stats);

  CallGraph *graph() const { return graph_; }
  FunctionStatistics *stats() const { return stats_; }

  const std::set<CallGraphNode*> &callees() const { return callees_; }
  CallGraphNode *AddCallee(CallGraphNode *node);

 private:
  CallGraph *graph_;
  FunctionStatistics *stats_;
  std::set<CallGraphNode*> callees_;

 private:
  AMXPROF_DISALLOW_COPY_AND_ASSIGN(CallGraphNode);
};

} // namespace amxprof

#endif // !AMXPROF_CALL_GRAPH_H
