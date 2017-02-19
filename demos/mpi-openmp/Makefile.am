#
# Copyright (c) 2017      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the libquo project. See the LICENSE file at the
# top-level directory of this distribution.
#

noinst_PROGRAMS =

if QUO_HAVE_OPENMP_SUPPORT
noinst_PROGRAMS += \
dgemv

###
dgemv_SOURCES = dgemv.c 
dgemv_CFLAGS = @OPENMP_CFLAGS@
dgemv_CPPFLAGS = -I$(top_srcdir)/src
dgemv_LDADD = $(top_builddir)/src/libquo.la
endif # QUO_HAVE_OPENMP_SUPPORT

noinst_PROGRAMS += \
libzeta
###
libzeta_SOURCES = zeta.c
libzeta_CFLAGS =
libzeta_CPPFLAGS =
libzeta_LDADD =

noinst_PROGRAMS += \
libeta
###
libeta_SOURCES = eta.c
libeta_CFLAGS =
libeta_CPPFLAGS =
libeta_LDADD =
