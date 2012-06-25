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

#include <memory>
#include <set>
#include <vector>

namespace amx_profiler {

class CallGraphNode;
class CallGraphWriter;
class FunctionInfo;

class CallGraph {
public:
	// This allows us to sort nodes by their Functions.
	class CompareNodes { 
	public:
		bool operator()(const std::shared_ptr<CallGraphNode> &left,
		                const std::shared_ptr<CallGraphNode> &right);
	};

	typedef std::set<std::shared_ptr<CallGraphNode>, CompareNodes> NodeSet;

	CallGraph(const std::shared_ptr<CallGraphNode> &root = 0);

	inline std::shared_ptr<CallGraphNode> root() const {
		return root_;
	}

	inline void set_root(const std::shared_ptr<CallGraphNode> &root) {
		root_ = root;
	}

	std::shared_ptr<CallGraphNode> sentinel() const {
		return sentinel_;
	}

	void AddCallee(const std::shared_ptr<CallGraphNode> &callee, 
	               const std::shared_ptr<CallGraphNode> &caller);

	// Walk through all nodes calling Func against each one.
	template<typename Func>
	inline void Traverse(Func f) const;

	// Output to a file.
	void Write(CallGraphWriter &writer) const;

private:
	std::shared_ptr<CallGraphNode> root_;
	std::shared_ptr<CallGraphNode> sentinel_;
};

class CallGraphNode : public std::enable_shared_from_this<CallGraphNode> {
public:
	CallGraphNode(const std::shared_ptr<FunctionInfo> &info, 
	              const std::shared_ptr<CallGraphNode> &caller = 0);

	inline const std::shared_ptr<FunctionInfo> &info() const {
		return info_;
	}

	inline std::shared_ptr<CallGraphNode> caller() const {
		return caller_;
	}

	inline const CallGraph::NodeSet &callees() const {
		return callees_;
	}

	void AddCallee(const std::shared_ptr<FunctionInfo> &info);
	void AddCallee(const std::shared_ptr<CallGraphNode> &node);

	// Recursive parent-to-child traversing.
	template<typename Func>
	inline void Traverse(Func f) const;

private:
	std::shared_ptr<FunctionInfo> info_;
	std::shared_ptr<CallGraphNode> caller_;

	CallGraph::NodeSet callees_;
};

template<typename Func>
inline void CallGraph::Traverse(Func f) const {
	sentinel_->Traverse(f);
}

template<typename Func>
inline void CallGraphNode::Traverse(Func f) const {
	f(shared_from_this());
	for (auto &c : callees_) {
		c->Traverse(f);
	}
}

} // namespace amx_profiler

#endif // !AMX_PROFILER_CALL_GRAPH_H
