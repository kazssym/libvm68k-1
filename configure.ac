dnl Process this file with autoconf to produce a configure script.
AC_INIT([Libvm68k], [1.0.6], [vx68k@lists.hypercore.co.jp], [libvm68k])
AC_CONFIG_SRCDIR([processor.cpp])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
LDRELEASE="-release 1.0"
AC_SUBST([LDRELEASE])dnl
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_ARG_WITH([nana],
[  --with-nana             use GNU Nana for testing [no]],, [with_nana=no])
if test "$with_nana" != no; then
  AC_CHECK_LIB([nana], [_I_default_handler])
  AC_CHECK_HEADERS([nana.h])
fi

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

AC_CONFIG_FILES([Makefile vm68k/Makefile])
AC_OUTPUT
