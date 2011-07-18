// SA:MP profiler plugin
// Copyright (C) 2011 Zeex

// Start performance profiling.
native Profiler_Start();

// Stop prifiling and write statistics to "<server_root>/profiler.txt".
native Profiler_Stop();

// Set current script's filename (e.g. "gamemodes/lvdm.amx").
// Call this somehwere before Profiler_Stop() if you want
// function names instead addresses in the stats table.
native Profiler_SetScriptName(const name[]);
