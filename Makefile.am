#
# Copyright (c) 2013-2019 Triad National Security, LLC
#                         All rights reserved.
#
# This file is part of the libquo project. See the LICENSE file at the
# top-level directory of this distribution.
#

ACLOCAL_AMFLAGS = -Iconfig -Isrc/hwloc/config

EXTRA_DIST = \
AUTHORS README.md THANKS LICENSE COPYRIGHT autogen contrib BROKEN-COMPILERS

SUBDIRS = \
src \
src/fortran \
src/tools \
src/xpm \
tests \
demos \
pkgconfig \
docs

AM_CFLAGS = $(HWLOC_EMBEDDED_CFLAGS)

AM_CPPFLAGS = $(HWLOC_EMBEDDED_CPPFLAGS)
