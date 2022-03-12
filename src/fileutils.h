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

#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <ctime>
#include <string>
#include <vector>

namespace fileutils {

extern const char kNativePathSepChar;
extern const char *kNativePathSepString;

extern const char kNativePathListSepChar;
extern const char *kNativePathListSepString;

std::string GetDirectory(const std::string &path);
std::string GetFileName(const std::string &path);
std::string GetBaseName(const std::string &path);
std::string GetFileExtension(const std::string &path);
const char *GetFileExtensionPtr(const char *path);

std::time_t GetModificationTime(const std::string &path);

void GetDirectoryFiles(const std::string &directory,
                       const std::string &pattern,
                       std::vector<std::string> &files);

bool SameFile(const std::string &path1, const std::string &path2);

std::string ToUnixPath(std::string path);

} // namespace fileutils

#endif // !FILEUTILS_H
