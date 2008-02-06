## Process this file with automake to produce a Makefile.in.

lib_LTLIBRARIES = libvm68k.la

libvm68k_la_LDFLAGS = -release 1.1 -version-info 0:0:0
libvm68k_la_SOURCES = bus.cpp context.cpp size.cpp processor.cpp \
	instr0.cpp instr1_3.cpp \
	instr4.cpp instr5.cpp instr6.cpp instr7.cpp instr8.cpp \
	instr9.cpp instr11.cpp instr12.cpp instr13.cpp instr14.cpp \
	condition_code.cpp
libvm68k_la_LIBADD = @LIBLTDL@

nobase_include_HEADERS = vm68k/base vm68k/bus vm68k/context vm68k/size
noinst_HEADERS = addressing.h \
	inst_xfer.h inst_arith.h inst_logical.h inst_bit.h instr.h

EXTRA_DIST = ChangeLog.vx68k NEWS.vx68k

SUBDIRS = @subdirs@ doc

dist-hook:
	cd $(distdir); \
find . ! -name \*SUMS -type f -print | sed 's,^\./,,' | sort | xargs sha1sum > SHA1SUMS \
|| rm -f SHA1SUMS
	if test -f $(srcdir)/sign-distdir.sh; then \
  $(SHELL) $(srcdir)/sign-distdir.sh $(distdir); \
fi
