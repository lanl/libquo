###############################################################################
# Copyright (c) 2013      Los Alamos National Security, LLC.
#                         All rights reserved.
################################################################################

noinst_PROGRAMS = \
rebind

if QUO_WITH_MPIFC
noinst_PROGRAMS += \
quofort
endif

### test 0
rebind_SOURCES = rebind.c
rebind_CPPFLAGS  =
rebind_LDFLAGS =
rebind_LDADD   = $(HWLOC_EMBEDDED_LDADD) $(HWLOC_EMBEDDED_LIBS)

### fortran interface test
if QUO_WITH_MPIFC
quofort_SOURCES = quofort.f90
quofort_FCFLAGS  = -I$(top_srcdir)/src
quofort_LDFLAGS = -L$(top_srcdir)/src
quofort_LDADD   = -lquo
endif
