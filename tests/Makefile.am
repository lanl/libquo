#
# Copyright (c) 2013-2019 Triad National Security, LLC
#                         All rights reserved.
#
# This file is part of the libquo project. See the LICENSE file at the
# top-level directory of this distribution.
#

noinst_PROGRAMS = \
rebind \
trivial \
dist-work \
barrier-subset \
quo-time

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
trivial_LDADD   = $(top_builddir)/src/libquo.la

### test 2
dist_work_SOURCES = dist-work.c
dist_work_CFLAGS  = -I$(top_srcdir)/src
dist_work_LDADD   = $(top_builddir)/src/libquo.la

### test 3
barrier_subset_SOURCES = barrier-subset.c
barrier_subset_CFLAGS  =
barrier_subset_LDFLAGS =
barrier_subset_LDADD   =

### test 4 (times quo operations)
quo_time_SOURCES = quo-time.c
quo_time_CFLAGS  = -I$(top_srcdir)/src
quo_time_LDADD   = $(top_builddir)/src/libquo.la

################################################################################
# Fortran Tests
################################################################################
### Minimal Fortran test.
if QUO_WITH_MPIFC
quofort_SOURCES = quofort.f90
quofort_FCFLAGS  = -I$(top_builddir)/src/fortran
quofort_LDADD = \
$(top_builddir)/src/fortran/libquo-usequo.la \
$(top_builddir)/src/libquo.la
endif

################################################################################
# XPM Tests
################################################################################
if QUO_WITH_XPM
noinst_PROGRAMS += \
xpm-0

xpm_0_SOURCES = \
xpm-0.c
xpm_0_CFLAGS  = \
-I$(top_srcdir)/src \
-I$(top_srcdir)/src/xpm
xpm_0_LDADD   = \
$(top_builddir)/src/xpm/libquo-xpm.la \
$(top_builddir)/src/libquo.la
endif

################################################################################
# Minimal Automated Testing TODO(skg): Improve.
################################################################################
check_PROGRAMS = \
trivial

if QUO_WITH_MPIFC
check_PROGRAMS += \
quofort
endif

TESTS = \
scripts/run-c-tests.sh

if QUO_WITH_MPIFC
TESTS += \
scripts/run-fortran-tests.sh
endif

# Note that this MUST be terminated by a semicolon.
AM_TESTS_ENVIRONMENT = \
. $(top_srcdir)/tests/scripts/tests-env.sh;

# With this, the test scripts will be able to print diagnostic messages to the
# original standard error stream, even if the test driver redirects the stderr
# of the test scripts to a log file before executing them.
AM_TESTS_FD_REDIRECT = 9>&2
