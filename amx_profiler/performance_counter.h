// Copyright (c) 2011-2013, Zeex
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef AMX_PROFILER_PERFORMANCE_COUNTER_H
#define AMX_PROFILER_PERFORMANCE_COUNTER_H

#include <chrono>

namespace amx_profiler {

class PerformanceCounter {
public:
	PerformanceCounter(PerformanceCounter *parent = nullptr);
	~PerformanceCounter();

	void Start();
	void Stop();

	inline std::chrono::high_resolution_clock::duration child_time() const
		{ return child_time_; }

	inline std::chrono::high_resolution_clock::duration total_time() const
		{ return total_time_; }

	inline std::chrono::high_resolution_clock::duration GetSelfTime() const
		{ return total_time() - child_time(); }

private:
	bool started_;
	PerformanceCounter *parent_;
	std::chrono::high_resolution_clock::time_point start_point_;
	std::chrono::high_resolution_clock::duration child_time_;
	std::chrono::high_resolution_clock::duration total_time_;
};

} // namespace amx_profiler

#endif // !AMX_PROFILER_PERFORMANCE_COUNTER_H
