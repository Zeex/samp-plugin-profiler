#ifndef AMXPROFILER_H
#define AMXPROFILER_H

#include <iostream>
#include <map>
#include <stack>
#include <string>

#include <platformstl/performance/performance_counter.hpp>

#include <amx/amx.h>
#include <amx/amxdbg.h>

class AMXFunctionProfile {
public:
    typedef platformstl::performance_counter Counter;
    typedef Counter::interval_type IntervalType;

    AMXFunctionProfile() : calls_(0), time_(0) {}

    // Time is measured in microseconds
    IntervalType GetExecutionTime() const;

    IntervalType GetNumberOfCalls() const;

private:
    friend class AMXProfiler;

    // These methods are only used by AMXProfiler
    void StartCounter();
    void StopCounter();
    void IncreaseCalls();

private:
    long calls_;
    Counter counter_;
    IntervalType time_;
};

class AMXProfiler {
public:
    explicit AMXProfiler(AMX *amx);
    ~AMXProfiler();

    // Starts a new profiling session
    void Run();

    bool IsRunning() const;

    // Stops profiling
    void Terminate();

    // Prints performance statistics for each function
    // Also can print function names if debug info provided
    void PrintStats(std::ostream &outStream, AMX_DBG *dbg = 0) const;

    int DebugHook();

private:
    // Default ctor
    AMXProfiler();

private:
    AMX *amx_;
    bool running_;

    // Previously set debug hook, if any
    AMX_DEBUG debugHook_;

    // To keep track of stack frame change
    cell currentStackFrame_;

    // Per-function data
    std::map<cell, AMXFunctionProfile> functions_;
};

#endif