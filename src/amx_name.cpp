// Copyright (C) 2011-2012 Sergey Zolotarev
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
#include <map>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <amx/amx.h>
#include <amxaux.h>
#include "amx_name.h"

class AmxFile {
public:
	static void FreeAmx(AMX *amx);

	explicit AmxFile(const std::string &name);

	bool IsLoaded() const { return amxPtr_.get() != 0; }

	const AMX *GetAmx() const { return amxPtr_.get(); }
	const std::string &GetName() const { return name_; }
	std::time_t GetLastWriteTime() const { return last_write_; }

private:
	boost::shared_ptr<AMX> amxPtr_;
	std::string name_;
	std::time_t last_write_;
};

AmxFile::AmxFile(const std::string &name)
	: name_(name)
	, last_write_(boost::filesystem::last_write_time(name))
	, amxPtr_(new AMX, FreeAmx)
{
	if (aux_LoadProgram(amxPtr_.get(), const_cast<char*>(name.c_str()), 0) != AMX_ERR_NONE) {
		amxPtr_.reset();
	}
}

void AmxFile::FreeAmx(AMX *amx) {
	aux_FreeProgram(amx);
	delete amx;
}

static std::map<std::string, AmxFile> scripts;
static std::map<AMX*, std::string> cachedNames;

std::string GetAmxName(AMX_HEADER *amxhdr) {
	std::string result;

	std::list<boost::filesystem::path> dirs;
	dirs.push_back(boost::filesystem::path("gamemodes/"));
	dirs.push_back(boost::filesystem::path("filterscripts/"));

	for (std::list<boost::filesystem::path>::const_iterator d = dirs.begin();
			d != dirs.end(); ++d) {
		if (!boost::filesystem::exists(*d) || !boost::filesystem::is_directory(*d)) {
			continue;
		}
		for (boost::filesystem::directory_iterator f(*d);
				f != boost::filesystem::directory_iterator(); ++f) {
			if (!boost::filesystem::is_regular_file(f->path()) ||
					!boost::algorithm::ends_with(f->path().string(), ".amx")) {
				continue;
			}

			std::string filename = f->path().string();
			std::time_t last_write = boost::filesystem::last_write_time(filename);

			std::map<std::string, AmxFile>::iterator script_it = scripts.find(filename);

			if (script_it == scripts.end() ||
					script_it->second.GetLastWriteTime() < last_write) {
				if (script_it != scripts.end()) {
					scripts.erase(script_it);
				}
				AmxFile script(filename);
				if (script.IsLoaded()) {
					scripts.insert(std::make_pair(filename, script));
				}
			}
		}
	}

	for (std::map<std::string, AmxFile>::const_iterator script_it = scripts.begin();
			script_it != scripts.end(); ++script_it)
	{
		void *amxhdr2 = script_it->second.GetAmx()->base;
		if (std::memcmp(amxhdr, amxhdr2, sizeof(AMX_HEADER)) == 0) {
			result = script_it->first;
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
