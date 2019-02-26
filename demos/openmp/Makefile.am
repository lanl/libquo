#
# Copyright (c) 2013-2019 Triad National Security, LLC
#                         All rights reserved.
#
# This file is part of the libquo project. See the LICENSE file at the
# top-level directory of this distribution.
#

noinst_PROGRAMS = sample quofort

sample_SOURCES = sample.c
sample_CPPFLAGS  = -I$(top_srcdir)/src
sample_LDADD   = $(top_builddir)/src/libquo.la

quofort_SOURCES = quofort.f90
quofort_FCFLAGS = -I$(top_builddir)/src -fopenmp #mod lives in top_builddir
quofort_LDADD   = $(top_builddir)/src/libquo-usequo.la $(top_builddir)/src/libquo.la
