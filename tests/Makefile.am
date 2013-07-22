###############################################################################
# Copyright (c) 2013      Los Alamos National Security, LLC.
#                         All rights reserved.
################################################################################

noinst_PROGRAMS = \
rebind \
trivial \
dist-work \
barrier-subset

if QUO_WITH_MPIFC
noinst_PROGRAMS += \
quofort
endif

### test 0
rebind_SOURCES = rebind.c
rebind_CPPFLAGS  =
rebind_LDFLAGS =
rebind_LDADD   = $(HWLOC_EMBEDDED_LDADD) $(HWLOC_EMBEDDED_LIBS)

### test 1
trivial_SOURCES = trivial.c
trivial_CFLAGS  = -I$(top_srcdir)/src
trivial_LDFLAGS = -L$(top_srcdir)/src
trivial_LDADD   = -lquo

### test 2
dist_work_SOURCES = dist-work.c
dist_work_CFLAGS  = -I$(top_srcdir)/src
dist_work_LDFLAGS = -L$(top_srcdir)/src
dist_work_LDADD   = -lquo

### test 3
barrier_subset_SOURCES = barrier-subset.c
barrier_subset_CFLAGS  =
barrier_subset_LDFLAGS =
barrier_subset_LDADD   =

### fortran interface test
if QUO_WITH_MPIFC
quofort_SOURCES = quofort.f90
quofort_FCFLAGS  = -I$(top_srcdir)/src
quofort_LDFLAGS = -L$(top_srcdir)/src
quofort_LDADD   = -lquo
endif
