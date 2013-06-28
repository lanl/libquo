###############################################################################
# Copyright (c) 2013      Los Alamos National Security, LLC.
#                         All rights reserved.
################################################################################

bin_PROGRAMS = \
rebind \
quofort

### test 0
rebind_SOURCES = rebind.c
rebind_CPPFLAGS  =
rebind_LDFLAGS =
rebind_LDADD   = $(HWLOC_EMBEDDED_LDADD) $(HWLOC_EMBEDDED_LIBS)

### fortran interface test
quofort_SOURCES = quofort.f90
quofort_FCFLAGS  = -I$(top_srcdir)/src
quofort_LDFLAGS = -L$(top_srcdir)/src
quofort_LDADD   = -lquo
