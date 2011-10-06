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

// Extracts the names of native functions from the native table.
static void GetNatives(AMX *amx, std::vector<AmxProfiler::Function> &natives) {
    AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx->base);
    AMX_FUNCSTUBNT *nativeTable = reinterpret_cast<AMX_FUNCSTUBNT*>(amx->base + hdr->natives);

    int numberOfNatives;
    amx_NumNatives(amx, &numberOfNatives);

    for (int i = 0; i < numberOfNatives; i++) {
        natives.push_back(AmxProfiler::Function(nativeTable[i].address,
            reinterpret_cast<char*>(amx->base + nativeTable[i].nameofs)));
    }
}

// Extracts the names of public functions from the native table.
static void GetPublics(AMX *amx, std::vector<AmxProfiler::Function> &publics) {
    AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx->base);
    AMX_FUNCSTUBNT *publicTable = reinterpret_cast<AMX_FUNCSTUBNT*>(amx->base + hdr->publics);

    int numberOfPublics;
    amx_NumPublics(amx, &numberOfPublics);

    for (int i = 0; i < numberOfPublics; i++) {
        publics.push_back(AmxProfiler::Function(publicTable[i].address, 
            reinterpret_cast<char*>(amx->base + publicTable[i].nameofs)));
    }
}

// Reads from a code section at a given location.
static inline cell ReadAmxCode(AMX *amx, cell where) {
    AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx->base);
    return *reinterpret_cast<cell*>(amx->base + hdr->cod + where);
}

// Comparison functiosn for different stats sorting modes
static bool ByCalls(const std::pair<cell, PerformanceCounter> &op1, 
                         const std::pair<cell, PerformanceCounter> &op2) {
    return op1.second.GetCalls() > op2.second.GetCalls();
}

static bool ByTime(const std::pair<cell, PerformanceCounter> &op1, 
                        const std::pair<cell, PerformanceCounter> &op2) {
    return op1.second.GetTime() > op2.second.GetTime();
}

static bool ByTimePerCall(const std::pair<cell, PerformanceCounter> &op1, 
                               const std::pair<cell, PerformanceCounter> &op2) {
    return static_cast<double>(op1.second.GetTime()) / static_cast<double>(op1.second.GetCalls())
         > static_cast<double>(op2.second.GetTime()) / static_cast<double>(op2.second.GetCalls());
}

AmxProfiler::AmxProfiler(AMX *amx) 
    : amx_(amx),
      debug_(amx->debug),
      callback_(amx->callback),
      active_(false),
      haveDbg_(false)
{
    // Since PrintStats is done in AmxUnload and amx->base is already freed before
    // AmxUnload gets called, therefore both native and public tables are not accessible, 
    // from there, so they must be stored separately in some global place.
    GetNatives(amx, natives_);
    GetPublics(amx, publics_);
}

AmxProfiler::AmxProfiler(AMX *amx, AMX_DBG amxdbg) 
    : amx_(amx),
      amxdbg_(amxdbg),
      debug_(amx->debug),
      callback_(amx->callback),
      active_(false),
      haveDbg_(true)
{
    GetNatives(amx, natives_);
    GetPublics(amx, publics_);
}

void AmxProfiler::Attach(AMX *amx) {
    AmxProfiler *prof = new AmxProfiler(amx);
    instances_[amx] = prof;
    prof->Activate();
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

bool AmxProfiler::PrintStats(const std::string &filename, StatsPrintOrder order) {
    std::ofstream stream(filename.c_str());

    if (stream.is_open()) {
        std::vector<std::pair<cell, PerformanceCounter> > stats(counters_.begin(), 
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

        for (std::vector<std::pair<cell, PerformanceCounter> >::iterator it = stats.begin(); 
             it != stats.end(); ++it) 
        {
            totalTime += it->second.GetTime();
        }        

        for (std::vector<std::pair<cell, PerformanceCounter> >::iterator it = stats.begin(); 
             it != stats.end(); ++it) 
        {
            stream << "\t<tr>\n";

            cell address = it->first;
            if (address <= 0) {
				AmxProfiler::Function &native = natives_[-address];
				if (native.name().empty()) {
					stream << "\t\t<td>" << "unknown native @ " << native.address() << "</td>\n";
				} else {
					stream << "\t\t<td>" << natives_[-address].name() << "</td>\n";
				}
            } else {
                const char *name = 0;
                if (haveDbg_ && dbg_LookupFunction(&amxdbg_, address, &name) == AMX_ERR_NONE) {
                    stream << "\t\t<td>" << name << "</td>\n";
                } else {
                    bool found = false;
                    for (std::vector<AmxProfiler::Function>::iterator pubIt = publics_.begin(); 
                         pubIt != publics_.end(); ++pubIt) 
                    {
                        if (pubIt->address() == address)  {
                            stream << "\t\t<td>" << pubIt->name() << "</td>\n";
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        // OK we tried all means but still don't know the name, so just print the address.
                        stream << "\t\t<td>" << "0x" << std::hex << address << std::dec << "</td>\n";
                    }
                }
            }

            PerformanceCounter &counter = it->second;

            stream << "\t\t<td>" << counter.GetCalls() << "</td>\n"
                   << "\t\t<td>" << std::fixed << std::setprecision(0)
                                 << static_cast<double>(counter.GetTime()) / 
                                        static_cast<double>(counter.GetCalls()) << "</td>\n"
                   << "\t\t<td>" << counter.GetTime() << "</td>\n"
                   << "\t\t<td>" << std::setprecision(2)
                                 << static_cast<double>(counter.GetTime() * 100) / 
                                        static_cast<double>(totalTime) << "</td>\n";
            stream << "\t</tr>\n";
        }

        stream << "</table>\n";

        return true;
    }

    return false;
}

int AmxProfiler::Debug() {
    // Get previous stack frame.
    cell prevFrame = amx_->stp;

    if (!call_stack_.empty()) {
        prevFrame = call_stack_.top().frame();
    }

    // Check whether current frame is different.
    if (amx_->frm < prevFrame) {
        // Probably entered a function body (first BREAK after PROC).
        cell address = amx_->cip - 2*sizeof(cell);            
        // Check if we have a PROC opcode behind.
        if (ReadAmxCode(amx_, address) == 46) {
            EnterFunction(CallInfo(amx_->frm, address, CallInfo::ORDINARY));
        }
    } else if (amx_->frm > prevFrame) {
        if (call_stack_.top().functionType() != CallInfo::PUBLIC) { // entry points are handled by Exec
            // Left the function
            cell address = call_stack_.top().address();
            LeaveFunction(address);
        }
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

	cell address = -index;

	EnterFunction(CallInfo(amx_->frm, address, CallInfo::NATIVE));
    int error = callback_(amx_, index, result, params);
	LeaveFunction(address);

    return error;
}

int AmxProfiler::Exec(cell *retval, int index) {	
    if (index >= 0 || index == AMX_EXEC_MAIN) {		
        AMX_HEADER *hdr = reinterpret_cast<AMX_HEADER*>(amx_->base);
        cell address = 0;
        if (index == AMX_EXEC_MAIN) {
            address = hdr->cip;
        } else {
            AMX_FUNCSTUBNT *publics = reinterpret_cast<AMX_FUNCSTUBNT*>(amx_->base + hdr->publics);
            address = publics[index].address;
        }        

		EnterFunction(CallInfo(amx_->stk - 3*sizeof(cell), address, CallInfo::PUBLIC));
        int error = amx_Exec(amx_, retval, index);
		LeaveFunction(address);

        return error;
    } else {
        return amx_Exec(amx_, retval, index);
    }
}

void AmxProfiler::EnterFunction(const CallInfo &info) {
	if (active_) {
		PerformanceCounter &counter = counters_[info.address()];
		if (call_stack_.empty()) {
			counter.Start();
		} else {
			counter.Start(&counters_[call_stack_.top().address()]);
		}
	}
	call_stack_.push(info);
}

void AmxProfiler::LeaveFunction(cell address) {
	while (true) {
		cell topAddress = call_stack_.top().address();
		if (active_) {
			counters_[topAddress].Stop();
		}
		call_stack_.pop();		
		if (topAddress == address) {
			break;
		}
	}
}

bool AmxProfiler::GetLastCall(CallInfo &call) const {
	if (!call_stack_.empty()) {
		call = call_stack_.top();
		return true;
	}
	return false;
}