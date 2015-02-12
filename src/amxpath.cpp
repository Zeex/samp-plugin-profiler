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

#include <cstring>
#include "amxpath.h"
#include "fileutils.h"

AmxFile::AmxFile(std::string name)
 : amx_(new AMX),
   name_(name),
   mtime_(fileutils::GetModificationTime(name))
{
  if (aux_LoadProgram(amx_, const_cast<char*>(name.c_str()), 0)
      != AMX_ERR_NONE) {
    delete amx_;
    amx_ = 0;
  }
}

AmxFile::~AmxFile() {
  if (amx_ != 0) {
    aux_FreeProgram(amx_);
  }
}

AmxPathFinder::~AmxPathFinder() {
  for (FileCache::iterator iterator = file_cache_.begin();
       iterator != file_cache_.end(); ++iterator)
  {
    delete iterator->second;
  }
}

std::string AmxPathFinder::FindAmxPath(AMX *amx) const {
  std::string result;

  PathCache::const_iterator iterator = path_cache_.find(amx);
  if (iterator != path_cache_.end()) {
    result = iterator->second;
  } else {
    result = FindAmxPath(reinterpret_cast<AMX_HEADER*>(amx->base));
    if (!result.empty()) {
      path_cache_.insert(std::make_pair(amx, result));
    }
  }

  return result;
}

std::string AmxPathFinder::FindAmxPath(AMX_HEADER *amxhdr) const {
  std::string result;

  for (DirSet::const_iterator iterator = search_dirs_.begin();
       iterator != search_dirs_.end(); ++iterator)
  {
    const std::string &dir = *iterator;

    std::vector<std::string> files;
    fileutils::GetDirectoryFiles(dir, "*.amx", files);

    for (std::vector<std::string>::const_iterator dir_iterator = files.begin();
         dir_iterator != files.end(); ++dir_iterator)
    {
      std::string filename;
      filename.append(dir);
      filename.append(fileutils::kNativePathSepString);
      filename.append(*dir_iterator);

      FileCache::iterator cache_iterator = file_cache_.find(filename);
      if (cache_iterator == file_cache_.end() ||
        cache_iterator->second->mtime() < fileutils::GetModificationTime(filename))
      {
        if (cache_iterator != file_cache_.end()) {
          file_cache_.erase(cache_iterator);
        }

        AmxFile *amx_file = new AmxFile(filename);
        if (amx_file->is_loaded()) {
          file_cache_.insert(std::make_pair(filename, amx_file));
        }
      }
    }
  }

  for (FileCache::const_iterator iterator = file_cache_.begin();
       iterator != file_cache_.end(); ++iterator) 
  {
    void *amxhdr2 = iterator->second->amx()->base;
    if (std::memcmp(amxhdr, amxhdr2, sizeof(AMX_HEADER)) == 0) {
      result = iterator->first;
      break;
    }
  }

  return result;
}
