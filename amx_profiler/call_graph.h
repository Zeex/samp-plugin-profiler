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

#ifndef AMX_PROFILER_CALL_GRAPH_H
#define AMX_PROFILER_CALL_GRAPH_H

#include <set>
#include <vector>

namespace amx_profiler {

class CallGraphNode;
class CallGraphWriter;
class FunctionStatistics;

class CallGraph {
	friend class CallGraphNode;

public:
	CallGraph(CallGraphNode *root = 0);
	~CallGraph();

	inline CallGraphNode *root() const {
		return root_;
	}

	inline void set_root(CallGraphNode *root) {
		root_ = root;
	}

	inline CallGraphNode *sentinel() const {
		return sentinel_;
	}

	void Write(CallGraphWriter *writer) const;

	template<typename F>
	inline void Traverse(F f) const {
		sentinel_->Traverse(f);
	}

private:
	void OwnNode(CallGraphNode *node);

private:
	CallGraphNode *root_;
	CallGraphNode *sentinel_;
	std::set<CallGraphNode*> nodes_;
};

class CallGraphNode {
public:
	class Compare { 
	public:
		bool operator()(const CallGraphNode *n1, const CallGraphNode *n2) const;
	};

	CallGraphNode(CallGraph *graph, FunctionStatistics *stats, CallGraphNode *caller = 0);

	void MakeRoot() {
		graph_->set_root(this);
	}

	inline CallGraph *graph() const {
		return graph_;
	}

	inline FunctionStatistics *stats() const {
		return stats_;
	}

	inline CallGraphNode *caller() const {
		return caller_;
	}

	inline const std::set<CallGraphNode*, Compare> &callees() const {
		return callees_;
	}

	CallGraphNode *AddCallee(FunctionStatistics *stats);
	CallGraphNode *AddCallee(CallGraphNode *node);

	// Recursive parent-to-child traversing.
	template<typename F>
	inline void Traverse(F f) const {
		f(this);
		for (auto c : callees_) {
			c->Traverse(f);
		}
	}

private:
	CallGraph *graph_;
	FunctionStatistics *stats_;
	CallGraphNode *caller_;
	std::set<CallGraphNode*, Compare> callees_;
};

} // namespace amx_profiler

#endif // !AMX_PROFILER_CALL_GRAPH_H
