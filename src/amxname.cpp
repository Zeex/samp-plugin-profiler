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
#include <exception>
#include <iterator>
#include <list>
#include <string>

#include "amxname.h"
#include "fileutils.h"

#include "amx/amx.h"
#include "amx/amxaux.h"

static std::map<std::string, AmxFile> scripts;
static std::map<AMX*, std::string> cachedNames;

AmxFile::AmxFile(const std::string &name)
	: name_(name)
	, modified_(fileutils::GetModificationTime(name))
	, amxPtr_(new AMX, FreeAmx)
{
	if (aux_LoadProgram(amxPtr_.get(), const_cast<char*>(name.c_str()), 0) != AMX_ERR_NONE) {
		amxPtr_.reset();
	}	
}

std::string GetAmxName(AMX_HEADER *amxhdr) {
	std::string result;

	std::list<std::string> searchDirs;
	searchDirs.push_back("gamemodes");
	searchDirs.push_back("filterscripts");

	std::vector<std::string> filenames;
	for (std::list<std::string>::const_iterator it = searchDirs.begin(); it != searchDirs.end(); ++it) {
		fileutils::GetFilesInDirectory(*it, "*.amx", filenames);
	}

	for (std::vector<std::string>::const_iterator it = filenames.begin(); it != filenames.end(); ++it) {
		std::string filename = *it;

		time_t modified = fileutils::GetModificationTime(filename);
		std::map<std::string, AmxFile>::iterator scriptsIter = scripts.find(filename);

		if (scriptsIter == scripts.end() || scriptsIter->second.GetModified() < modified) {
			if (scriptsIter != scripts.end()) {
				scripts.erase(scriptsIter);
			}
			AmxFile script(filename);
			if (script.IsLoaded()) {
				scripts.insert(std::make_pair(filename, script));
			}
		}
	}
  
	for (std::map<std::string, AmxFile>::const_iterator it = scripts.begin(); it != scripts.end(); ++it) {
		if (std::memcmp(amxhdr, reinterpret_cast<AMX_HEADER*>(it->second.GetAmx()->base), sizeof(AMX_HEADER)) == 0) {
			result = it->first;
			break;
		}
	}

	return result;
}

std::string GetAmxName(AMX *amx) {
	std::string result;

	std::map<AMX*, std::string>::const_iterator it = cachedNames.find(amx);
	if (it != cachedNames.end()) {
		result = it->second;
	} else {
		result = GetAmxName(reinterpret_cast<AMX_HEADER*>(amx->base));
		if (!result.empty()) {
			cachedNames.insert(std::make_pair(amx, result));
		}
	}

	return result;
}

void AmxFile::FreeAmx(AMX *amx) {
	aux_FreeProgram(amx);
	delete amx;
}
