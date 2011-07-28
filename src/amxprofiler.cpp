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

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>

#include "amxprofiler.h"

#include "amx/amx.h"
#include "amx/amxdbg.h"

std::map<AMX*, AmxProfiler*> AmxProfiler::instances_;

AmxProfiler::AmxProfiler() {}

static void GetNatives(AMX *amx, std::vector<std::string> &names) {
    AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx->base);

    AMX_FUNCSTUBNT *natives = reinterpret_cast<AMX_FUNCSTUBNT*>(amx->base + hdr->natives);
    int numNatives = (hdr->libraries - hdr->natives) / hdr->defsize;

    for (int i = 0; i < numNatives; i++) {
        names.push_back(reinterpret_cast<char*>(amx->base + natives[i].nameofs));
    }
}

AmxProfiler::AmxProfiler(AMX *amx, AMX_DBG amxdbg) 
    : amx_(amx),
      amxdbg_(amxdbg),
      debug_(amx->debug),
      callback_(amx->callback),
      active_(false),
      frame_(amx->stp)
{
    // Since PrintStats is done in AmxUnload and amx->base is already freed before
    // AmxUnload gets called, the native table is not accessible there, thus natives' 
    // names must be stored manually in another global place.
    GetNatives(amx, nativeNames_);
}

void AmxProfiler::Attach(AMX *amx, AMX_DBG amxdbg) {
    AmxProfiler *prof = new AmxProfiler(amx, amxdbg);
    instances_[amx] = prof;
    prof->Activate();
}

void AmxProfiler::Detach(AMX *amx) {
    AmxProfiler *prof = AmxProfiler::Get(amx);
    if (prof != 0) {
        prof->Deactivate();
        delete prof;
    }
    instances_.erase(amx);
}

AmxProfiler *AmxProfiler::Get(AMX *amx) {
    std::map<AMX*, AmxProfiler*>::iterator it = instances_.find(amx);
    if (it != instances_.end()) {
        return it->second;
    }
    return 0;
}

static int AMXAPI Debug(AMX *amx) {
    return AmxProfiler::Get(amx)->Debug();
}

static int AMXAPI Callback(AMX *amx, cell index, cell *result, cell *params) {
    return AmxProfiler::Get(amx)->Callback(index, result, params);
}

void AmxProfiler::Activate() {
    if (!active_) {
        active_ = true;
        amx_SetDebugHook(amx_, ::Debug);
        amx_SetCallback(amx_, ::Callback);
    }
}

bool AmxProfiler::IsActive() const {
    return active_;
}

void AmxProfiler::Deactivate() {
    if (active_) {
        active_ = false;
        amx_SetDebugHook(amx_, debug_);
        amx_SetCallback(amx_, callback_);
    }
}

void AmxProfiler::ResetStats() {
    counters_.clear();
}

static bool ByCalls(const std::pair<cell, AmxPerformanceCounter> &op1, 
                         const std::pair<cell, AmxPerformanceCounter> &op2) {
    return op1.second.GetCalls() > op2.second.GetCalls();
}

static bool ByTime(const std::pair<cell, AmxPerformanceCounter> &op1, 
                        const std::pair<cell, AmxPerformanceCounter> &op2) {
    return op1.second.GetTime() > op2.second.GetTime();
}

static bool ByTimePerCall(const std::pair<cell, AmxPerformanceCounter> &op1, 
                               const std::pair<cell, AmxPerformanceCounter> &op2) {
    return static_cast<double>(op1.second.GetTime()) / static_cast<double>(op1.second.GetCalls())
         > static_cast<double>(op2.second.GetTime()) / static_cast<double>(op2.second.GetCalls());
}

bool AmxProfiler::PrintStats(const std::string &filename, StatsPrintOrder order) {
    std::ofstream stream(filename.c_str());

    if (stream.is_open()) {
        std::vector<std::pair<cell, AmxPerformanceCounter> > stats(counters_.begin(), 
                                                     counters_.end());
        switch (order) {
            case ORDER_BY_CALLS:
                std::sort(stats.begin(), stats.end(), ByCalls);
                break;
            case ORDER_BY_TIME:
                std::sort(stats.begin(), stats.end(), ByTime);
                break;
            case ORDER_BY_TIME_PER_CALL:
                std::sort(stats.begin(), stats.end(), ByTimePerCall);
                break;
            default:
                // leave as is
                break;
        }

        stream << "<table>\n"
               << "\t<tr>\n"
               << "\t\t<td>Function</td>\n"
               << "\t\t<td>Calls</td>\n"
               << "\t\t<td>Time per call, &#181;s</td>\n"
               << "\t\t<td>Overall time, &#181;s</td>\n"
               << "\t\t<td>Overall time, &#037;</td>\n"
               << "\t</tr>\n";

        platformstl::int64_t totalTime = 0;

        for (std::vector<std::pair<cell, AmxPerformanceCounter> >::iterator it = stats.begin(); 
             it != stats.end(); ++it) 
        {
            totalTime += it->second.GetTime();
        }        

        for (std::vector<std::pair<cell, AmxPerformanceCounter> >::iterator it = stats.begin(); 
             it != stats.end(); ++it) 
        {
            stream << "\t<tr>\n";

            if (it->first <= 0) {
                stream << "\t\t<td>" << nativeNames_[-it->first] << "</td>\n";
            } else {
                const char *name;
                if (dbg_LookupFunction(&amxdbg_, it->first, &name) == AMX_ERR_NONE) {
                    //stream << "\t\t<td>" << name << "@" << std::hex << it->first << std::dec << "</td>\n";
                    stream << "\t\t<td>" << name << "</td>\n";
                } else {
                    stream << "\t\t<td>" << std::hex << it->first << std::dec << "</td>\n";
                }
            }

            stream << "\t\t<td>" << it->second.GetCalls() << "</td>\n"
                   << "\t\t<td>" << std::fixed << std::setprecision(0)
                                 << static_cast<double>(it->second.GetTime()) / 
                                        static_cast<double>(it->second.GetCalls()) << "</td>\n"
                   << "\t\t<td>" << it->second.GetTime() << "</td>\n"
                   << "\t\t<td>" << std::setprecision(2)
                                 << static_cast<double>(it->second.GetTime() * 100) / 
                                        static_cast<double>(totalTime) << "</td>\n";
            stream << "\t</tr>\n";
        }

        stream << "</table>\n";

        return true;
    }

    return false;
}

int AmxProfiler::Debug() {
    if (amx_->frm != frame_ && frame_ > amx_->hea) {
        if (amx_->frm < frame_) {
            // Probably entered a function body (first BREAK after PROC)
            cell address = amx_->cip - 2*sizeof(cell);
            // Check if we have a PROC opcode behind us
            AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx_->base);
            cell op = *reinterpret_cast<cell*>(hdr->cod + amx_->base + address);
            if (op == 46) {
                callStack_.push(address);
                if (active_) {
                    counters_[address].Start();
                }
            } 
        } else {
            // Left a function
            cell address = callStack_.top();
            if (active_) {
                counters_[address].Stop();
            }
            callStack_.pop();
            if (callStack_.empty()) {
                frame_ = amx_->stp;
            }
        }
        frame_ = amx_->frm;
    }

    if (debug_ != 0) {
        // Others could set their own debug hooks
        return debug_(amx_);
    }   

    return AMX_ERR_NONE;      
}

int AmxProfiler::Callback(cell index, cell *result, cell *params) {
    // The default AMX callback (amx_Callback) can replace SYSREQ.C opcodes
    // with SYSREQ.D for better performance. 
    amx_->sysreq_d = 0; 

    if (active_) {
        counters_[-index].Start();; // Notice negative index
    }

    // Call any previously set AMX callback (must not be null so we don't check)
    int error = callback_(amx_, index, result, params);

    if (active_) {
        counters_[-index].Stop();
    }

    return error;
}

int AmxProfiler::Exec(cell *retval, int index) {
    AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx_->base);
    AMX_FUNCSTUBNT *publics = reinterpret_cast<AMX_FUNCSTUBNT*>(amx_->base + hdr->publics);

    cell address = publics[index].address;
    callStack_.push(address);

    // Set frame_ to the value which amx_->frm will be set to
    // during amx_Exec. If we do this Debug() will think that 
    // the frame stays the same and won't profile this call.
    frame_ = amx_->stk - 3*sizeof(cell);

    counters_[address].Start();
    int error = amx_Exec(amx_, retval, index);
    counters_[address].Stop();

    if (!callStack_.empty()) {
        callStack_.pop();
    } else {
        frame_ = amx_->stp;
    }

    return error;
}

