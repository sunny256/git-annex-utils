#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([git-annex-utils], [0.01], [git-annex-utils-bugs@mysteryvortex.com])
AM_INIT_AUTOMAKE([dist-zip dist-bzip2 subdir-objects])
AC_CONFIG_SRCDIR([src/gadu-src/gadu.c])
AC_CONFIG_HEADERS([config.h])

#do checks with c++ preproc/compiler/etc...
AC_LANG([C])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O #required for subdir-objects
#AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# AutoTroll with Qt.
#AT_WITH_QT

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS_ONCE([getopt.h])
#AC_CHECK_HEADERS([])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS_ONCE([getopt_long])

#set the default install dir to ./inst
#AC_PREFIX_DEFAULT([`pwd`/inst])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT