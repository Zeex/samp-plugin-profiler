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

#ifndef AMXNAMEFINDER_H
#define AMXNAMEFINDER_H

#include <list>
#include <map>
#include <string>

#include "singleton.h"
#include "amx/amx.h"

class AmxNameFinder : public Singleton<AmxNameFinder> {
    friend class Singleton<AmxNameFinder>;
public:
    void AddSearchDir(const std::string &dir);
    void UpdateCache();

    std::string GetAmxName(AMX *amx) const;
    std::string GetAmxName(AMX_HEADER *amxhdr) const;

private:
    AmxNameFinder();

    std::list<std::string> searchDirs_;

    struct AmxFileData {
        AMX    amx;
        time_t lastModified;
    };

    mutable std::map<AMX*, std::string> cachedNames_;
    std::map<std::string, AmxFileData> scripts_;
};

#endif
