#
# Copyright (c) 2013-2019 Triad National Security, LLC
#                         All rights reserved.
#
# This file is part of the libquo project. See the LICENSE file at the
# top-level directory of this distribution.
#

SUBDIRS = \
mpi-openmp

noinst_PROGRAMS = \
basic-info \
nice-tid-distrib \
quodemo-0 \
quodemo-1 \
caller-driven-multilib \
callee-driven-multilib \
mpi-omp \
quo-mpi-sub-comm

noinst_LTLIBRARIES = \
libcaller_driven_callee_p1.la \
libcallee_driven_callee_p1.la

###
basic_info_SOURCES = basic-info.c
basic_info_CPPFLAGS = -I$(top_srcdir)/src
basic_info_LDADD = $(top_builddir)/src/libquo.la

###
nice_tid_distrib_SOURCES = nice-tid-distrib.c
nice_tid_distrib_CPPFLAGS = -I$(top_srcdir)/src
nice_tid_distrib_LDADD = $(top_builddir)/src/libquo.la

###
quodemo_0_SOURCES = quodemo-0.c
quodemo_0_CPPFLAGS = -I$(top_srcdir)/src
quodemo_0_LDADD = $(top_builddir)/src/libquo.la

###
quodemo_1_SOURCES = quodemo-1.c
quodemo_1_CPPFLAGS = -I$(top_srcdir)/src
quodemo_1_LDADD = $(top_builddir)/src/libquo.la

### caller-driven multi-library example
caller_driven_multilib_SOURCES = caller-driven-ex-common.h caller-driven-ex-p0.c
caller_driven_multilib_CPPFLAGS = -I$(top_srcdir)/src
caller_driven_multilib_LDADD = \
$(top_builddir)/src/libquo.la libcaller_driven_callee_p1.la

libcaller_driven_callee_p1_la_SOURCES = \
caller-driven-ex-common.h caller-driven-ex-p1.h caller-driven-ex-p1.c
libcaller_driven_callee_p1_la_CPPFLAGS = -I$(top_srcdir)/src
libcaller_driven_callee_p1_la_LIBADD = $(top_builddir)/src/libquo.la

### callee-driven multi-library example
callee_driven_multilib_SOURCES = callee-driven-ex-p0.c
callee_driven_multilib_CPPFLAGS = -I$(top_srcdir)/src
callee_driven_multilib_LDADD = \
$(top_builddir)/src/libquo.la libcallee_driven_callee_p1.la

libcallee_driven_callee_p1_la_SOURCES = \
callee-driven-ex-p1.h callee-driven-ex-p1.c
libcallee_driven_callee_p1_la_CPPFLAGS = -I$(top_srcdir)/src
libcallee_driven_callee_p1_la_LIBADD = $(top_builddir)/src/libquo.la

###
mpi_omp_SOURCES = mpi-omp.c
mpi_omp_CPPFLAGS = -I$(top_srcdir)/src
mpi_omp_LDADD = $(top_builddir)/src/libquo.la

###
quo_mpi_sub_comm_SOURCES = quo-mpi-sub-comm.c
quo_mpi_sub_comm_CPPFLAGS = -I$(top_srcdir)/src
quo_mpi_sub_comm_LDADD = $(top_builddir)/src/libquo.la
