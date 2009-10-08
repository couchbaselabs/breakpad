CXX=g++
CC=gcc

CPPFLAGS=-DNDEBUG
CXXFLAGS=-g3 -O2 -Wall -m32


# These flags are necessary to compile correctly; the 'override'
# command makes sure these are added even if the user sets CPPFLAGS on
# the command line.
override CPPFLAGS+=-I../../.. -D_REENTRANT

.PHONY:all clean

BIN=dump_syms

all:$(BIN)

TOP = ../../../..
VPATH  = $(TOP)/src/common/linux
VPATH += $(TOP)/src/common
VPATH += $(TOP)/src/common/dwarf

DUMP_OBJ =					\
	dump_symbols.o				\
	dump_syms.o				\
	dwarf2diehandler.o			\
	file_id.o				\
	guid_creator.o				\
	md5.o					\
	module.o				\
	stabs_reader.o				\
	$(NULL)

dump_syms:$(DUMP_OBJ)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^

# From this directory:
dump_syms.o: dump_syms.cc

# From src/common/linux:
dump_symbols.o: dump_symbols.cc
stabs_reader.o: stabs_reader.cc
module.o: module.cc
guid_creator.o: guid_creator.cc
file_id.o: file_id.cc

# From src/common:
md5.o: md5.c

# From src/common/dwarf:
dwarf2diehandler.o: dwarf2diehandler.cc

clean:
	rm -f $(BIN) $(DUMP_OBJ)
