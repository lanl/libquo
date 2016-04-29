###############################################################################
# Copyright (c) 2006-2012 Cisco Systems, Inc.  All rights reserved.
# Copyright (c) 2012      The University of Tennessee and The University
#                         of Tennessee Research Foundation.  All rights
#                         reserved.
# Copyright (c) 2012      Inria.  All rights reserved.
# Copyright (c) 2013-2015 Los Alamos National Security, LLC.
#                         All rights reserved.
#
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
################################################################################

SUBDIRS = hwloc

LIBVINFO = "3:0:0"

lib_LTLIBRARIES = \
libquo.la

if QUO_WITH_MPIFC
lib_LTLIBRARIES += libquo-usequo.la
endif

include_HEADERS = \
quo.h

libquo_la_SOURCES = \
quo-private.h \
quo-utils.h quo-utils.c \
quo-set.h quo-set.c \
quo-hwloc.h quo-hwloc.c \
quo-mpi.h quo-mpi.c \
quo.h quo.c

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libquo.pc

if QUO_WITH_MPIFC
libquo_usequo_la_SOURCES = \
quof.f90
libquo_usequo_la_FCFLAGS = -I$(top_srcdir)/src
libquo_usequo_la_LDFLAGS = -L$(top_srcdir)/src -version-info $(LIBVINFO)
libquo_usequo_la_LIBADD = libquo.la
endif

# these are generated
MOSTLYCLEANFILES = *.mod

# install the generated .mod files.  unfortunately, each f90 compiler
# may generate different filenames, so we have to use a glob.  :-(
install-exec-hook:
	@ for file in `ls *.mod`; do \
	  echo $(INSTALL) $$file $(DESTDIR)$(libdir); \
	  $(INSTALL) $$file $(DESTDIR)$(libdir); \
	done

uninstall-local:
	@ for file in `ls *.mod`; do \
	  echo rm -f $(DESTDIR)$(libdir)/$$file; \
	  rm -f $(DESTDIR)$(libdir)/$$file; \
	done

libquo_la_CFLAGS =
# From: http://www.sourceware.org/autobook/autobook/autobook_91.html
#
# The version scheme used by Libtool tracks interfaces, where an interface is the
# set of exported entry points into the library. All Libtool libraries start with
# `-version-info' set to `0:0:0' -- this will be the default version number if
# you don't explicitly set it on the Libtool link command line. The meaning of
# these numbers (from left to right) is as follows:
#
# CURRENT -- The number of the current interface exported by the library. A
# current value of `0', means that you are calling the interface exported by
# this library interface 0.
#
# REVISION -- The implementation number of the most recent interface exported by
# this library. In this case, a revision value of `0' means that this is the
# first implementation of the interface.  If the next release of this library
# exports the same interface, but has a different implementation (perhaps some
# bugs have been fixed), the revision number will be higher, but current number
# will be the same. In that case, when given a choice, the library with the
# highest revision will always be used by the runtime loader.
#
# AGE -- The number of previous additional interfaces supported by this library.
# If age were `2', then this library can be linked into executables which were
# built with a release of this library that exported the current interface
# number, current, or any of the previous two interfaces.  By definition age
# must be less than or equal to current. At the outset, only the first ever
# interface is implemented, so age can only be `0'.
#
# For later releases of a library, the `-version-info' argument needs to be set
# correctly depending on any interface changes you have made. This is quite
# straightforward when you understand what the three numbers mean:

# If you have changed any of the sources for this library, the revision number
# must be incremented. This is a new revision of the current interface.  If the
# interface has changed, then current must be incremented, and revision reset to
# `0'. This is the first revision of a new interface.  If the new interface is a
# superset of the previous interface (that is, if the previous interface has not
# been broken by the changes in this new release), then age must be incremented.
# This release is backwards compatible with the previous release.  If the new
# interface has removed elements with respect to the previous interface, then
# you have broken backward compatibility and age must be reset to `0'. This
# release has a new, but backwards incompatible interface.  For example, if the
# next release of the library included some new commands for an existing socket
# protocol, you would use -version-info 1:0:1. This is the first revision of a
# new interface. This release is backwards compatible with the previous release.
# Later, you implement a faster way of handling part of the algorithm at the
# core of the library, and release it with -version-info 1:1:1. This is a new
# revision of the current interface.
#
# Unfortunately the speed of your new implementation can only be fully exploited
# by changing the API to access the structures at a lower level, which breaks
# compatibility with the previous interface, so you release it as -version-info
# 2:0:0. This release has a new, but backwards incompatible interface.
libquo_la_LDFLAGS = -version-info $(LIBVINFO)
libquo_la_LIBADD = $(HWLOC_EMBEDDED_LDADD) $(HWLOC_EMBEDDED_LIBS)

################################################################################
bin_PROGRAMS = \
quo-info

quo_info_SOURCES = \
quo-info.c
