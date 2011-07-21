// SA:MP profiler plugin
// Copyright (C) 2011 Zeex

// This function starts a new profiling session.
// It needs a path to the compiled .amx in order to load debug info from it.
// Sample paths: gamemodes/lvdm.amx, filterscripts/debugfs.amx, etc.
native Profiler_Start(const path_to_amx[]);

// Stops prifiling.
native Profiler_Stop();

// Saves perfromance statistics for each function in as an HTML table to the specified file.
// Should not be called when profler is running.
native Profiler_PrintStats(const filename[]);
