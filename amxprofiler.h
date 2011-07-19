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

#ifndef AMXPROFILER_H
#define AMXPROFILER_H

#include <map>
#include <string>
#include <vector>

#include <platformstl/platformstl.hpp>
#include <platformstl/performance/performance_counter.hpp>

#include <amx/amx.h>

class AMXFunctionProfile {
public:
    AMXFunctionProfile() : calls_(0), time_(0) {}

    platformstl::int64_t GetExecutionTime() const;
    platformstl::int64_t GetNumberOfCalls() const;

private:
    friend class AMXProfiler;

    void StartCounter();
    void StopCounter();
    void IncreaseCalls();

private:
    platformstl::int64_t calls_;
    platformstl::performance_counter counter_;
    platformstl::performance_counter::interval_type time_;
};

struct AMXProfilerStat {
    // Is it a native function?
    bool native; 
    // A function address or native table index 
    cell address; 
    // How many times it was called
    platformstl::int64_t numberOfCalls;
    // Total execution time in microseconds
    platformstl::int64_t executionTime;
};

class AMXProfiler {
public:
    explicit AMXProfiler(AMX *amx);
    ~AMXProfiler();

    void Run();
    bool IsRunning() const;
    void Terminate();

    std::vector<AMXProfilerStat> GetStats() const;

    int DebugHook();
    int Callback(cell index, cell *result, cell *params);

private:
    // Default ctor
    AMXProfiler();

private:
    AMX *amx_;
    bool running_;

    // Previously set debug hook and callback, if any
    AMX_DEBUG debugHook_;
    AMX_CALLBACK callback_;

    // To keep track of stack frame change
    cell currentStackFrame_;

    // Per-function data
    std::map<cell, AMXFunctionProfile> functions_;
};

#endif
