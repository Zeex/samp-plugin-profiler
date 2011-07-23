// SA:MP profiler plugin v1.5
// Copyright (C) 2011 Zeex

enum ProfilerStatsOrder {
	PROFILER_ORDER_NONE,
	PROFILER_ORDER_BY_CALLS,
	PROFILER_ORDER_BY_TIME,
	PROFILER_ORDER_BY_TIME_PER_CALL
};

// Must be called before any other Profiler_* function.
// It needs a path to the compiled .amx in order to load debug info from it.
// Sample paths: gamemodes/lvdm.amx, filterscripts/debugfs.amx, etc.
native Profiler_Init(const path_to_amx[]);

// This function starts a new profiling session.
native Profiler_Start();

// Stops prifiling.
native Profiler_Stop();

// Resets gathered statistics.
native Profiler_ResetStats();

// Saves perfromance statistics for each function in as an HTML table to the specified file.
// Should not be called when profler is running.
native Profiler_PrintStats(const filename[], ProfilerStatsOrder:order);
