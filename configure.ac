#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(gsmc, 1.2, ik5nax@radioteknos.it)
AC_CONFIG_SRCDIR([calc.c])
AM_INIT_AUTOMAKE(gsmc, 1.2, ik5nax@radioteknos.it)

# Checks for programs.
#AM_PROG_CC_C_O
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

# Checks for libraries.
PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([GTK], [gtk+-2.0])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_CHECK_FUNCS([sqrt])
AC_OUTPUT([Makefile gsmc.1])

