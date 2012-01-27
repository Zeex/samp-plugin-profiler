// AMX profiler for SA-MP server: http://sa-mp.com
//
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

#ifndef AMX_PROFILER_CHRONO_H
#define AMX_PROFILER_CHRONO_H

#ifdef HAVE_BOOST_CHRONO
	#include <boost/chrono.hpp>
	namespace amx_profiler { namespace chrono = boost::chrono; }
#else
	#include <chrono>
	namespace amx_profiler { namespace chrono = std::chrono; }
#endif

#endif // !AMX_PROFILER_CHRONO_H
