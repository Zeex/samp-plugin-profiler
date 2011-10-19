#include <iostream>

#include "perfcounter.h"

PerformanceCounter::PerformanceCounter() 
	: started_(false)
	, paused_(false)
	, child_(0)
	, parent_(0)
	, calls_(0)
	, counter_()
	, time_(0)  
{
}

PerformanceCounter::~PerformanceCounter() {
	Stop();
}

void PerformanceCounter::Start(PerformanceCounter *parent) {
	if (!started_) {	
		if (parent != 0) {
			parent->child_ = this;
			parent->Pause();
		}	
		parent_ = parent;

		++calls_;
		counter_.start();

		started_ = true;		
	}
}

void PerformanceCounter::Stop() {
	if (started_) {
		counter_.stop();
		time_ += counter_.get_microseconds();

		if (child_ != 0) {
			child_->Stop();
			child_ = 0;
		}

		if (parent_ != 0) {
			parent_->child_ = 0;
			parent_->Resume();
			parent_ = 0;
		}

		started_ = false;
	}
}

void PerformanceCounter::Pause() {
	if (!paused_) {
		counter_.stop();
		time_ += counter_.get_microseconds();
		paused_ = true;
	}
}

void PerformanceCounter::Resume() {
	if (paused_) {
		counter_.start();
		paused_ = false;
	}
}

platformstl::int64_t PerformanceCounter::GetCalls() const {
	return calls_;
}

platformstl::int64_t PerformanceCounter::GetTime() const {
	return time_;
}