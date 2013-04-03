// Copyright (c) 2011-2013, Zeex
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

#ifndef AMX_PROFILER_CALL_GRAPH_H
#define AMX_PROFILER_CALL_GRAPH_H

#include <set>

namespace amx_profiler {

class CallGraphNode;
class FunctionStatistics;

class CallGraph {
	friend class CallGraphNode;

public:
	class Visitor {
	public:
		virtual void Visit(const CallGraphNode *node) = 0;
	};

	typedef std::set<CallGraphNode*> NodeSet;

	CallGraph(CallGraphNode *root = 0);
	~CallGraph();

	CallGraphNode *root() const { return root_; }
	void set_root(CallGraphNode *root) { root_ = root;}

	CallGraphNode *sentinel() const { return sentinel_; }

	void Traverse(Visitor *visitor) const;

private:
	void OwnNode(CallGraphNode *node);

private:
	CallGraphNode *root_;
	CallGraphNode *sentinel_;
	NodeSet nodes_;
};

class CallGraphNode {
public:
	class Compare {
	public:
		bool operator()(const CallGraphNode *n1, const CallGraphNode *n2) const;
	};

	typedef std::set<CallGraphNode*, Compare> CalleeSet;

	CallGraphNode(CallGraph *graph, FunctionStatistics *stats, CallGraphNode *caller = 0);

	void MakeRoot() { graph_->set_root(this); }

	CallGraph *graph() const { return graph_; }
	FunctionStatistics *stats() const { return stats_; }

	CallGraphNode *caller() const { return caller_; }
	const CalleeSet &callees() const { return callees_; }

	CallGraphNode *AddCallee(FunctionStatistics *stats);
	CallGraphNode *AddCallee(CallGraphNode *node);

	void Traverse(CallGraph::Visitor *visitor) const;

private:
	CallGraph *graph_;
	FunctionStatistics *stats_;
	CallGraphNode *caller_;
	CalleeSet callees_;
};

} // namespace amx_profiler

#endif // !AMX_PROFILER_CALL_GRAPH_H
