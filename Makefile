CC   = gcc
CXX  = g++
CCAS = yasm
RM   = rm -vf

CPPFLAGS  = -DLINUX -Isrc/amx -I$(STLSOFT)/include -DHAVE_STDINT_H -DHAVE_ALLOCA_H
CFLAGS    = -c -Wall 
CXXFLAGS  = $(CFLAGS)
CCASFLAGS = -f elf32
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
	src/amx/amxaux.o\
	src/amx/amxdbg.o

.PHONY: all clean

all: $(OUTFILE)

.SUFFIXES: .o .c .cpp .asm

$(OUTFILE): $(OBJFILES)
	$(CXX) $(LDFLAGS) -o $@ $^

.asm.o:
	$(CCAS) $(CCASFLAGS) -o $@ $<

clean:
	$(RM) *.o $(OUTFILE)
