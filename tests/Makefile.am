###############################################################################
# Copyright (c) 2013      Los Alamos National Security, LLC.
#                         All rights reserved.
################################################################################

bin_PROGRAMS = \
quoinfo \
quodemo \
quodemo-multilib \
rebind \
quofort

noinst_LTLIBRARIES = \
libdemop1.la

### demo 0
quoinfo_SOURCES = quoinfo.c
quoinfo_CPPFLAGS  = -I$(top_srcdir)/src
quoinfo_LDFLAGS = -L$(top_srcdir)/src
quoinfo_LDADD   = -lquo

### demo 1
quodemo_SOURCES = quodemo.c
quodemo_CPPFLAGS  = -I$(top_srcdir)/src
quodemo_LDFLAGS = -L$(top_srcdir)/src
quodemo_LDADD   = -lquo

### demo 2 (multi lib example)
quodemo_multilib_SOURCES = quodemo-multilib-common.h quodemo-p0.c
quodemo_multilib_CPPFLAGS  = -I$(top_srcdir)/src
quodemo_multilib_LDFLAGS = -L$(top_srcdir)/src
quodemo_multilib_LDADD   = -lquo -ldemop1

libdemop1_la_SOURCES = quodemo-multilib-common.h quodemo-p1.h quodemo-p1.c
libdemop1_la_CPPFLAGS = -I$(top_srcdir)/src
libdemop1_la_LDFLAGS = -L$(top_srcdir)/src
libdemop1_la_LIBADD = -lquo

### test 0
rebind_SOURCES = rebind.c
rebind_CPPFLAGS  = -I$(top_srcdir)/src
rebind_LDFLAGS = -L$(top_srcdir)/src
rebind_LDADD   = -lquo

### fortran demo
quofort_SOURCES = quofort.f90
quofort_FCFLAGS  = -I$(top_srcdir)/src
quofort_LDFLAGS = -L$(top_srcdir)/src
quofort_LDADD   = -lquo

