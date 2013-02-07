// Copyright (c) 2011-2013, Zeex
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
#include <ctime>
#include <exception>
#include <iterator>
#include <memory>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>
#include <amx/amx.h>
#include <amx/amxaux.h>
#ifdef _WIN32
	#include <windows.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#if !defined stat
		#define stat _stat
	#endif
#else
	#include <dirent.h>
	#include <fnmatch.h>
	#include <sys/stat.h>
#endif
#include "amxpath.h"

static inline std::time_t GetFileModificationTime(const std::string filename) {
	struct stat attrib;
	stat(filename.c_str(), &attrib);
	return attrib.st_mtime;
}

class AmxFile {
public:
	explicit AmxFile(std::string name);
	~AmxFile();

	AMX *amx() {
		return const_cast<AMX*>(const_cast<const AmxFile*>(this)->amx());
	}
	const AMX *amx() const { return amx_; }

	bool is_loaded() const { return amx_ != nullptr; }
	std::string name() const { return name_; }
	std::time_t modification_time() const { return modification_time_; }

private:
	AmxFile(const AmxFile &);
	void operator=(const AmxFile &);

private:
	AMX *amx_;
	std::string name_;
	std::time_t modification_time_;
};

AmxFile::AmxFile(std::string name)
	: name_(name)
	, modification_time_(GetFileModificationTime(name))
{
	if (AMX *amx = new AMX) {
		std::memset(amx, 0, sizeof(AMX));
		if (aux_LoadProgram(amx, const_cast<char*>(name.c_str()), nullptr) == AMX_ERR_NONE) {
			amx_ = amx;
		} else {
			delete amx;
		}
	}
}

AmxFile::~AmxFile() {
	if (amx_ != nullptr) {
		aux_FreeProgram(amx_);
		delete amx_;
	}
}

static std::unordered_map<std::string, std::shared_ptr<AmxFile>> string_to_amx_file;
static std::unordered_map<AMX*, std::string> amx_to_string;

template<typename OutputIterator>
static void GetFilesInDirectory(const std::string &path, const std::string &pattern, OutputIterator result) {
	#if defined _WIN32
		WIN32_FIND_DATA FindFileData;
		HANDLE hFindFile = FindFirstFile((path + "\\" + pattern).c_str(), &FindFileData);
		if (hFindFile != INVALID_HANDLE_VALUE) {
			do {
				if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
					*result++ = path + "\\" + FindFileData.cFileName;
				}
			} while (FindNextFile(hFindFile, &FindFileData) != 0);
			FindClose(hFindFile);
		}
	#else
		DIR *dp;
		if ((dp = opendir(path.c_str())) != nullptr) {
			struct dirent *dirp;
			while ((dirp = readdir(dp)) != nullptr) {
				if (!fnmatch(pattern.c_str(), dirp->d_name,
								FNM_CASEFOLD | FNM_NOESCAPE | FNM_PERIOD)) {
					*result++ = path + "/" + dirp->d_name;
				}
			}
			closedir(dp);
		}
	#endif
}

std::string GetAmxPath(AMX_HEADER *amxhdr) {
	std::string result;

	std::list<std::string> files;
	GetFilesInDirectory("gamemodes", "*.amx", std::back_inserter(files));
	GetFilesInDirectory("filterscripts", "*.amx", std::back_inserter(files));

	for (auto &filename : files) {
		auto it = ::string_to_amx_file.find(filename);
		if (it == ::string_to_amx_file.end()
			|| it->second->modification_time() < GetFileModificationTime(filename))
		{
			if (it != ::string_to_amx_file.end()) {
				::string_to_amx_file.erase(it);
			}
			auto amx_file = std::shared_ptr<AmxFile>(new AmxFile(filename));
			if (amx_file && amx_file->is_loaded()) {
				::string_to_amx_file.insert(std::make_pair(filename, amx_file));
			}
		}
	}

	for (auto &string_script : ::string_to_amx_file) {
		auto amxhdr2 = string_script.second->amx()->base;
		if (std::memcmp(amxhdr, amxhdr2, sizeof(AMX_HEADER)) == 0) {
			result = string_script.first;
			break;
		}
	}

	return result;
}

std::string GetAmxPath(AMX *amx) {
	std::string result;

	auto it = ::amx_to_string.find(amx);
	if (it != ::amx_to_string.end()) {
		result = it->second;
	} else {
		result = GetAmxPath(reinterpret_cast<AMX_HEADER*>(amx->base));
		if (!result.empty()) {
			::amx_to_string.insert(std::make_pair(amx, result));
		}
	}

	return result;
}
