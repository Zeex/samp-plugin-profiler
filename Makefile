CC  = gcc
CXX = g++

CPPFLAGS  = -DLINUX -Isrc/amx -I$(STLSOFT)/include -DHAVE_STDINT_H -DHAVE_ALLOCA_H
CFLAGS    = -c -Wall -m32
CXXFLAGS  = -c -Wall -m32
LDFLAGS   = -shared -export-dynamic -static-libstdc++

PLUGIN = profiler
OUTFILE = $(PLUGIN).so

OBJFILES =\
	src/amxname.o\
	src/amxplugin.o\
	src/fileutils.o\
	src/jump.o\
	src/logprintf.o\
	src/plugin.o\
	src/profiler.o\
	src/perfcounter.o\
	src/amx/amxaux.o\
	src/amx/amxdbg.o

.PHONY: all clean

all: $(OUTFILE)

$(OUTFILE): $(OBJFILES)
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	rm -vf $(OBJFILES) $(OUTFILE)
