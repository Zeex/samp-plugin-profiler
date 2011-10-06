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

#ifndef AMXPROFILER_H
#define AMXPROFILER_H

#include <map>
#include <stack>
#include <string>
#include <vector>

#include <malloc.h> // _alloca

#include "perfcounter.h"
#include "amx/amx.h"
#include "amx/amxdbg.h"

class AmxProfiler {
public:
    enum StatsPrintOrder {
        ORDER_NONE,
        ORDER_BY_CALLS,
        ORDER_BY_TIME,
        ORDER_BY_TIME_PER_CALL
    };

    class Function {
    public:
        Function(cell address, const char *name) 
            : address_(address),
              name_(name)
        {}

        cell address() const { return address_; }
        std::string name() const { return name_; }

    private:
        cell address_;
        std::string name_;
    };

    static void Attach(AMX *amx);
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
    AmxProfiler(AMX *amx);
    AmxProfiler(AMX *amx, AMX_DBG amxdbg);

    bool active_;
    bool haveDbg_;

    AMX          *amx_;
    AMX_DBG       amxdbg_;
    AMX_DEBUG     debug_;
    AMX_CALLBACK  callback_;

    class CallInfo {
    public:
		enum FunctionType {
			NATIVE,
			PUBLIC,
			ORDINARY
		};

        CallInfo(cell frame, cell address, FunctionType functionType) 
            : frame_(frame), 
              address_(address), 
              functionType_(functionType) 
        {}

        cell frame() const  { return frame_; }
        cell address() const  { return address_; }
		FunctionType functionType() const { return functionType_; }

    private:
        cell frame_;
        cell address_;
        FunctionType functionType_;
    };

	void EnterFunction(const CallInfo &info);
	void LeaveFunction(cell address);
	bool GetLastCall(CallInfo &call) const;

    std::stack<CallInfo> call_stack_;

    std::vector<Function> natives_;
    std::vector<Function> publics_;

    std::map<cell, PerformanceCounter> counters_;

    static std::map<AMX*, AmxProfiler*> instances_;
};

#endif