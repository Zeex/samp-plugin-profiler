lib_LTLIBRARIES = profiler.la

profiler_la_CPPFLAGS = -DLINUX -Iamx
profiler_la_LDFLAGS = -avoid-version -module -shared

profiler_la_SOURCES =\
	amx/amxaux.c\
	amx/amxdbg.c\
	amxnamefinder.cpp\
	amxplugin.cpp\
	fileutils.cpp\
	jump.cpp\
	profiler.cpp
