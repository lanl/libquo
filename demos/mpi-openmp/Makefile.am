#
# Copyright (c) 2017      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the libquo project. See the LICENSE file at the
# top-level directory of this distribution.
#

#SUBDIRS = openmp

noinst_PROGRAMS = \
dgemv

###
dgemv_SOURCES = dgemv.c 
dgemv_CPPFLAGS = -I$(top_srcdir)/src
dgemv_LDADD = $(top_builddir)/src/libquo.la
