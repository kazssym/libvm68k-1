## Process this file with automake to produce a Makefile.in.

LIBS =

lib_LTLIBRARIES = libvm68k.la

libvm68k_la_LDFLAGS = $(LTLIBRELEASE) -version-info 4:0:0
libvm68k_la_SOURCES = version.cc bus_target.cc bus.cc \
processor.cc instr0.cc instr1-3.cc \
instr4.cc instr5.cc instr6.cc instr7.cc instr8.cc \
instr9.cc instr11.cc instr12.cc instr13.cc instr14.cc \
context.cc condition_code.cc

noinst_HEADERS = instr.h

info_TEXINFOS = libvm68k.texi

EXTRA_DIST = ChangeLog.vx68k NEWS.vx68k

SUBDIRS = vm68k
