## Process this file with automake to produce a Makefile.in.

LIBS =

lib_LTLIBRARIES = libvm68k.la

libvm68k_la_LDFLAGS = $(LTLIBRELEASE) -version-info 4:0:0
libvm68k_la_SOURCES = processor.cc instr0.cc instr1-3.cc instr4.cc inst5.cc \
inst6.cc inst7.cc inst8.cc inst9.cc inst11.cc inst12.cc inst13.cc inst14.cc \
context.cc condition_code.cc memory_map.cc memory.cc version.cc

noinst_HEADERS = instr.h

info_TEXINFOS = libvm68k.texi

EXTRA_DIST = ChangeLog.vx68k NEWS.vx68k

SUBDIRS = vm68k
