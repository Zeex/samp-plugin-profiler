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

#ifndef AMX_PROFILER_CALL_GRAPH_WRITER_H
#define AMX_PROFILER_CALL_GRAPH_WRITER_H

#include <algorithm>
#include <iostream>
#include <tuple>
#include "call_graph.h"
#include "call_graph_writer_gv.h"
#include "function.h"
#include "function_info.h"
#include "time_interval.h"

namespace amx_profiler {

CallGraphWriterGV::CallGraphWriterGV(std::ostream *stream, const std::string &name, const std::string top_node_name) 
	: stream_(stream)
	, name_(name)
	, top_name_(top_node_name)
{
}

void CallGraphWriterGV::Write(const CallGraph &graph) {
	*stream_ << 
	"digraph \"Call graph of " << name_ << "\" {\n"
	"	size=\"6,4\"; ratio = fill;\n"
	"	node [style=filled];\n"
	;

	// Write basic graph (nodes + arrows).
	graph.Traverse([this](const std::shared_ptr<const CallGraphNode> &node) {
		if (!node->callees().empty()) {
			std::string caller_name;
			if (node->info()) {
				caller_name = node->info()->function()->name();
			} else {
				caller_name = top_name_;
			}
			std::for_each(node->callees().begin(), node->callees().end(), [&](const std::shared_ptr<CallGraphNode> &c) {
				std::string color;
				if (c->info()->function()->type() == "public") {
					color = "#001EE0";
				} else if (c->info()->function()->type() == "native") {
					color = "#9900E0";
				} else {
					color = "#000000";
				}
				*stream_ << "\t\"" << caller_name << "\" -> \"" << c->info()->function()->name() 
					<< "\" [color=\"" << color << "\"];" << std::endl;
			});
		}	
	});

	// Get maximum execution time.
	TimeInterval max_time = 0;
	graph.Traverse([&max_time, &graph](const std::shared_ptr<const CallGraphNode> &node) {
		if (node != graph.sentinel()) {
			auto time = node->info()->GetSelfTime();
			if (time > max_time) {
				max_time = time;
			}
		}
	});

	// Color nodes depending to draw attention to hot spots.
	graph.Traverse([&max_time, this, &graph](const std::shared_ptr<const CallGraphNode> &node) {
		if (node != graph.sentinel()) {
			auto time = node->info()->GetSelfTime();
			auto ratio = static_cast<double>(time) / static_cast<double>(max_time);
			// We encode color in hue-saturation-brightness.
			auto hsb = std::make_tuple((1.0 - ratio) * 0.65, (ratio * 0.8) + 0.2, 1.0);
			*stream_ << "\t\"" << node->info()->function()->name() << "\" [color=\""
				<< std::get<0>(hsb) << ", "
				<< std::get<1>(hsb) << ", "
				<< std::get<2>(hsb)
			<< "\"];" << std::endl;
		}
	});

	*stream_ <<
	"}\n";
}

} // namespace amx_profiler

#endif // !AMX_PROFILER_CALL_GRAPH_WRITER_H


