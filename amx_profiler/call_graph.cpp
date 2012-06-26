// Copyright (c) 2011-2012, Zeex
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met: 
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer. 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "call_graph.h"
#include "call_graph_writer.h"
#include "function.h"
#include "function_info.h"
#include "time_interval.h"

namespace amx_profiler {

CallGraph::CallGraph(CallGraphNode *root)
	: root_(root)
	, sentinel_(new CallGraphNode(this, 0))
{
	if (!root) {
		root_ = sentinel_;
	}
}

CallGraph::~CallGraph() {
	delete sentinel_;
	for (auto node : nodes_) {
		delete node;
	}
}

void CallGraph::Write(CallGraphWriter *writer) const {
	writer->Write(this);
}

void CallGraph::OwnNode(CallGraphNode *node) {
	nodes_.insert(node);
}

bool CallGraphNode::Compare::operator()(const CallGraphNode *n1, const CallGraphNode *n2) const {
	return n1->info()->function()->address() < n2->info()->function()->address();
}

CallGraphNode::CallGraphNode(CallGraph *graph, FunctionInfo *info, CallGraphNode *caller) 
	: graph_(graph)
	, info_(info)
	, caller_(caller)
{
}

CallGraphNode *CallGraphNode::AddCallee(FunctionInfo *info) {
	auto node = new CallGraphNode(graph_, info, this);
	return AddCallee(node);
}

CallGraphNode *CallGraphNode::AddCallee(CallGraphNode *node) {
	graph_->OwnNode(node);
	callees_.insert(node);
	return node;
}

} // namespace amx_profiler
