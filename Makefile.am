## Process this file with automake to produce Makefile.in
     
bin_PROGRAMS=gsmc
gsmc_SOURCES= autotune.c calc.c dataconv.c draw.c main.c print.c widget.c autotune.h calc.h dataconv.h draw.h main.h print.h widget.h
gsmc_CFLAGS = $(AM_CFLAGS) $(GTK_CFLAGS)
gsmc_LDADD = $(AM_LDADD) $(GTK_LIBS) -lm

EXTRA_DIST=gsmc.1.in gtkrc TODO
man_MANS=gsmc.1
