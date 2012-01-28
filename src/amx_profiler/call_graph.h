// AMX profiler for SA-MP server: http://sa-mp.com
//
// Copyright (C) 2011-2012 Sergey Zolotarev
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
	for (auto iterator = callees_.begin(); iterator != callees_.end(); ++iterator) {
		(*iterator)->Traverse(f);
	}
}

} // namespace amx_profiler

#endif // !AMX_PROFILER_CALL_GRAPH_H
