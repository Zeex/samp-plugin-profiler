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

#ifndef AMXPATH_H
#define AMXPATH_H

#include <ctime>
#include <map>
#include <set>
#include <string>
#include <amx/amx.h>
#include <amx/amxaux.h>

class AmxFile {
 public:
  explicit AmxFile(std::string name);
  ~AmxFile();

  AMX *amx() { return amx_; }
  const AMX *amx() const { return amx_; }

  bool is_loaded() const { return amx_ != 0; }

  std::string name() const { return name_; }
  std::time_t mtime() const { return mtime_; }

 private:
  AmxFile(const AmxFile &);
  void operator=(const AmxFile &);

 private:
  AMX *amx_;
  std::string name_;
  std::time_t mtime_;
};

class AmxPathFinder {
 public:
  ~AmxPathFinder();

  void AddSearchDirectory(std::string path) {
    search_dirs_.insert(path);
  }

  std::string FindAmxPath(AMX *amx) const;
  std::string FindAmxPath(AMX_HEADER *amxhdr) const;

 private:
  typedef std::set<std::string> DirSet;
  DirSet search_dirs_;

  typedef std::map<std::string, AmxFile*> FileCache;
  mutable FileCache file_cache_;

  typedef std::map<AMX*, std::string> PathCache;
  mutable PathCache path_cache_;
};

#endif // !AMXPATH_H
