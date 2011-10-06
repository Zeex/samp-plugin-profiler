CC  = gcc
CXX = g++

CPPFLAGS  = -DLINUX -Isrc/amx -I$(STLSOFT)/include -DHAVE_STDINT_H -DHAVE_ALLOCA_H
CFLAGS    = -c -Wall 
CXXFLAGS  = -c -Wall
LDFLAGS   = -shared -export-dynamic 

PLUGIN = profiler
OUTFILE = $(PLUGIN).so

OBJFILES =\
	src/amxnamefinder.o\
	src/amxplugin.o\
	src/amxprofiler.o\
	src/fileutils.o\
	src/jump.o\
	src/plugin.o\
	src/perfcounter.o\
	src/amx/amxaux.o\
	src/amx/amxdbg.o

.PHONY: all clean

all: $(OUTFILE)

$(OUTFILE): $(OBJFILES)
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	rm -vf *.o $(OUTFILE)
