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

#include <iosfwd>
#include <list>
#include <memory>
#include "call_stack.h"
#include "function.h"

namespace amx_profiler {

class CallGraphNode : public std::enable_shared_from_this<CallGraphNode> {
public:
	CallGraphNode(const std::shared_ptr<Function> &fn, 
	              const std::shared_ptr<CallGraphNode> &caller = 0);

	inline const std::shared_ptr<Function> &function() const {
		return fn_;
	}

	inline std::shared_ptr<CallGraphNode> caller() const {
		return caller_;
	}

	inline const std::list<std::shared_ptr<CallGraphNode>> &callees() const {
		return callees_;
	}

	void AddCallee(const std::shared_ptr<Function> &fn);
	void AddCallee(const std::shared_ptr<CallGraphNode> &node);

	void Write(std::ostream &stream) const;

private:
	std::shared_ptr<Function> fn_;
	std::shared_ptr<CallGraphNode> caller_;
	std::list<std::shared_ptr<CallGraphNode>> callees_;
};

class CallGraph {
public:
	CallGraph(const std::shared_ptr<CallGraphNode> &root = 0);

	inline std::shared_ptr<CallGraphNode> root() const {
		return root_;
	}

	inline void set_root(const std::shared_ptr<CallGraphNode> &root) {
		root_ = root;
	}

	std::weak_ptr<CallGraphNode> sentinel() const {
		return sentinel_;
	}

	// Write the whole graph to "stream" in GraphViz format:
	// http://www.graphviz.org/content/profile
	void Write(std::ostream &stream) const;

private:
	std::shared_ptr<CallGraphNode> root_;
	std::shared_ptr<CallGraphNode> sentinel_;
};

} // namespace amx_profiler

#endif // !AMX_PROFILER_CALL_GRAPH_H
