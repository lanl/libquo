###############################################################################
# Copyright (c) 2013-2015 Los Alamos National Security, LLC
#                         All rights reserved.
#
# Copyright 2013. Los Alamos National Security, LLC. This software was produced
# under U.S. Government contract DE-AC52-06NA25396 for Los Alamos National
# Laboratory (LANL), which is operated by Los Alamos National Security, LLC for
# the U.S. Department of Energy. The U.S. Government has rights to use,
# reproduce, and distribute this software.  NEITHER THE GOVERNMENT NOR LOS
# ALAMOS NATIONAL SECURITY, LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR
# ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE.  If software is modified
# to produce derivative works, such modified software should be clearly marked,
# so as not to confuse it with the version available from LANL.
#
# Additionally, redistribution and use in source and binary forms, with or
# without modification, are permitted provided that the following conditions are
# met:
#
# · Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# · Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# · Neither the name of Los Alamos National Security, LLC, Los Alamos National
#   Laboratory, LANL, the U.S. Government, nor the names of its contributors may
#   be used to endorse or promote products derived from this software without
#   specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND
# CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS NATIONAL
# SECURITY, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
################################################################################

#SUBDIRS = openmp

noinst_PROGRAMS = \
quoinfo \
quodemo-0 \
quodemo-1 \
quodemo-multilib \
mpi-omp

noinst_LTLIBRARIES = \
libdemop1.la

### info demo 0
quoinfo_SOURCES = quoinfo.c
quoinfo_CPPFLAGS  = -I$(top_srcdir)/src
quoinfo_LDADD   = $(top_builddir)/src/libquo.la

### demo 0
quodemo_0_SOURCES = quodemo-0.c
quodemo_0_CPPFLAGS  = -I$(top_srcdir)/src
quodemo_0_LDADD   = $(top_builddir)/src/libquo.la

### demo 1
quodemo_1_SOURCES = quodemo-1.c
quodemo_1_CPPFLAGS  = -I$(top_srcdir)/src
quodemo_1_LDADD   = $(top_builddir)/src/libquo.la

### demo 2 (multi lib example)
quodemo_multilib_SOURCES = quodemo-multilib-common.h quodemo-p0.c
quodemo_multilib_CPPFLAGS  = -I$(top_srcdir)/src
quodemo_multilib_LDADD   = $(top_builddir)/src/libquo.la libdemop1.la

libdemop1_la_SOURCES = quodemo-multilib-common.h quodemo-p1.h quodemo-p1.c
libdemop1_la_CPPFLAGS = -I$(top_srcdir)/src
libdemop1_la_LIBADD = $(top_builddir)/src/libquo.la

### demo 3
mpi_omp_SOURCES = mpi-omp.c
mpi_omp_CPPFLAGS  = -I$(top_srcdir)/src
mpi_omp_LDADD   = $(top_builddir)/src/libquo.la
