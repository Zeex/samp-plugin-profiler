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

#ifndef AMXPROF_CALL_GRAPH_WRITER_H
#define AMXPROF_CALL_GRAPH_WRITER_H

#include <iostream>
#include <string>
#include "call_graph.h"
#include "call_graph_writer_dot.h"
#include "function.h"
#include "function_statistics.h"

namespace amxprof {

void CallGraphWriterDot::Write(const CallGraph *graph) {
  *stream() << 
    "digraph \"Call graph of '" << script_name() << "'\" {\n"
    "  size=\"10,8\"; ratio=fill; rankdir=LR\n"
    "  node [style=filled];\n"
    ;

  WriteNode write_node(this);
  graph->Traverse(&write_node);
  
  ComputeMaxTime compute_max_time(this);
  graph->Traverse(&compute_max_time);

  WriteNodeColor write_node_color(this, compute_max_time.max_time());
  graph->Traverse(&write_node_color);

  *stream() << "}\n";
}

void CallGraphWriterDot::WriteNode::Visit(const CallGraphNode *node) {
  if (node->callees().empty()) {
    return;
  }

  std::string caller_name;
  if (node->stats() != 0) {
    caller_name = node->stats()->function()->name();
  } else {
    caller_name = writer_->root_node_name();
  }

  std::ostream *stream = writer_->stream();
  std::set<CallGraphNode*>::const_iterator iterator =
    node->callees().begin();

  for (; iterator != node->callees().end(); ++iterator) {
    const CallGraphNode *callee = *iterator;

    *stream << "  \"" << caller_name << "\" -> \""
            << callee->stats()->function()->name() << "\" [color=\"";

    Function::Type fn_type = callee->stats()->function()->type();
    switch (fn_type) {
      case Function::NORMAL:
        *stream << "#777777";
        break;
      case Function::PUBLIC:
        *stream << "#4B4E99";
        break;
      case Function::NATIVE:
        *stream << "#7C4B99";
        break;
    }

    *stream << "\"];\n";
  }
}

void CallGraphWriterDot::WriteNodeColor::Visit(const CallGraphNode *node) {
  std::ostream *stream = writer_->stream();

  if (node == node->graph()->sentinel()) {
    *stream << "  \"" << writer_->root_node_name() << "\" [shape=diamond];\n";
    return;
  }

  Nanoseconds time = node->stats()->self_time();
  double ratio = static_cast<double>(time.count()) /
                 static_cast<double>(max_time_.count());

  // We encode color in HSB.
  struct {
    double h; // hue
    double s; // saturation
    double b; // brightness
  } hsb = {
    (1.0 - ratio) * 0.6,
    (ratio * 0.9) + 0.1,
    1.0
  };

  *stream << "  \"" << node->stats()->function()->name() << "\" [color=\""
          << hsb.h << ", "
          << hsb.s << ", "
          << hsb.b << "\""
          << ", shape=";

  Function::Type fn_type = node->stats()->function()->type();
  switch (fn_type) {
    case Function::PUBLIC:
      *stream << "octagon";
      break;
    case Function::NATIVE:
      *stream << "box";
      break;
    case Function::NORMAL:
      *stream << "oval";
      break;
  }

  *stream << "];\n";
}

void CallGraphWriterDot::ComputeMaxTime::Visit(const CallGraphNode *node) {
  if (node == node->graph()->sentinel()) {
    return;
  }

  Nanoseconds time = node->stats()->self_time();
  if (time > max_time_) {
    max_time_ = time;
  }
}

} // namespace amxprof

#endif // !AMXPROF_CALL_GRAPH_WRITER_H


