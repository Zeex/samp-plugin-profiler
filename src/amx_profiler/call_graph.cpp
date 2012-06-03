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

bool CallGraph::CompareNodes::operator()(const std::shared_ptr<CallGraphNode> &left,
		        const std::shared_ptr<CallGraphNode> &right) {
	return left->info()->function()->address() < right->info()->function()->address();
}

CallGraphNode::CallGraphNode(const std::shared_ptr<FunctionInfo> &info, 
                             const std::shared_ptr<CallGraphNode> &caller) 
	: info_(info)
	, caller_(caller)
	, std::enable_shared_from_this<CallGraphNode>()
{
}

void CallGraphNode::AddCallee(const std::shared_ptr<FunctionInfo> &info) {
	auto new_node = std::shared_ptr<CallGraphNode>(new CallGraphNode(info, shared_from_this()));
	AddCallee(new_node);
}

void CallGraphNode::AddCallee(const std::shared_ptr<CallGraphNode> &node) {
	callees_.insert(node);

}

CallGraph::CallGraph(const std::shared_ptr<CallGraphNode> &root)
	: root_(root)
	, sentinel_(new CallGraphNode(0))
{
	if (!root) {
		root_ = sentinel_;
	}
}

void CallGraph::Write(CallGraphWriter &writer) const {
	writer.Write(*this);
}

} // namespace amx_profiler
