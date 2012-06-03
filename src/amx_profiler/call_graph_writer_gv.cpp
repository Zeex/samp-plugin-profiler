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
	"	size=\"10,8\"; ratio=fill; rankdir=LR\n"
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
				*stream_ << "\t\"" << caller_name << "\" -> \"" << c->info()->function()->name() 
					<< "\" [color=\"";
				// Arrow color is associated with callee type.
				std::string fn_type = c->info()->function()->type();
				if (fn_type == "public") {
					*stream_ << "#4B4E99";
				} else if (fn_type == "native") {
					*stream_ << "#7C4B99";
				} else {
					*stream_ << "#777777";
				}
				*stream_ << "\"];\n";
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
			// We encode color in HSB.
			auto hsb = std::make_tuple(
				(1.0 - ratio) * 0.6, // hue
				(ratio * 0.9) + 0.1, // saturation
				1.0                  // brightness
			);
			*stream_ << "\t\"" << node->info()->function()->name() << "\" [color=\""
				<< std::get<0>(hsb) << ", "
				<< std::get<1>(hsb) << ", "
				<< std::get<2>(hsb) << "\""
			// Choose different shape depending on funciton type.
			<< ", shape=";
			std::string fn_type = node->info()->function()->type();
			if (fn_type == "public") {
				*stream_ << "octagon";
			} else if (fn_type == "native") {
				*stream_ << "box";
			} else {
				*stream_ << "oval";
			}
			*stream_ << "];\n";
		} else {
			*stream_ << "\t\"" << top_name_ << "\" [shape=diamond];\n";
		}
	});

	*stream_ <<
	"}\n";
}

} // namespace amx_profiler

#endif // !AMX_PROFILER_CALL_GRAPH_WRITER_H


