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

#include "amxprofiler.h"

platformstl::int64_t AMXFunPerfCounter::GetExecutionTime() const {
    return time_;
}

platformstl::int64_t AMXFunPerfCounter::GetNumberOfCalls() const {
    return calls_;
}

void AMXFunPerfCounter::StartCounter() {
    counter_.start();
}

void AMXFunPerfCounter::StopCounter() {
    counter_.stop();
    time_ += counter_.get_microseconds();
}

void AMXFunPerfCounter::IncreaseCalls() {
    calls_++;
}

AMXProfiler::AMXProfiler(AMX *amx) 
    : amx_(amx), 
      running_(false), 
      debugHook_(amx->debug), 
      callback_(amx->callback), 
      currentStackFrame_(0)
{  
    amx_SetUserData(amx_, AMX_USERTAG('p', 'r', 'o', 'f'), this);
}

AMXProfiler::~AMXProfiler() {
    amx_SetUserData(amx_, AMX_USERTAG('p', 'r', 'o', 'f'), 0);
}

static int AMXAPI DebugHook(AMX *amx) {
    void *prof;
    amx_GetUserData(amx, AMX_USERTAG('p', 'r', 'o', 'f'), &prof);
    return static_cast<AMXProfiler*>(prof)->DebugHook();
}

static int AMXAPI Callback(AMX *amx, cell index, cell *result, cell *params) {
    void *prof;
    amx_GetUserData(amx, AMX_USERTAG('p', 'r', 'o', 'f'), &prof);
    return static_cast<AMXProfiler*>(prof)->Callback(index, result, params);
}

bool AMXProfiler::Run() {
    if (!running_) {
        currentStackFrame_ = 0;
        functions_.clear();
        amx_SetDebugHook(amx_, ::DebugHook);
        amx_SetCallback(amx_, ::Callback);
        running_ = true;
        return true;
    }
    return false;
}

bool AMXProfiler::IsRunning() const {
    return running_;
}

bool AMXProfiler::Terminate() {
    if (running_) {
        amx_SetDebugHook(amx_, debugHook_);
        amx_SetCallback(amx_, callback_);
        running_ = false;
        return true;
    }
    return false;
}

// Get address of a CALL by frame
static cell GetCallAddress(AMX *amx, cell frame) {
    AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx->base);
    cell data = reinterpret_cast<cell>(amx->base + hdr->dat);
    return *(reinterpret_cast<cell*>(data + frame) + 1) - 2*sizeof(cell);
}

// Get address of callee
static cell GetCallTarget(AMX *amx, cell callAddr) {
    AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx->base);
    cell code = reinterpret_cast<cell>(amx->base) + hdr->cod;
    return *reinterpret_cast<cell*>(callAddr + sizeof(cell) + code) - code;
}

int AMXProfiler::DebugHook() {
    if (amx_->frm != currentStackFrame_) {
        if (amx_->frm < currentStackFrame_ || currentStackFrame_ == 0) {
            // Entered a function
            cell callAddr = GetCallAddress(amx_, amx_->frm);
            if (callAddr > 0) {
                cell funcAddr = GetCallTarget(amx_, callAddr);
                functions_[funcAddr].IncreaseCalls();
                functions_[funcAddr].StartCounter();
            }
        } else if (amx_->frm > currentStackFrame_) {
            // Left a function
            cell callAddr = GetCallAddress(amx_, currentStackFrame_);
            if (callAddr > 0) {
                cell funcAddr = GetCallTarget(amx_, callAddr);
                functions_[funcAddr].StopCounter();        
            }
        }
        currentStackFrame_ = amx_->frm;
    }

    if (debugHook_ != 0) {
        // Others could set their own debug hooks
        return debugHook_(amx_);
    }                     
    return AMX_ERR_NONE;  
}

int AMXProfiler::Callback(cell index, cell *result, cell *params) {
    AMXFunPerfCounter &fun = functions_[-index]; // Notice negative index
    fun.StartCounter();

    // The default AMX callback (amx_Callback) can replace SYSREQ.C opcodes
    // with SYSREQ.D for better performance. 
    amx_->sysreq_d = 0; 

    // Call any previously set AMX callback (must not be null so we don't check)
    int error = callback_(amx_, index, result, params);

    fun.StopCounter();
    fun.IncreaseCalls();

    return error;  
}

std::vector<AMXFunPerfStats> AMXProfiler::GetStats() const {
    std::vector<AMXFunPerfStats> stats;

    for (std::map<cell, AMXFunPerfCounter>::const_iterator it = functions_.begin();
         it != functions_.end(); ++it)
    {
        if (it->second.GetNumberOfCalls() > 0) { // Sometimes calls == 0
            AMXFunPerfStats st;
            cell address = it->first;
            st.native = address <= 0;
            st.address = address < 0 ? -address : address;
            st.numberOfCalls = it->second.GetNumberOfCalls();
            st.executionTime = it->second.GetExecutionTime();
            stats.push_back(st);
        }
    }

    return stats;
}

