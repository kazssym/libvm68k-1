## Process this file with automake to produce a Makefile.in.

lib_LTLIBRARIES = libvm68k.la

libvm68k_la_LDFLAGS = -release 1.1 -version-info 0:0:0
libvm68k_la_SOURCES = bus.cpp size.cpp context.cpp processor.cpp \
	inst/inst0.cpp inst/inst1.cpp inst/inst2.cpp inst/inst3.cpp \
	inst4.cpp instr5.cpp instr6.cpp instr7.cpp instr8.cpp \
	instr9.cpp instr11.cpp instr12.cpp instr13.cpp instr14.cpp \
	condition_code.cpp
libvm68k_la_LIBADD = @LIBLTDL@

nobase_include_HEADERS = vm68k/bits/base.h vm68k/bits/bus.h \
	vm68k/bits/data_size.h vm68k/bits/context.h vm68k/bits/processor.h \
	vm68k/bus vm68k/data_size vm68k/context vm68k/processor
nobase_noinst_HEADERS = inst/addressing.h \
	inst/transfer.h inst/monadic.h inst/arith.h inst/logic.h \
	inst/bit.h
