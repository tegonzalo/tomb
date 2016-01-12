#
# makefile
# Created by T.Gonzalo on 05/08/2013.
# Last modified on 21/10/2015
#

# COMPILER
CC = g++

# MAIN
MAIN = group
MATHMAIN = mathgroup

# INCLUDES AND DEPENDENCIES
IDIR = ./include
INCS = -I$(IDIR)
DEPENDENCIES = $(shell find $(IDIR) -type f -name *.h)

# LIBRARIES
LIBS = -lm

# FLAGS
# CFLAGS = -Wall -g -I$(IDIR)
CFLAGS = -w -g -std=c++11

# SOURCE FILES
SDIR = ./src
#SOURCES = simplegroup.cc liegroup.cc subgroup.cc irrep.cc rrep.cc root.cc weigth.cc
SOURCES = $(shell find $(SDIR) -type f -name *.cc)

# OBJECT FILES
ODIR = ./build
OBJECTS := $(patsubst $(SDIR)/%,$(ODIR)/%,$(SOURCES:.cc=.o))

# JSON
JSONDIR = ./libjson/_internal/Source
JSONOBJECTS  = internalJSONNode.o JSONAllocator.o JSONChildren.o \
                 JSONDebug.o JSONIterators.o JSONMemory.o JSONNode.o \
                 JSONNode_Mutex.o JSONPreparse.o JSONStream.o JSONValidator.o \
                 JSONWorker.o JSONWriter.o libjson.o
JSOBJ = $(patsubst %,$(JSONDIR)/%,$(JSONOBJECTS))
CFLAGS := -DNDEBUG $(CFLAGS)
INCS := $(INCS) -I./libjson

# MATHLINK
MLDIR = ./mlink/
MATHINCS = $(INCS) -I./mathlink-linux/
MATHLIBS = $(LIBS) -L./mathlink-linux/ -lML64i4 -lm -lpthread -lrt -lstdc++ -ldl -luuid
#-lMLi3 -lstdc++.6 -framework Foundation
MLOBJECTS = $(filter-out $(ODIR)/$(MAIN).o,$(OBJECTS))
MPREP = ./mathlink-linux/mprep
MCC = mcc
MLOBJ = $(ODIR)/$(MATHMAIN)tm.o

# TARGETS
all: $(MAIN) $(MATHMAIN) 

$(MAIN): $(OBJECTS) $(JSOBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(INCS) $(LIBS)
	
$(MATHMAIN): $(ODIR)/$(MATHMAIN).o $(MLOBJECTS) $(JSOBJ) $(MLOBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(MATHINCS) $(MATHLIBS)

$(ODIR)/%.o: $(SDIR)/%.cc $(DEPENDENCIES)
	$(CC) -c -o $@ $< $(CFLAGS) $(INCS)

$(JSONDIR)/%.o: $(JSONDIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(MLOBJ): $(MLDIR)/$(MATHMAIN).tm
	$(MPREP) $? -o $(ODIR)/$(MATHMAIN)tm.cc
	$(CC) -c -o $@ $(ODIR)/$(MATHMAIN)tm.cc $(CFLAGS) $(MATHINCS)

$(ODIR)/$(MATHMAIN).o: $(MLDIR)/$(MATHMAIN).cc $(DEPENDENCIES)
	$(CC) -c -o $@ $< $(CFLAGS) $(MATHINCS)

.PHONY: clean

clean:
	rm -rf $(ODIR)/*  *~ core $(IDIR)/*~ $(MAIN)
	rm -rf $(ODIR)/*  *~ core $(IDIR)/*~ $(MATHMAIN)
	rm -rf $(JSONDIR)/*.o $(JSONDIR)/*~
	rm -rf $(MLDIR)/$(MATHMAIN)tm.cc
	rm -rf $(MLDIR)/$(MATHMAIN)tm.o
	rm -rf $(MLDIR)/$(MATHMAIN).o
