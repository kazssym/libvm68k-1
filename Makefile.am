## Process this file with automake to produce a Makefile.in.

LIBS =

lib_LTLIBRARIES = libvm68k.la

libvm68k_la_LDFLAGS = $(LTLIBRELEASE) -version-info 4:0:0
libvm68k_la_SOURCES = processor.cpp instr0.cpp instr1_3.cpp \
	instr4.cpp instr5.cpp instr6.cpp instr7.cpp instr8.cpp \
	instr9.cpp instr11.cpp instr12.cpp instr13.cpp instr14.cpp \
	context.cpp condition_code.cpp \
	bus.cpp bus_target.cpp version.cpp

noinst_HEADERS = instr.h

info_TEXINFOS = libvm68k.texi

EXTRA_DIST = ChangeLog.vx68k NEWS.vx68k

SUBDIRS = vm68k
