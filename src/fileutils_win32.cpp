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
#include <windows.h>
#include "fileutils.h"

namespace fileutils {

namespace {

HANDLE OpenFile(const std::string &path) {
  return CreateFile(path.c_str(),
                    GENERIC_READ,
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);
}

} // namespace

const char kNativePathSepChar = '\\';
const char *kNativePathSepString = "\\";

const char kNativePathListSepChar = ';';
const char *kNativePathListSepString = ";";

void GetDirectoryFiles(const std::string &directory,
                       const std::string &pattern,
                       std::vector<std::string> &files)
{
  std::string fileName;
  fileName.append(directory);
  fileName.append(kNativePathSepString);
  fileName.append(pattern);

  WIN32_FIND_DATA findFileData;

  HANDLE hFindFile = FindFirstFile(fileName.c_str(), &findFileData);
  if (hFindFile == INVALID_HANDLE_VALUE) {
    return;
  }

  do {
    if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        files.push_back(findFileData.cFileName);
    }
  } while (FindNextFile(hFindFile, &findFileData) != 0);

  FindClose(hFindFile);
}

bool SameFile(const std::string &path1, const std::string &path2) {
  // Note: both files must stay open simultaneously, otherwise CreateFile()
  // may return the same file ID for both files.

  HANDLE file1_handle = OpenFile(path1);
  if (file1_handle == INVALID_HANDLE_VALUE) {
    return false;
  }

  BY_HANDLE_FILE_INFORMATION file1_info;
  if (!GetFileInformationByHandle(file1_handle, &file1_info)) {
    return false;
  }

  HANDLE file2_handle = OpenFile(path2);
  if (file2_handle == INVALID_HANDLE_VALUE) {
    CloseHandle(file1_handle);
    return false;
  }

  BY_HANDLE_FILE_INFORMATION file2_info;
  if (!GetFileInformationByHandle(file2_handle, &file2_info)) {
    CloseHandle(file1_handle);
    CloseHandle(file2_handle);
    return false;
  }

  bool same_file =
    file1_info.dwVolumeSerialNumber == file2_info.dwVolumeSerialNumber
    && file1_info.nFileIndexLow == file2_info.nFileIndexLow
    && file1_info.nFileIndexHigh == file2_info.nFileIndexHigh;

  CloseHandle(file1_handle);
  CloseHandle(file2_handle);

  return same_file;
}

} // namespace fileutils
