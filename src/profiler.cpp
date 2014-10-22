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
#include <cstdarg>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>
#include <amxprof/call_graph_writer_dot.h>
#include <amxprof/debug_info.h>
#include <amxprof/profiler.h>
#include <amxprof/statistics_writer_html.h>
#include <amxprof/statistics_writer_json.h>
#include <amxprof/statistics_writer_text.h>
#include "amxpath.h"
#include "logprintf.h"
#include "profiler.h"

ConfigReader Profiler::server_cfg_("server.cfg");

bool Profiler::profile_gamemode_ =
  server_cfg_.GetOptionDefault("profile_gamemode", false);
std::string Profiler::profile_filterscripts_ =
  server_cfg_.GetOptionDefault<std::string>("profile_filterscripts", "");
std::string Profiler::profile_format_ =
  server_cfg_.GetOptionDefault<std::string>("profile_format", "html");
bool Profiler::call_graph_ =
  server_cfg_.GetOptionDefault("call_graph", false);
std::string Profiler::call_graph_format_ =
  server_cfg_.GetOptionDefault<std::string>("call_graph_format", "dot");

#define logprintf Use_Printf_isntead_of_logprintf

namespace {

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

std::string GetAmxPath(AMX *amx) {
  static AmxPathFinder finder;
  finder.AddSearchDirectory("gamemodes");
  finder.AddSearchDirectory("filterscripts");
  return finder.FindAmxPath(amx);
}

void ToLower(std::string &s) {
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}

std::string ToUnixPath(const std::string &path) {
  std::string fsPath = path;
  std::replace(fsPath.begin(), fsPath.end(), '\\', '/');
  return fsPath;
}

bool IsGameMode(const std::string &amx_path) {
  return amx_path.find("gamemodes/") != std::string::npos;
}

bool IsFilterScript(const std::string &amx_path) {
  return amx_path.find("filterscripts/") != std::string::npos;
}

} // anonymous namespace

int Profiler::Debug() {
  if (profiler_ != 0) {
    try {
      return profiler_->DebugHook(prev_debug_);
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
  if (profiler_ != 0) {
    try {
      return profiler_->CallbackHook(index, result, params, prev_callback_);
    } catch (const std::exception &e) {
      PrintException(e);
    }
  }
  if (prev_callback_ != 0) {
    return prev_callback_(amx(), index, result, params);
  }
  return AMX_ERR_NONE;
}

int Profiler::Exec(cell *retval, int index) {
  if (profiler_ != 0) {
    try {
      return profiler_->ExecHook(retval, index, amx_Exec);
    } catch (const std::exception &e) {
      PrintException(e);
    }
  }
  return amx_Exec(amx(), retval, index);
}

Profiler::Profiler(AMX *amx)
  : AMXService<Profiler>(amx),
    prev_debug_(amx->debug),
    prev_callback_(amx->callback),
    profiler_(0),
    debug_info_(0)
{
  try {
    std::string amx_path = ToUnixPath(GetAmxPath(amx));
    if (amx_path.empty()) {
      Printf("Failed to find .amx file");
      return;
    }

    bool attach_profiler = false;
    if (IsGameMode(amx_path)) {
      attach_profiler = profile_gamemode_;
    } else if (IsFilterScript(amx_path)) {
      std::stringstream fs_stream(profile_filterscripts_);
      do {
        std::string fs_name;
        fs_stream >> fs_name;
        if (amx_path == "filterscripts/" + fs_name + ".amx" ||
            amx_path == "filterscripts/" + fs_name) {
          attach_profiler = true;
          break;
        }
      } while (!fs_stream.eof());
    }
    if (!attach_profiler) {
      return;
    }

    if (amxprof::HasDebugInfo(amx)) {
      debug_info_ = new amxprof::DebugInfo(amx_path);
      if (!debug_info_->is_loaded()) {
        Printf("Error loading debug info: %s",
               aux_StrError(debug_info_->last_error()));
        delete debug_info_;
        debug_info_ = 0;
      }
    }

    profiler_ = new amxprof::Profiler(amx, debug_info_);
    profiler_->set_call_graph_enabled(call_graph_);

    if (debug_info_ != 0) {
      Printf("Attached profiler to '%s'", amx_path.c_str());
    } else {
      Printf("Attached profiler to '%s' (no debug info)", amx_path.c_str());
    }
  }
  catch (const std::exception &e) {
    PrintException(e);
  }
}

Profiler::~Profiler() {
  try {
    if (profiler_ != 0) {
      std::string amx_path = GetAmxPath(amx());
      std::string amx_name = std::string(amx_path, 0,
                                         amx_path.find_last_of("."));

      ToLower(profile_format_);
      std::string profile_filename = amx_name + "-profile." +
                                     profile_format_;
      std::ofstream profile_stream(profile_filename.c_str());

      if (profile_stream.is_open()) {
        amxprof::StatisticsWriter *writer = 0;

        if (profile_format_ == "html") {
          writer = new amxprof::StatisticsWriterHtml;
        } else if (profile_format_ == "txt" ||
                   profile_format_ == "text") {
          writer = new amxprof::StatisticsWriterText;
        } else if (profile_format_ == "json") {
          writer = new amxprof::StatisticsWriterJson;
        } else {
          Printf("Unrecognized profile format '%s'", profile_format_.c_str());
        }

        if (writer != 0) {
          Printf("Writing stats to '%s'", profile_filename.c_str());
          writer->set_stream(&profile_stream);
          writer->set_script_name(amx_path);
          writer->set_print_date(true);
          writer->set_print_run_time(true);
          writer->Write(profiler_->stats());
          delete writer;
        }

        profile_stream.close();
      } else {
        Printf("Error opening file '%s'", profile_filename.c_str());
      }

      if (call_graph_) {
        ToLower(call_graph_format_);
        std::string call_graph_filename = amx_name + "-calls." +
                                          call_graph_format_;
        std::ofstream call_graph_stream(call_graph_filename.c_str());

        if (call_graph_stream.is_open()) {
          amxprof::CallGraphWriterDot *writer = 0;

          if (call_graph_format_ == "dot") {
            writer = new amxprof::CallGraphWriterDot;
          } else {
            Printf("Unrecognized call graph format '%s'",
                   call_graph_format_.c_str());
          }

          if (writer != 0) {
            Printf("Writing call graph to '%s'", call_graph_filename.c_str());
            writer->set_stream(&call_graph_stream);
            writer->set_script_name(amx_path);
            writer->set_root_node_name("SA-MP Server");
            writer->Write(profiler_->call_graph());
            delete writer;
          }

          call_graph_stream.close();
        } else {
          Printf("Error opening file '%s'", call_graph_filename.c_str());
        }
      }
    }

    delete profiler_;
    delete debug_info_;
  }
  catch (const std::exception &e) {
    PrintException(e);
  }
}
