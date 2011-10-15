#ifndef PERFCOUNTER_H
#define PERFCOUNTER_H

#include <platformstl/platformstl.hpp>
#include <platformstl/performance/performance_counter.hpp>

class PerformanceCounter {
public:
	PerformanceCounter();
	~PerformanceCounter();

	void Start(PerformanceCounter *parent = 0);
	void Stop();

	platformstl::int64_t GetCalls() const;
	platformstl::int64_t GetTime() const;

private:
	void Pause();
	void Resume();

	bool started_;
	bool paused_;

	PerformanceCounter *child_;
	PerformanceCounter *parent_;

	platformstl::int64_t calls_;
	platformstl::performance_counter counter_;
	platformstl::performance_counter::interval_type time_;
};

#endif