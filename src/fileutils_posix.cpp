// Copyright (c) 2011-2018 Zeex
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

#include <string>
#include <vector>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>
#include "fileutils.h"

namespace fileutils {

const char kNativePathSepChar = '/';
const char *kNativePathSepString = "/";

const char kNativePathListSepChar = ':';
const char *kNativePathListSepString = ":";

void GetDirectoryFiles(const std::string &directory, const std::string &pattern,
                       std::vector<std::string> &files)
{
  DIR *dp;
  if ((dp = opendir(directory.c_str())) != 0) {
    struct dirent *dirp;
    while ((dirp = readdir(dp)) != 0) {
      if (!fnmatch(pattern.c_str(),
                   dirp->d_name,
                   FNM_CASEFOLD | FNM_NOESCAPE | FNM_PERIOD)) {
        files.push_back(dirp->d_name);
      }
    }
    closedir(dp);
  }
}

bool SameFile(const std::string &path1, const std::string &path2) {
  struct stat stat1, stat2;
  if (stat(path1.c_str(), &stat1) < 0 ||
      stat(path2.c_str(), &stat2) < 0) {
    return false;
  }
  return stat1.st_dev == stat2.st_dev && stat1.st_ino == stat2.st_ino;
}

} // namespace fileutils
