#
# simple Makefile for a Zune gui
#
# Copyright 2014 Oliver Brunner - aros<at>oliver-brunner.de
#
# This file is part of Janus-UAE.
#
# Janus-UAE is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Janus-UAE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Janus-UAE. If not, see <http://www.gnu.org/licenses/>.
#

# to build for 
# AROS ABI v0: ABI=v0
# AROS ABI v1: ABI=v1
#ABI        = v0

BITS := $(shell getconf LONG_BIT)
ifeq ($(BITS),64)
	# 64 bit
	CPU      = x86_64
	# AROS source svn
	SVN        = /home/oli/tc/svn
else
	# 32 bit
	CPU      = i386
	SVN        = /home/oli
endif

LIB        = libgui.a

CC         = $(SVN)/$(ABI)/AROS/bin/linux-$(CPU)/tools/crosstools/$(CPU)-aros-gcc
CPP        = $(SVN)/$(ABI)/AROS/bin/linux-$(CPU)/tools/crosstools/$(CPU)-aros-g++
#RANLIB = 
AR         = $(SVN)/$(ABI)/AROS/bin/linux-$(CPU)/tools/crosstools/$(CPU)-aros-ar
LD         = $(SVN)/$(ABI)/AROS/bin/linux-$(CPU)/tools/crosstools/$(CPU)-aros-ld

# suppress warning: deprecated conversion from string constant to ‘char*’
FLAGS      = -Wno-write-strings -O2 #-Wall
ARFLAGS    = rc
DEBUGFLAGS = -g

CDEFS   = -DUAE_ABI_$(ABI)
INCLUDES= -I. -I.. -I../../include -I../../md-i386-gcc  -I$(SVN)/$(ABI)/AROS/bin/linux-$(CPU)/gen/include/SDL

CFLAGS = $(CDEFS) $(DEBUGFLAGS) $(FLAGS) $(INCLUDES)
CPPFLAGS = $(CDEFS) $(DEBUGFLAGS) $(FLAGS) $(INCLUDES)

LINK_LIBS=-larossupport -lmui

# ============== common ====================

COMMON_OBJS= \
	mui_head.o \
	mui_data.o \
	mui_file_request.o \
	mui_tree.o \
	mui_messagebox.o \
	mui_win.o \
	mui_listview.o \
	mui_dialog.o \
	combo.o \
	fs.o \
	cursor.o \
	mui_class.o 

# build dependencies magic
DEP_OBJS=$(COMMON_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $*.c

%.o: %.cpp
	@echo "building dependency for "$(subst .o,.cpp,$<)" .."
	@$(CPP) -M $(CPPFLAGS) $(subst .o,.cpp,$<) > $@.$$$$;                      \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $(subst .o,.dep,$@);     \
	rm -f $@.$$$$
	$(CPP) $(CPPFLAGS) -o $@ -c $*.cpp


all: uae 

uae: tools mui_data.h common 
	$(AR) $(ARFLAGS) $(LIB) $(COMMON_OBJS) 


common: $(COMMON_OBJS)
	@echo building common ..

aros: $(AROS_OBJS)
	@echo building aros objects ..

tools:
	@echo builing tools ..
	cd png2c;export CC=gcc;make

	
# ============== autogenerate some .cpp files ====================
# never know, which one needs to be generated first. But only one
# is actually executed as the other condition is already fulfilled.
mui_data.cpp: ../../od-win32/resources/winuae.rc parse_fixed.pl
	./parse_fixed.pl ../../od-win32/resources/winuae.rc mui_data.cpp

mui_data.h: ../../od-win32/resources/winuae.rc parse_fixed.pl
	./parse_fixed.pl ../../od-win32/resources/winuae.rc mui_data.cpp


# ============== clean ====================
clean: 
	rm -f $(COMMON_OBJS) $(LIB)
	cd png2c;make clean


# ============== dependencies ====================

-include $(subst .o,.dep,$(DEP_OBJS))

