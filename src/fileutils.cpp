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

#include "fileutils.h"

#include <time.h>
#include <sys/stat.h>

#if defined WIN32 || defined _WIN32
    #include <windows.h>
    #include <sys/types.h>
    #if !defined stat
        #define stat _stat
    #endif
#else
    #include <dirent.h>
    #include <fnmatch.h>
#endif

namespace fileutils {

std::string GetBaseName(const std::string &path) {
    std::string::size_type lastSep = path.find_last_of("/\\");
    if (lastSep != std::string::npos) {
        return path.substr(lastSep + 1);
    }
    return path;
}

std::string GetExtenstion(const std::string &path) {
    std::string ext;
    std::string::size_type period = path.rfind('.');
    if (period != std::string::npos) {
        ext = path.substr(period + 1);;
    } 
    return ext;
}

time_t GetMoficationTime(const std::string &path) {
    struct stat attrib;
    stat(path.c_str(), &attrib);
	return attrib.st_mtime;
}

void GetFilesInDirectory(const std::string &dir, const std::string &pattern, std::vector<std::string> &result) {
#if defined WIN32 || defined _WIN32
    WIN32_FIND_DATA findFileData;
    HANDLE hFindFile = FindFirstFile((dir + "\\" + pattern).c_str(), &findFileData);
    if (hFindFile != INVALID_HANDLE_VALUE) {
        do {
            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                result.push_back(dir + "\\" + findFileData.cFileName);
            }
        } while (FindNextFile(hFindFile, &findFileData) != 0);
        FindClose(hFindFile);
    }
#else
    DIR *dp;
    if ((dp = opendir(dir.c_str())) != 0) {
        struct dirent *dirp;
        while ((dirp = readdir(dp)) != 0) {
            if (!fnmatch(pattern.c_str(), dirp->d_name,
                            FNM_CASEFOLD | FNM_NOESCAPE | FNM_PERIOD)) {
                result.push_back(dir + "/" + dirp->d_name);
            }
        }
        closedir(dp);
    }
#endif
}

} // namespace fileutils