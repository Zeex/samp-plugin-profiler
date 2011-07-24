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
#include "logprintf.h"

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
      frame_ (amx_->stp)
{  
    amx_SetUserData(amx_, AMX_USERTAG('p', 'r', 'o', 'f'), this);
    amx_SetDebugHook(amx_, ::DebugHook);
}

AMXProfiler::~AMXProfiler() {
    amx_SetUserData(amx_, AMX_USERTAG('p', 'r', 'o', 'f'), 0);
}

bool AMXProfiler::Run() {
    if (!running_) {
        running_ = true;
        amx_SetCallback(amx_, ::Callback);
        return true;
    }
    return false;
}

bool AMXProfiler::IsRunning() const {
    return running_;
}

bool AMXProfiler::Terminate() {
    if (running_) {
        running_ = false;
        amx_SetCallback(amx_, callback_);
        return true;
    }
    return false;
}

void AMXProfiler::ResetStats() {
    functions_.clear();
}

void AMXProfiler::GetStats(std::vector<AMXFunPerfStats> &stats) const {
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
    };
}

int AMXProfiler::DebugHook() {
    // Check whether stack frame changed.
    if (amx_->frm != frame_) {
#ifdef _DEBUG
        logprintf("frm changed to %x", amx_->frm);
#endif
        // When a function is called with amx_Exec it is possible
        // that we couldn't detect it has returned (luck of BREAK opcodes).
        // Pop remaining functions from the call stack.
        if (!calls_.empty() && amx_->frm >= calls_.top().first) {
            while (!calls_.empty() && amx_->frm >= calls_.top().first) {
                if (amx_->frm == calls_.top().first) {
                    // Whether to pop the last element depends on whether we have entered
                    // another function or we're staying in the same one.
                    // The first is true if there's a PROC opcode 2 cells behind.
                    AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx_->base);
                    cell op = *reinterpret_cast<cell*>(amx_->base + hdr->cod + amx_->cip - 2*sizeof(cell));
                    if (op != 46 /* OP_PROC */) {
                        // OK, nothing new.
                        break;
                    }
                }
                cell address = calls_.top().second;
                if (running_) {
                    functions_[address].StopCounter();
                }
                calls_.pop();
#ifdef _DEBUG
                logprintf("Left(!) %x", address);
#endif
            }
            if (calls_.empty()) {
#ifdef _DEBUG
                logprintf("calls_.empty()");
#endif
                frame_ = amx_->stk;
            } else {
                frame_ = calls_.top().first;
            }
        }
        if (amx_->frm < frame_ || calls_.empty()) {
            // Just entered a function body (first BREAK after PROC).
            // Its address is CIP - 2*sizeof(cell).
            cell address = amx_->cip - 2*sizeof(cell);
            calls_.push(std::make_pair(amx_->frm, address));
            if (running_) {
                functions_[address].IncreaseCalls();
                functions_[address].StartCounter();
            }
#ifdef _DEBUG
            logprintf("Entered %x", address);
#endif
        } else if (amx_->frm > frame_) {
            // Left a function.
            // The addres is at the top of the call stack.
            cell address = calls_.top().second;
            if (running_) {
                functions_[address].StopCounter();        
            }
            calls_.pop();
#ifdef _DEBUG
            logprintf("Left %x", address);
#endif
        }
        frame_ = amx_->frm;
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
