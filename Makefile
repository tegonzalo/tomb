#
# makefile
# Created by T.Gonzalo on 05/08/2013.
# Last modified on 21/10/2015
#

# COMPILER
CC = g++

# MAIN
TOMBMAIN = tomb
GROUPMAIN = group
MATHMAIN = mathgroup
MAINS = $(TOMBMAIN) $(GROUPMAIN) $(MATHMAIN)

# INCLUDES AND DEPENDENCIES
IDIR = ./include
INCS = -I$(IDIR)
DEPENDENCIES = $(shell find $(IDIR) -type f -name *.h)

# LIBRARIES
LIBS = -lm

# FLAGS
# CFLAGS = -Wall -g -I$(IDIR)
CFLAGS = -w -g -std=c++11 -fopenmp

# SOURCE FILES
SDIR = ./src
SOURCES = $(shell find $(SDIR) -type f -name *.cc)

# OBJECT FILES
ODIR = ./build
OBJECTS := $(patsubst $(SDIR)/%,$(ODIR)/%,$(SOURCES:.cc=.o))
OBJS = $(filter-out $(patsubst %,$(ODIR)/%.o, $(MAINS)),$(OBJECTS))

# JSON
JSONDIR = ./external/libjson/_internal/Source
JSONOBJECTS  = internalJSONNode.o JSONAllocator.o JSONChildren.o \
                 JSONDebug.o JSONIterators.o JSONMemory.o JSONNode.o \
                 JSONNode_Mutex.o JSONPreparse.o JSONStream.o JSONValidator.o \
                 JSONWorker.o JSONWriter.o libjson.o
JSOBJ = $(patsubst %,$(JSONDIR)/%,$(JSONOBJECTS))
CFLAGS := -DNDEBUG $(CFLAGS)
INCS := $(INCS) -I./external/libjson

# MATHLINK
MLDIR = ./mlink/
MATHINCS = $(INCS) -I./external/mathlink-linux/
MATHLIBS = $(LIBS) -L./external/mathlink-linux/ -lML64i4 -lm -lpthread -lrt -lstdc++ -ldl -luuid
#-lMLi3 -lstdc++.6 -framework Foundation
MPREP = ./mathlink-linux/mprep
MCC = mcc
MLOBJ = $(ODIR)/$(MATHMAIN)tm.o

# TARGETS
#all: $(MAINS) 

#$(TOMBMAIN): $(ODIR)/$(TOMBMAIN).o $(OBJS) $(JSOBJ)
#	$(CC) -o $@ $^ $(CFLAGS) $(INCS) $(LIBS)
	
#$(GROUPMAIN):  $(ODIR)/$(GROUPMAIN).o $(OBJS) $(JSOBJ)
#	$(CC) -o $@ $^ $(CFLAGS) $(INCS) $(LIBS)
	
#$(MATHMAIN): $(ODIR)/$(MATHMAIN).o $(OBJS) $(JSOBJ) $(MLOBJ)
#	$(CC) -o $@ $^ $(CFLAGS) $(MATHINCS) $(MATHLIBS)

$(ODIR)/%.o: $(SDIR)/%.cc $(DEPENDENCIES)
	$(CC) -c -o $@ $< $(CFLAGS) $(INCS)

$(JSONDIR)/%.o: $(JSONDIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

#$(MLOBJ): $(SDIR)/$(MATHMAIN).tm
#	$(MPREP) $? -o $(ODIR)/$(MATHMAIN)tm.cc
#	$(CC) -c -o $@ $(ODIR)/$(MATHMAIN)tm.cc $(CFLAGS) $(MATHINCS)

#$(ODIR)/$(MATHMAIN).o: $(SDIR)/$(MATHMAIN).cc $(DEPENDENCIES)
#	$(CC) -c -o $@ $< $(CFLAGS) $(MATHINCS)

group: $(ODIR)/group.o $(ODIR)/files.o $(ODIR)/simplegroup.o $(ODIR)/root.o $(ODIR)/liegroup.o $(ODIR)/helper_functions.o $(ODIR)/weight.o $(ODIR)/irrep.o $(JSOBJ) $(ODIR)/rrep.o $(ODIR)/subgroup.o
	$(CC) -o $@ $^ $(CFLAGS) $(INCS) $(LIBS)


.PHONY: clean

clean:
	rm -rf $(ODIR)/*
	rm -rf *~ core $(IDIR)/*~ 
	rm -rf $(TOMBMAIN) $(GROUPMAIN) $(MATHMAIN)
	rm -rf $(JSONDIR)/*.o $(JSONDIR)/*~
	rm -rf $(MLDIR)/$(MATHMAIN)tm.cc
	rm -rf $(MLDIR)/$(MATHMAIN)tm.o
	rm -rf $(MLDIR)/$(MATHMAIN).o
