#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(gsmc, 1.0, ik5nax@amsat.org)
AC_CONFIG_SRCDIR([calc.c])
AM_INIT_AUTOMAKE(gsmc, 1.0, ik5nax@amsat.org)

# Checks for programs.
AC_PROG_CC
pkg_modules="gtk+-2.0"
PKG_CHECK_MODULES(PACKAGE,[$pkg_modules])
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

# Checks for libraries.
LIBS=" "
PKG_LIB2=`pkg-config --libs gtk+-2.0`
PKG_LIB=""
for i in $PKG_LIB2 ; do
        case $i in 
                -l*) PKG_LIB="$i $PKG_LIB" ;;
                -L*) LIBS="$i $LIBS" ;;
        esac
done
for i in $PKG_LIB ; do
        j=${i#'-l'}
        AC_CHECK_LIB($j,main)
done

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_CHECK_FUNCS([sqrt])
AC_OUTPUT([Makefile gsmc.1])

