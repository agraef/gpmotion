# -*- mode: makefile-gmake -*-

# This needs GNU make.

# You may want to set these if you have the Pd include files in a
# non-standard location, and/or want to install the external in a
# custom directory.

#PDINCLUDEDIR = /usr/include/pd
#PDLIBDIR = /usr/lib/pd/extra

# No need to edit anything below this line, usually.

lib.name = gpmotion

gpmotion_version := $(shell git describe --tags 2>/dev/null)

gpmotion.class.sources := gpmotion.c motion.cpp
gpmotion.class.ldlibs := -lm

datafiles = COPYING $(wildcard gpmotion*-help.pd)

PDLIBBUILDER_DIR=.
include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder
