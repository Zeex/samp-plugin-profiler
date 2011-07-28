lib_LTLIBRARIES = profiler.la

profiler_la_CPPFLAGS = -DLINUX -Isrc/amx
profiler_la_LDFLAGS = -avoid-version -module -shared

profiler_la_SOURCES =\
	src/amx/amxaux.c\
	src/amx/amxdbg.c\
	src/amxnamefinder.cpp\
	src/amxplugin.cpp\
	src/amxprofiler.cpp\
	src/fileutils.cpp\
	src/jump.cpp\
	src/plugin.cpp
