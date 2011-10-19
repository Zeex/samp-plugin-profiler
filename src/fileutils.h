// Copyright (c) 2011 Zeex
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>
#include <vector>

#include <ctime>

namespace fileutils {

std::string GetBaseName(const std::string &path);
std::string GetExtenstion(const std::string &path);

std::time_t GetModificationTime(const std::string &path);

void GetFilesInDirectory(const std::string &dir, const std::string &pattern, std::vector<std::string> &result);

} // namespace fileutils

#endif // FILEUTILS_H
