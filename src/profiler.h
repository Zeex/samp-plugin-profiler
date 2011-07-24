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

#ifndef PROFILER_H
#define PROFILER_H

#include <map>
#include <stack>
#include <string>

#include <platformstl/platformstl.hpp>
#include <platformstl/performance/performance_counter.hpp>

#include <malloc.h> // _alloca

#include "amx/amx.h"
#include "amx/amxdbg.h"

class AmxPerformanceCounter {
public:
    AmxPerformanceCounter() 
        : calls_(0),
          counter_(),
          time_(0)  
    {
    }

    inline void Start() {
        calls_++;
        counter_.start();
    }

    inline void Stop() {
        counter_.stop();
        time_ += counter_.get_microseconds();
    }

    inline platformstl::int64_t GetCalls() const {
        return calls_;
    }

    inline platformstl::int64_t GetTime() const {
        return time_;
    }

private:
    platformstl::int64_t calls_;
    platformstl::performance_counter counter_;
    platformstl::performance_counter::interval_type time_;
};

class AmxProfiler {
public:
    enum StatsPrintOrder {
        ORDER_NONE,
        ORDER_BY_CALLS,
        ORDER_BY_TIME,
        ORDER_BY_TIME_PER_CALL
    };

    static void Attach(AMX *amx, AMX_DBG amxdbg);
    static void Detach(AMX *amx);

    static AmxProfiler *Get(AMX *amx);

    void Activate();
    bool IsActive() const;
    void Deactivate();

    void ResetStats();
    bool PrintStats(const std::string &filename, StatsPrintOrder order = ORDER_BY_TIME);

    int Debug();
    int Callback(cell index, cell *result, cell *params);
    int Exec(cell *retval, int index);

private:
    AmxProfiler();
    AmxProfiler(AMX *amx, AMX_DBG amxdbg);

    bool active_;

    AMX          *amx_;
    AMX_DBG       amxdbg_;
    AMX_DEBUG     debug_;
    AMX_CALLBACK  callback_;

    cell frame_;
    std::stack<cell> callStack_;

    std::vector<std::string> nativeNames_;

    std::map<ucell, AmxPerformanceCounter> counters_;

    static std::map<AMX*, AmxProfiler*> instances_;
};

#endif