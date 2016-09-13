#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_CONFIG_HEADERS([config.h])
AC_PREREQ(2.57)

AC_INIT(gsmc, __VERSION, ik5nax@radioteknos.it)
AC_CONFIG_SRCDIR([calc.c])
AM_INIT_AUTOMAKE

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

AC_DEFINE(__VERSION, "1.2.1", "current version")
AC_DEFINE(__VERSION_DATE, "06/09/2016", "current version date")

AC_OUTPUT([Makefile gsmc.1])

