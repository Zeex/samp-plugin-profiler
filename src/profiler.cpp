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
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <list>
#include <numeric>
#include <string>
#include <vector>

#include "jump.h"
#include "plugincommon.h"
#include "profiler.h"

std::map<AMX*, AmxProfiler*> AmxProfiler::instances_;

AmxProfiler::AmxProfiler() {}

AmxProfiler::AmxProfiler(AMX *amx, AMX_DBG amxdbg) 
    : amx_(amx),
      amxdbg_(amxdbg),
      debug_(amx->debug),
      callback_(amx->callback),
      active_(false),
      frame_(amx->stp)
{
}

void AmxProfiler::Attach(AMX *amx, AMX_DBG amxdbg) {
    instances_[amx] = new AmxProfiler(amx, amxdbg);
}

void AmxProfiler::Detach(AMX *amx) {
    delete AmxProfiler::Get(amx);
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
    active_ = true;
    amx_SetDebugHook(amx_, ::Debug);
    amx_SetCallback(amx_, ::Callback);
}

bool AmxProfiler::IsActive() const {
    return active_;
}

void AmxProfiler::Deactivate() {
    active_ = false;
    amx_SetDebugHook(amx_, debug_);
    amx_SetCallback(amx_, callback_);
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

static const char *GetNativeName(AMX *amx, cell index) {
    AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx->base);

    AMX_FUNCSTUBNT *natives = reinterpret_cast<AMX_FUNCSTUBNT*>(amx->base + hdr->natives);
    int numNatives = (hdr->libraries - hdr->natives) / hdr->defsize;

    for (int i = 0; i < numNatives; i++) {
        if (i == index) {
            return reinterpret_cast<char*>(amx->base + natives[i].nameofs);
        }
    }
    return "<unknown native>";
}

bool AmxProfiler::PrintStats(const char *filename, StatsPrintOrder order) {
    std::ofstream stream(filename);

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
                stream << "\t\t<td>" << GetNativeName(amx_, -it->first) << "</td>\n";
            } else {
                const char *name;
                if (dbg_LookupFunction(&amxdbg_, it->first, &name) == AMX_ERR_NONE) {
                    stream << "\t\t<td>" << name << "@" << std::hex << it->first << std::dec << "</td>\n";
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
    if (amx_->frm != frame_) {
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

static uint32_t amx_Exec_addr;
static unsigned char amx_Exec_code[5];

static int AMXAPI Exec(AMX *amx, cell *retval, int index) {
    memcpy(reinterpret_cast<void*>(::amx_Exec_addr), ::amx_Exec_code, 5);

    int error;

    // Check if this script has a profiler attached to it
    AmxProfiler *prof = AmxProfiler::Get(amx);
    if (prof != 0 && prof->IsActive()) {
        error =  prof->Exec(retval, index);
    } else {
        error = amx_Exec(amx, retval, index);
    }

    SetJump(reinterpret_cast<void*>(::amx_Exec_addr), (void*)::Exec, ::amx_Exec_code);

    return error;
}

int AmxProfiler::Exec(cell *retval, int index) {
    AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx_->base);
    AMX_FUNCSTUBNT *publics = reinterpret_cast<AMX_FUNCSTUBNT*>(amx_->base + hdr->publics);

    cell address = publics[index].address;
    callStack_.push(address);
    counters_[address].Start();

    int error = amx_Exec(amx_, retval, index);

    counters_[address].Stop();
    callStack_.pop();

    return error;
}

namespace natives {
    
    // native Profiler_Init(const path_to_amx[]);
    cell AMX_NATIVE_CALL Profiler_Init(AMX *amx, cell *params) {
        char *path_to_amx;
        amx_StrParam(amx, params[1], path_to_amx);
        if (path_to_amx != 0) {
            FILE *fp = fopen(path_to_amx, "rb");
            if (fp != 0) {
                AMX_DBG amxdbg;
                if (dbg_LoadInfo(&amxdbg, fp) == AMX_ERR_NONE) {
                    AmxProfiler::Attach(amx, amxdbg);
                    return 1;
                }
                fclose(fp);
            } 
        }
        return 0;
    }

    // native Profiler_Start();
    cell AMX_NATIVE_CALL Profiler_Start(AMX *amx, cell *params) {
        AmxProfiler *prof = AmxProfiler::Get(amx);
        if (!prof->IsActive()) {
            prof->Activate();
            return 1;
        } 
        return 0;
    }

    // native Profiler_Stop();
    cell AMX_NATIVE_CALL Profiler_Stop(AMX *amx, cell *params) {
        AmxProfiler *prof = AmxProfiler::Get(amx);
        if (prof->IsActive()) {
            prof->Deactivate();
            return 1;
        } 
        return 0;
    }

    // native Profiler_ResetStats();
    cell AMX_NATIVE_CALL Profiler_ResetStats(AMX *amx, cell *params) {
        AmxProfiler::Get(amx)->ResetStats();
        return 0;
    }

    // native Profiler_PrintStats(const filename[], ProfilerStatsOrder:order);
    cell AMX_NATIVE_CALL Profiler_PrintStats(AMX *amx, cell *params) {
        char *filename;
        amx_StrParam(amx, params[1], filename);
        return AmxProfiler::Get(amx)->PrintStats(filename, static_cast<AmxProfiler::StatsPrintOrder>(params[2]));
    }

    // native Profiler_Finalize()

    const AMX_NATIVE_INFO all[] = { 
        {"Profiler_Init",       Profiler_Init},
        {"Profiler_Start",      Profiler_Start},
        {"Profiler_Stop",       Profiler_Stop},
        {"Profiler_ResetStats", Profiler_ResetStats},
        {"Profiler_PrintStats", Profiler_PrintStats},
        {0,                     0}
    };
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

extern void *pAMXFunctions;

// Both x86 and x86-64 are Little Endian
static void *AMXAPI DummyAmxAlign(void *v) { return v; }

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **pluginData) {
    pAMXFunctions = pluginData[PLUGIN_DATA_AMX_EXPORTS];

    // The server does not export amx_Align16 and amx_Align32 for some reason
    ((void**)pAMXFunctions)[PLUGIN_AMX_EXPORT_Align16] = (void*)DummyAmxAlign;
    ((void**)pAMXFunctions)[PLUGIN_AMX_EXPORT_Align32] = (void*)DummyAmxAlign;

    ::amx_Exec_addr = reinterpret_cast<uint32_t>((static_cast<void**>(pAMXFunctions))[PLUGIN_AMX_EXPORT_Exec]);
    SetJump(reinterpret_cast<void*>(::amx_Exec_addr), (void*)::Exec, ::amx_Exec_code);

    return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
    return;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
    return amx_Register(amx, natives::all, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) {
    AmxProfiler::Detach(amx);
    return AMX_ERR_NONE;
}
