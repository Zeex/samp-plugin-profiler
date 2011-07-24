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

#include <cstring>
#include <iterator>
#include <list>
#include <string>

#include "amxnamefinder.h"
#include "fileutils.h"

#include "amx/amx.h"
#include "amx/amxaux.h"

AmxNameFinder::AmxNameFinder() {}

void AmxNameFinder::AddSearchDir(const std::string &dir) {
    searchDirs_.push_back(dir);
}

void AmxNameFinder::UpdateCache() {
    std::vector<std::string> filenames;
    for (std::list<std::string>::iterator it = searchDirs_.begin(); it != searchDirs_.end(); ++it) {
        fileutils::GetFilesInDirectory(*it, "*.amx", filenames);
    }
    for (std::vector<std::string>::iterator it = filenames.begin(); it != filenames.end(); ++it) {
        time_t modified = fileutils::GetMoficationTime(*it);
        std::map<std::string, AmxFileData>::iterator scriptIter = scripts_.find(*it);
        if (scriptIter == scripts_.end() || scriptIter->second.lastModified < modified) {
            AMX amx;
            if (aux_LoadProgram(&amx, const_cast<char*>(it->c_str()), 0) == AMX_ERR_NONE) {
                AmxFileData script;
                script.amx = amx;
                script.lastModified = modified;
                scripts_.insert(std::make_pair(*it, script));
            }
        }
    }
}

std::string AmxNameFinder::GetAmxName(AMX_HEADER *amxhdr) const {
    std::string result;
  
    for (std::map<std::string, AmxFileData>::const_iterator it = scripts_.begin(); 
         it != scripts_.end(); ++it) 
    {
        if (std::memcmp(amxhdr, reinterpret_cast<AMX_HEADER*>(it->second.amx.base), 
            sizeof(AMX_HEADER)) == 0) 
        {
            result = it->first;
            break;
        }
    }

    return result;
}

std::string AmxNameFinder::GetAmxName(AMX *amx) const {
    std::string result = cachedNames_[amx];
    if (result.empty()) {
        result = GetAmxName(reinterpret_cast<AMX_HEADER*>(amx->base));
        if (!result.empty()) {
            cachedNames_[amx] = result;
        }
    } 
    return result;
}
