// SA:MP Profiler plugin
//
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

#include <cassert>

#include "function.h"

namespace samp_profiler {

Function::Function(Type type, Handle handle) 
	: type_(type)
	, handle_(handle)
	, time_(0)
	, child_time_(0)
	, num_calls_(0)
{
}

// static
Function Function::Native(cell index) {
	Handle handle;
	handle.index = index;
	return Function(NATIVE, handle);
}

// static
Function Function::Public(cell index) {
	Handle handle;
	handle.index = index;
	return Function(PUBLIC, handle);
}

// static
Function Function::Normal(ucell address) {
	Handle handle;
	handle.address = address;
	return Function(NORMAL, handle);
}

bool Function::operator<(const Function &that) const {
	if (this->type() == that.type()) {
		switch (type_) {
		case NATIVE:
		case PUBLIC:
			return this->handle_.index < that.handle_.index;
		case NORMAL:
			return this->handle_.address < that.handle_.address;
		default:
			assert(0 && "Unknown function type");
			return false; // never reached
		}
	} else {
		return this->type() < that.type();
	}
}

bool Function::operator==(const Function &that) const {
	if (this->type_ != that.type_) {
		return false;
	}
	switch (type_) {
	case NATIVE:
	case PUBLIC:
		return this->handle_.index == that.handle_.index;
	case NORMAL:
		return this->handle_.address == that.handle_.address;
	default:
		assert(0 && "Unknown function type");
		return false; // never reached
	}
}

} // namespace samp_profiler
