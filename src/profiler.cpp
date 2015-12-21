// Copyright (c) 2011-2014 Zeex
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

#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>
#include <amxprof/call_graph_writer_dot.h>
#include <amxprof/statistics_writer_html.h>
#include <amxprof/statistics_writer_json.h>
#include <amxprof/statistics_writer_text.h>
#include "amxpath.h"
#include "fileutils.h"
#include "logprintf.h"
#include "profiler.h"

#define logprintf Use_Printf_isntead_of_logprintf

namespace {
namespace cfg {

ConfigReader server_cfg("server.cfg");

std::string gamemodes =
    server_cfg.GetOptionDefault<std::string>("profiler_gamemodes", "");
std::string filterscripts =
    server_cfg.GetOptionDefault<std::string>("profiler_filterscripts", "");
std::string output_format =
    server_cfg.GetOptionDefault<std::string>("profiler_outputformat", "html");
bool call_graph =
    server_cfg.GetOptionDefault("profiler_callgraph", false);
std::string call_graph_format =
    server_cfg.GetOptionDefault<std::string>("profiler_callgraphformat", "dot");

namespace old {

// old config variables.
bool profile_gamemode =
    server_cfg.GetOptionDefault("profile_gamemode", false);
std::string profile_filterscripts =
    server_cfg.GetOptionDefault<std::string>("profile_filterscripts", "");
std::string profile_format =
    server_cfg.GetOptionDefault<std::string>("profile_format", "html");
bool call_graph =
    server_cfg.GetOptionDefault("call_graph", false);
std::string call_graph_format =
    server_cfg.GetOptionDefault<std::string>("call_graph_format", "dot");

} // namespace old
} // namespace cfg

void Printf(const char *format, ...) {
  std::va_list va;
  va_start(va, format);

  std::string new_format;
  new_format.append("[profiler] ");
  new_format.append(format);

  vlogprintf(new_format.c_str(), va);
  va_end(va);
}

void PrintException(const std::exception &e) {
  Printf("Error: %s", e.what());
}

void SplitString(const std::string &s,
                 char delim,
                 std::vector<std::string> &comps) {
  std::string::size_type begin = 0;
  std::string::size_type end;

  while (begin < s.length()) {
    end = s.find(delim, begin);
    end = (end == std::string::npos) ? s.length() : end;
    comps.push_back(std::string(s.begin() + begin, s.begin() + end));
    begin = end + 1;
  }
}

void ToLower(std::string &s) {
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}

std::string ToUnixPath(std::string path) {
  std::replace(path.begin(), path.end(), '\\', '/');
  return path;
}

bool IsCallGraphEnabled() {
  return cfg::call_graph || cfg::old::call_graph;
}

bool IsGameMode(const std::string &amx_path) {
  return amx_path.find("gamemodes/") != std::string::npos;
}

bool IsFilterScript(const std::string &amx_path) {
  return amx_path.find("filterscripts/") != std::string::npos;
}

bool ShouldBeProfiled(const std::string amx_path) {
  if (IsGameMode(amx_path)) {
    if (cfg::old::profile_gamemode) {
      return true;
    }
    std::vector<std::string> gm_names;
    SplitString(cfg::gamemodes, ' ', gm_names);
    for (std::vector<std::string>::const_iterator iterator = gm_names.begin();
         iterator != gm_names.end(); ++iterator) {
      const std::string &gm_name = *iterator;
      if (amx_path == "gamemodes/" + gm_name + ".amx" ||
          amx_path == "gamemodes/" + gm_name) {
        return true;
      }
    }
  }
  if (IsFilterScript(amx_path)) {
    std::vector<std::string> fs_names;
    SplitString(cfg::old::profile_filterscripts, ' ', fs_names);
    SplitString(cfg::filterscripts, ' ', fs_names);
    for (std::vector<std::string>::const_iterator iterator = fs_names.begin();
         iterator != fs_names.end(); ++iterator) {
      const std::string &fs_name = *iterator;
      if (amx_path == "filterscripts/" + fs_name + ".amx" ||
          amx_path == "filterscripts/" + fs_name) {
        return true;
      }
    }
  }
  return false;
}

} // anonymous namespace

Profiler::Profiler(AMX *amx)
 : AMXService<Profiler>(amx),
   prev_debug_(amx->debug),
   prev_callback_(amx->callback),
   profiler_(amx, IsCallGraphEnabled()),
   state_(PROFILER_DISABLED)
{
  AmxPathFinder amx_path_finder;
  amx_path_finder.AddSearchDirectory("gamemodes");
  amx_path_finder.AddSearchDirectory("filterscripts");

  const char *amx_search_path = getenv("AMX_PATH");
  if (amx_search_path != 0) {
    std::vector<std::string> dirs;
    SplitString(amx_search_path, fileutils::kNativePathListSepChar, dirs);
    for (std::vector<std::string>::const_iterator iterator = dirs.begin();
         iterator != dirs.end(); ++iterator) {
      amx_path_finder.AddSearchDirectory(*iterator);
    }
  }

  amx_path_ = ToUnixPath(amx_path_finder.FindAmxPath(amx));
  amx_name_ = fileutils::GetDirectory(amx_path_)
            + "/"
            + fileutils::GetBaseName(amx_path_);

  if (amx_path_.empty()) {
    Printf("Could not find AMX file (try setting AMX_PATH?)");
  }
}

int Profiler::Load() {
  if (ShouldBeProfiled(amx_path_)) {
    Attach();
  }
  return AMX_ERR_NONE;
}

int Profiler::Unload() {
  return AMX_ERR_NONE;
}

int Profiler::Debug() {
  if (state_ == PROFILER_STARTED) {
    try {
      return profiler_.DebugHook(prev_debug_);
    } catch (const std::exception &e) {
      PrintException(e);
    }
  }
  if (prev_debug_ != 0) {
    return prev_debug_(amx());
  }
  return AMX_ERR_NONE;
}

int Profiler::Callback(cell index, cell *result, cell *params) {
  if (state_ == PROFILER_STARTED) {
    try {
      return profiler_.CallbackHook(index, result, params, prev_callback_);
    } catch (const std::exception &e) {
      PrintException(e);
    }
  }
  return prev_callback_(amx(), index, result, params);
}

int Profiler::Exec(cell *retval, int index) {
  if (profiler_.call_stack()->is_empty()) {
    switch (state_) {
      case PROFILER_ATTACHING:
        if (!Attach()) {
          break;
        }
        // fallthrough
      case PROFILER_STARTING:
        state_ = PROFILER_STARTED;
        break;
    }
  }
  if (state_ == PROFILER_STARTED) {
    try {
      int error = profiler_.ExecHook(retval, index, amx_Exec);
      if (state_ == PROFILER_STOPPING
          && profiler_.call_stack()->is_empty()) {
        state_ = PROFILER_STOPPED;
      }
      return error;
    } catch (const std::exception &e) {
      PrintException(e);
    }
  }
  return amx_Exec(amx(), retval, index);
}

ProfilerState Profiler::GetState() const {
  return state_;
}

bool Profiler::Attach() {
  try {
    if (amx_path_.empty()) {
      return false;
    }
    if (state_ >= PROFILER_ATTACHED) {
      return false;
    }

    if (amxprof::HasDebugInfo(amx())) {
      if (debug_info_.Load(amx_path_)) {
        profiler_.set_debug_info(&debug_info_);
      } else {
        Printf("Error loading debug info: %s",
                aux_StrError(debug_info_.last_error()));
      }
    }

    if (debug_info_.is_loaded()) {
      Printf("Attached profiler to %s", amx_path_.c_str());
    } else {
      Printf("Attached profiler to %s (no debug info)", amx_path_.c_str());
    }

    state_ = PROFILER_ATTACHED;
    return true;
  }
  catch (const std::exception &e) {
    PrintException(e);
  }
  return false;
}

bool Profiler::Start() {
  if (state_ < PROFILER_ATTACHED) {
    state_ = PROFILER_ATTACHING;
    return true;
  }
  if (state_ >= PROFILER_ATTACHED) {
    state_ = PROFILER_STARTING;
    return true;
  }
  return false;
}

bool Profiler::Stop() {
  if (state_ >= PROFILER_STARTED) {
    state_ = PROFILER_STOPPING;
    return true;
  }
  return false;
}

bool Profiler::Dump() const {
  try {
    if (state_ < PROFILER_ATTACHED) {
      return false;
    }

    std::string output_format = cfg::output_format;
    if (output_format.empty()) {
      output_format = cfg::old::profile_format;
    }
    ToLower(output_format);
    std::string profile_filename =
        amx_name_ + "-profile." + output_format;
    std::ofstream profile_stream(profile_filename.c_str());

    if (profile_stream.is_open()) {
      amxprof::StatisticsWriter *writer = 0;

      if (output_format == "html") {
        writer = new amxprof::StatisticsWriterHtml;
      } else if (output_format == "txt" ||
                  output_format == "text") {
        writer = new amxprof::StatisticsWriterText;
      } else if (output_format == "json") {
        writer = new amxprof::StatisticsWriterJson;
      } else {
        Printf("Unsupported output format '%s'", output_format.c_str());
      }

      if (writer != 0) {
        Printf("Writing profile to %s", profile_filename.c_str());
        writer->set_stream(&profile_stream);
        writer->set_script_name(amx_path_);
        writer->set_print_date(true);
        writer->set_print_run_time(true);
        writer->Write(profiler_.stats());
        delete writer;
      }

      profile_stream.close();
    } else {
      Printf("Error opening '%s' for writing", profile_filename.c_str());
    }

    if (IsCallGraphEnabled()) {
      std::string call_graph_format = cfg::call_graph_format;
      if (call_graph_format.empty()) {
        call_graph_format = cfg::old::call_graph_format;
      }
      ToLower(call_graph_format);
      std::string call_graph_filename =
          amx_name_ + "-calls." + call_graph_format;
      std::ofstream call_graph_stream(call_graph_filename.c_str());

      if (call_graph_stream.is_open()) {
        amxprof::CallGraphWriterDot *writer = 0;

        if (call_graph_format == "dot") {
          writer = new amxprof::CallGraphWriterDot;
        } else {
          Printf("Unsupported call graph format '%s'",
                  call_graph_format.c_str());
        }

        if (writer != 0) {
          Printf("Writing call graph to %s", call_graph_filename.c_str());
          writer->set_stream(&call_graph_stream);
          writer->set_script_name(amx_path_);
          writer->set_root_node_name("SA-MP Server");
          writer->Write(profiler_.call_graph());
          delete writer;
        }

        call_graph_stream.close();
      } else {
        Printf("Error opening %s for writing", call_graph_filename.c_str());
      }
    }
    return true;
  }
  catch (const std::exception &e) {
    PrintException(e);
  }
  return false;
}
