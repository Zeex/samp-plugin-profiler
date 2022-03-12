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

#include "call_graph.h"
#include "function.h"
#include "function_statistics.h"

namespace amxprof {

namespace {

class Deleter : public CallGraph::Visitor {
 public:
  virtual void Visit(const CallGraphNode *node) {
    delete node;
  }
};

} // anonymous namespace

bool CallGraph::CompareStats::operator()(const FunctionStatistics *lhs,
                                         const FunctionStatistics *rhs) const {
  return lhs->function()->address() < rhs->function()->address();
}

CallGraph::CallGraph()
 : sentinel_(new CallGraphNode(this, 0))
{
}

CallGraph::~CallGraph() {
  Deleter deleter;
  Traverse(&deleter);
}

CallGraphNode *CallGraph::PushCall(FunctionStatistics *stats) {
  CallGraphNode *node = 0;
  NodeMap::iterator iterator = nodes_.find(stats);
  if (iterator == nodes_.end()) {
    node = new CallGraphNode(this, stats);
    nodes_.insert(std::make_pair(stats, node));
  } else {
    node = iterator->second;
  }
  if (call_stack_.empty()) {
    sentinel_->AddCallee(node);
  } else {
    call_stack_.top()->AddCallee(node);
  }
  call_stack_.push(node);
  return node;
}

CallGraphNode *CallGraph::PopCall() {
  CallGraphNode *node = 0;
  if (!call_stack_.empty()) {
    node = call_stack_.top();
    call_stack_.pop();
  }
  return node;
}

void CallGraph::Traverse(Visitor *visitor) const {
  visitor->Visit(sentinel_);
  for (NodeMap::const_iterator iterator = nodes_.begin();
       iterator != nodes_.end(); ++iterator) {
    visitor->Visit(iterator->second);
  }
}

CallGraphNode::CallGraphNode(CallGraph *graph,
                             FunctionStatistics *stats)
 : graph_(graph),
   stats_(stats)
{
}

CallGraphNode *CallGraphNode::AddCallee(CallGraphNode *node) {
  callees_.insert(node);
  return node;
}

} // namespace amxprof
