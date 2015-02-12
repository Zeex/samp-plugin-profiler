// Copyright (c) 2011-2015 Zeex
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

#ifndef AMXPROF_STATISTICS_WRITER_H
#define AMXPROF_STATISTICS_WRITER_H

#include <iosfwd>
#include <string>

namespace amxprof {

class Statistics;

class StatisticsWriter {
 public:
  StatisticsWriter();
  virtual ~StatisticsWriter();

  virtual void Write(const Statistics *stats) = 0;

  std::ostream *stream() const { return stream_; }
  void set_stream(std::ostream *stream) { stream_ = stream; }
  
  std::string script_name() const { return script_name_; }
  void set_script_name(std::string script_name) { script_name_ = script_name; }
  
  bool print_date() const { return print_date_; }
  void set_print_date(bool print_date) { print_date_ = print_date; }

  bool print_run_time() const { return print_run_time_; }
  void set_print_run_time(bool print_run_time) { print_run_time_ = print_run_time; }

 private:
  std::ostream *stream_;
  std::string script_name_;
  bool print_date_;
  bool print_run_time_;
};

} // namespace amxprof

#endif // !AMXPROF_PROFILE_WRITER_H
