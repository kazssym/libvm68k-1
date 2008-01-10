dnl Process this file with autoconf to produce a configure script.
AC_INIT([Virtual M68000 Toolkit], [1.1.0], [vx68k@googlegroups.com], [libvm68k])
AC_CONFIG_SRCDIR([processor.cpp])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_CXX
AC_USE_SYSTEM_EXTENSIONS

AC_C_CONST
AC_TYPE_SIZE_T
AC_CXX_EXCEPTIONS
AC_CXX_TEMPLATES
AC_CXX_NAMESPACES
AC_PROG_LIBTOOL
if test "$ac_cv_cxx_exceptions" != yes; then
  AC_MSG_WARN([the C++ compiler does not support exceptions])
  AC_MSG_WARN([this program cannot be built without exceptions])
fi

if test "$ac_cv_cxx_templates" != yes; then
  AC_MSG_WARN([the C++ compiler does not support templates])
  AC_MSG_WARN([this program cannot be built without templates])
fi

if test "$ac_cv_cxx_namespaces" != yes; then
  AC_MSG_WARN([the C++ compiler does not implement namespaces])
  AC_MSG_WARN([this program cannot be built without namespaces])
fi

AC_CONFIG_FILES([Makefile doc/Makefile])
AC_CONFIG_HEADERS([include/config.h])
AC_OUTPUT
