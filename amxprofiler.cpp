#include <algorithm>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

#include "amxprofiler.h"

AMXFunctionProfile::IntervalType AMXFunctionProfile::GetExecutionTime() const {
    return time_;
}

AMXFunctionProfile::IntervalType AMXFunctionProfile::GetNumberOfCalls() const {
    return calls_;
}

void AMXFunctionProfile::StartCounter() {
    counter_.start();
}

void AMXFunctionProfile::StopCounter() {
    counter_.stop();
    time_ += counter_.get_microseconds();
}

void AMXFunctionProfile::IncreaseCalls() {
    calls_++;
}

AMXProfiler::AMXProfiler(AMX *amx) 
    : amx_(amx), running_(false), debugHook_(amx->debug), currentStackFrame_(0)
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

void AMXProfiler::Run() {
    functions_.clear();
    amx_SetDebugHook(amx_, ::DebugHook);
}

bool AMXProfiler::IsRunning() const {
    return running_;
}

void AMXProfiler::Terminate() {
    amx_SetDebugHook(amx_, debugHook_);
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
            // Entered function
            cell callAddr = GetCallAddress(amx_, amx_->frm);
            if (callAddr > 0) {
                cell funcAddr = GetCallTarget(amx_, callAddr);
                functions_[funcAddr].IncreaseCalls();
                functions_[funcAddr].StartCounter();
            }
        } else if (amx_->frm > currentStackFrame_) {
            // Left function
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

typedef std::pair<cell, AMXFunctionProfile> KeyValPair;

static bool ByExecutionTime(const KeyValPair &op1, const KeyValPair &op2) {
    return op1.second.GetExecutionTime() > op2.second.GetExecutionTime();
}

static bool ByNumberOfCalls(const KeyValPair &op1, const KeyValPair &op2) {
    return op1.second.GetNumberOfCalls() > op2.second.GetNumberOfCalls();
}

static void PrintHorizLine(std::ostream &stream, const std::vector<int> &colWidth) {
    int width = std::accumulate(colWidth.begin(), colWidth.end(), 0)
                + colWidth.size() * 2 - 1;
    stream << '|';
    stream.fill('-');
    stream << std::setw(width) << '-';
    stream.fill(' ');
    stream << '|' << std::endl;
}

void AMXProfiler::PrintStats(std::ostream &stream, AMX_DBG *dbg) const {
    std::vector<KeyValPair> v(functions_.begin(), functions_.end());
    std::sort(v.begin(), v.end(), ByExecutionTime);

    stream << std::setiosflags(std::ios::left);

    std::vector<int> colWidth;
    colWidth.push_back(33);
    colWidth.push_back(20);
    colWidth.push_back(10);

    PrintHorizLine(stream, colWidth);

    // Table header
    stream << "| " << std::setw(colWidth[0]) << "Function" 
              << "| " << std::setw(colWidth[1]) << "Number of calls" 
              << "| " << std::setw(colWidth[2]) << "Time, %" 
              << "|" << std::endl;

    PrintHorizLine(stream, colWidth);

    // Calculate overall execution time
    AMXFunctionProfile::IntervalType totalTime = 0;
    for (std::vector<KeyValPair>::iterator it = v.begin(); it != v.end(); ++it) {
        totalTime += it->second.GetExecutionTime();
    }

    for (std::vector<KeyValPair>::iterator it = v.begin(); it != v.end(); ++it)
    {
        if (dbg != 0) {
            const char *name;
            dbg_LookupFunction(dbg, it->first, &name);
            stream << "| " << std::setw(colWidth[0]) << name;
        } else {
            stream << "| " << std::setw(colWidth[0]) << std::hex << it->first << std::dec;
        }
        stream << "| " << std::setw(colWidth[1]) << it->second.GetNumberOfCalls()
               << "| " << std::setw(colWidth[2]) << std::setprecision(4)
               << it->second.GetExecutionTime() * 100.0 / totalTime
               << "|" << std::endl;
        PrintHorizLine(stream, colWidth);
    }
}
