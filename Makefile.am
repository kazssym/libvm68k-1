## Process this file with automake to produce a Makefile.in.

EXTRA_DIST = ChangeLog.vx68k NEWS.vx68k

SUBDIRS = @subdirs@ doc lib

dist-hook:
	cd $(distdir); \
find . ! -name \*SUMS -type f -print | sed 's,^\./,,' | sort | xargs sha1sum > SHA1SUMS \
|| rm -f SHA1SUMS
	if test -f $(srcdir)/sign-dist.sh; then \
  $(SHELL) $(srcdir)/sign-dist.sh $(distdir); \
fi
	if test -f $(srcdir)/export-dist.sh; then \
  $(SHELL) $(srcdir)/export-dist.sh $(distdir) $(PACKAGE); \
fi
