###############################################################################
# Copyright (c) 2013      Los Alamos National Security, LLC
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
#   specific prior written permission.  THIS SOFTWARE IS PROVIDED BY LOS ALAMOS
#   NATIONAL SECURITY, LLC AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
#   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
#   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
#   EVENT SHALL LOS ALAMOS NATIONAL SECURITY, LLC OR CONTRIBUTORS BE LIABLE FOR
#   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
#   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
#   DAMAGE.
################################################################################

AC_PREREQ([2.60])

AC_INIT([libquo], [1.1rc2], [samuel@lanl.gov])

# Check If AM_SILENT_RULES Rule Are Supported
# If So, Build Using AM_SILENT_RULES
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_SRCDIR([src])
AC_CONFIG_MACRO_DIR([m4])
AM_CONFIG_HEADER([config.h])

# for hwloc - must be in between AC_INIT and AM_INIT_AUTOMAKE
AC_CANONICAL_TARGET

AC_USE_SYSTEM_EXTENSIONS

# note: -Wall -Werror are automake options.  they are NOT compiler flags
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CC_C99
AM_PROG_AR
AC_GNU_SOURCE
AC_PROG_FC

LT_INIT

# checks for header files.
AC_CHECK_HEADERS([\
inttypes.h limits.h stdint.h stdlib.h string.h unistd.h stdbool.h time.h \
getopt.h ctype.h netdb.h sys/socket.h netinet/in.h arpa/inet.h sys/types.h \
stddef.h assert.h pthread.h sys/mman.h sys/stat.h fcntl.h
])

# checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strerror strtoul mmap])

# check for sizeof(uintptr_t) for the Fortran interface.
# This sets the size of QUO_IKIND in the quof.h header that is generated.
AC_COMPUTE_INT(quo_sizeof_voidptr, [sizeof(void *)])
AC_SUBST(QUO_IKIND_VAL, $quo_sizeof_voidptr)

################################################################################
# mpi stuff
################################################################################
AX_PROGS_MPICOMPS
AS_IF([test "x$AX_PROGS_MPICOMPS_HAVE_MPICC" != "x1"],dnl
      [AC_MSG_ERROR(
       [*** CC cannot compile MPI apps. set CC to something that can. ***])])

# note that a mpi wrapper compiler for fortran is only needed for tests
AS_IF([test "x$AX_PROGS_MPICOMPS_HAVE_MPIFC" != "x1"],dnl
      [AC_MSG_WARN(
       [*** FC cannot compile MPI apps. set FC to something that can. ***])])
AM_CONDITIONAL(QUO_WITH_MPIFC, test "x$AX_PROGS_MPICOMPS_HAVE_MPIFC" = "x1")

################################################################################
# hwloc stuff
################################################################################
# set some hwloc configure options
enable_libxml2=no
enable_xml=yes
# TODO - allow for this someday (busted on the Crays -- no static lib)
enable_pci=no
enable_libpci=no

HWLOC_SET_SYMBOL_PREFIX(quo_)
HWLOC_SETUP_CORE([src/hwloc],
                 [],
                 [AC_MSG_ERROR([*** hwloc configure failure. ***])],
                 [1])
CPPFLAGS="$HWLOC_EMBEDDED_CPPFLAGS $CPPFLAGS"
LIBS="$HWLOC_EMBEDDED_LIBS $LIBS"
HWLOC_DO_AM_CONDITIONALS

################################################################################
# pthreads stuff
################################################################################
AX_PTHREAD([], AC_MSG_ERROR([*** pthread configure failure. ***]))
# TODO
#AC_SUBST(QUO_PTHREAD_CFLAGS, $PTHREAD_CFLAGS)
#AC_SUBST(QUO_PTHREAD_LIBS, $PTHREAD_LIBS)

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile
                 demos/Makefile])

AC_OUTPUT

# let the user know the configuration 

cat << EOF

libquo configuration
+-------------------
| version : $VERSION
| CC      : $CC
| CFLAGS  : $CFLAGS
| LDFLAGS : $LDFLAGS
| LIBS    : $LIBS
| CPPFLAGS: $CPPFLAGS
| CPP     : $CPP
| FC      : $FC

EOF
