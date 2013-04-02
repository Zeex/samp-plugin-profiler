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
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "amxpath.h"

static void FreeAmx(AMX *amx) {
	if (amx != 0) {
		aux_FreeProgram(amx);
	}
}

AmxFile::AmxFile(std::string name)
	: amx_ptr_(new AMX, FreeAmx)
	, name_(name)
	, mtime_(boost::filesystem::last_write_time(name))
{
	if (aux_LoadProgram(amx_ptr_.get(), const_cast<char*>(name.c_str()), 0)
			!= AMX_ERR_NONE) {
		amx_ptr_.reset();
	}
}

std::string AmxPathFinder::FindAmxPath(AMX *amx) const {
	std::string result;

	PathCache::const_iterator iterator = path_cache_.find(amx);
	if (iterator != path_cache_.end()) {
		result = iterator->second;
	} else {
		result = FindAmxPath(reinterpret_cast<AMX_HEADER*>(amx->base));
		if (!result.empty()) {
			path_cache_.insert(std::make_pair(amx, result));
		}
	}

	return result;
}

std::string AmxPathFinder::FindAmxPath(AMX_HEADER *amxhdr) const {
	std::string result;

	for (DirSet::const_iterator iterator = search_dirs_.begin();
			iterator != search_dirs_.end(); ++iterator)
	{
		const std::string &dir = *iterator;

		boost::filesystem::directory_iterator dir_end;
		for (boost::filesystem::directory_iterator dir_iterator(dir);
				dir_iterator != dir_end; ++dir_iterator)
		{
			if (!boost::filesystem::is_regular_file(dir_iterator->status())) {
				continue;
			}

			const std::string filename = dir_iterator->path().string();
			if (!boost::algorithm::ends_with(filename, ".amx")) {
				continue;
			}

			FileCache::const_iterator cache_iterator = file_cache_.find(filename);
			if (cache_iterator == file_cache_.end() ||
			    cache_iterator->second.mtime() < boost::filesystem::last_write_time(filename))
			{
				if (cache_iterator != file_cache_.end()) {
					file_cache_.erase(cache_iterator);
				}

				AmxFile amx_file(filename);
				if (amx_file.is_loaded()) {
					file_cache_.insert(std::make_pair(filename, amx_file));
				}
			}
		}
	}

	for (FileCache::const_iterator iterator = file_cache_.begin();
			iterator != file_cache_.end(); ++iterator) 
	{
		void *amxhdr2 = iterator->second.amx()->base;
		if (std::memcmp(amxhdr, amxhdr2, sizeof(AMX_HEADER)) == 0) {
			result = iterator->first;
			break;
		}
	}

	return result;
}
