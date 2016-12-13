###############################################################################
# Copyright (c) 2013-2016 Los Alamos National Security, LLC
#                         All rights reserved.
#
# Los Alamos National Security, LLC. This software was produced under U.S.
# Government contract DE-AC52-06NA25396 for Los Alamos National Laboratory
# (LANL), which is operated by Los Alamos National Security, LLC for the U.S.
# Department of Energy. The U.S. Government has rights to use, reproduce, and
# distribute this software.  NEITHER THE GOVERNMENT NOR LOS ALAMOS NATIONAL
# SECURITY, LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY
# FOR THE USE OF THIS SOFTWARE.  If software is modified to produce derivative
# works, such modified software should be clearly marked, so as not to confuse
# it with the version available from LANL.
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
AC_PREREQ([2.60])

AC_INIT([libquo],
        [1.3-alpha],
        [samuel@lanl.gov],
        [libquo],
        [http://losalamos.github.io/libquo/])

AC_CONFIG_MACRO_DIR([config])
AC_CONFIG_SRCDIR([src])

AX_QUO_LIBTOOL_VERSION

################################################################################
# AC_CANONICAL_*
################################################################################
# for hwloc - must be in between AC_INIT and AM_INIT_AUTOMAKE
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

################################################################################
# Automake things
################################################################################
# note: -Wall -Werror are automake options.  they are NOT compiler flags
AM_INIT_AUTOMAKE([1.11 -Wall -Werror foreign])
# Build using AM_SILENT_RULES
AM_SILENT_RULES([yes])
AM_CONFIG_HEADER([config.h])

################################################################################
# C Language
################################################################################
AC_USE_SYSTEM_EXTENSIONS

AC_PROG_CC_C99
AC_PROG_CC_C_O
AC_PROG_CPP

AX_PROG_MPICC

AS_IF([test "x$AX_PROG_MPICC_HAVE_MPICC" != "x1"],dnl
      [AC_MSG_ERROR(
       [*** CC cannot compile MPI apps. set CC to something that can. ***])])

################################################################################
# Fortran Language Support
################################################################################
AC_PROG_FC
AC_PROG_FC_C_O

AX_PROG_MPIFC
# determine whether or not we have a fortran wrapper compiler that can deal with
# mpi apps.  if not disable fortran support.
AS_IF([test "x$AX_PROG_MPIFC_HAVE_MPIFC" != "x1"],dnl
      [AC_MSG_WARN(
       [*** FC cannot compile MPI apps. set FC to something that can. ***])])
AM_CONDITIONAL(QUO_WITH_MPIFC, test "x$AX_PROG_MPIFC_HAVE_MPIFC" = "x1")

################################################################################
# Libtool things
################################################################################
AM_PROG_AR

LT_PREREQ([2.4.2])
LT_INIT

################################################################################
# C++ Language Support
################################################################################
# Delay this bit until now. There is some weird libtool linker interaction that
# I don't understand that is causing some link-time errors in our distro. This
# ordering seems to "fix" things, but I still don't understand why...
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_CXX_C_O

AX_PROG_MPICXX
# determine whether or not we have a C++ wrapper compiler that can deal with
# mpi apps.  if not disable C++ support.
AS_IF([test "x$AX_PROG_MPICXX_HAVE_MPICXX" != "x1"],dnl
      [AC_MSG_WARN(
       [*** CXX cannot compile MPI apps. set CXX to something that can. ***])])
AM_CONDITIONAL(QUO_WITH_MPICXX, test "x$AX_PROG_MPICXX_HAVE_MPICXX" = "x1")

# checks for header files.
AC_CHECK_HEADERS([\
inttypes.h limits.h stdint.h stdlib.h string.h unistd.h stdbool.h time.h \
getopt.h ctype.h netdb.h sys/socket.h netinet/in.h arpa/inet.h sys/types.h \
stddef.h assert.h pthread.h sys/mman.h sys/stat.h fcntl.h syscall.h omp.h \
sched.h strings.h
])

# checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# checks for library functions.
AC_CHECK_FUNCS([memset strerror strtoul mmap])

dnl check for sizeof(uintptr_t) for the Fortran interface.
dnl This sets the size of QUO_IKIND in the quof.h header that is generated.
dnl AC_COMPUTE_INT(quo_sizeof_voidptr, [sizeof(void *)])
dnl AC_SUBST(QUO_IKIND_VAL, $quo_sizeof_voidptr)

################################################################################
# hwloc stuff
################################################################################
# set some hwloc configure options
enable_embedded_mode=yes
enable_libxml2=no
enable_xml=yes
# TODO - allow for this someday (busted on the Crays -- no static libs)
enable_pci=no
enable_libpci=no
enable_libudev=no
#enable_libnuma=no

HWLOC_SET_SYMBOL_PREFIX(quo_internal_)
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

################################################################################
# OpenMP configury (Disable for now)
################################################################################
#AX_OPENMP([], AC_MSG_ERROR([*** OpenMP configure failure. ***]))
#CFLAGS="$CFLAGS $OPENMP_CFLAGS"
#CXXFLAGS="$CXXFLAGS $OPENMP_CXXFLAGS"
#FFLAGS="$FFLAGS $OPENMP_FFLAGS"

################################################################################
# For docs target support
################################################################################
AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test "x$DOXYGEN" = "x"; then
    AC_MSG_WARN([Doxygen not found. Continuing without Doxygen support])
fi
AM_CONDITIONAL([QUO_HAVE_DOXYGEN_DOCS_SUPPORT],
               [test "x$DOXYGEN" != "x"])

################################################################################
# For package info (quo-info)
################################################################################
QUO_WHICH_CC=`which \`echo "$CC" | tr -s ' ' | cut -f1 -d' '\``
QUO_WHICH_FC=`which \`echo "$FC" | tr -s ' ' | cut -f1 -d' '\``
if test "x$QUO_WHICH_FC" = x; then
    QUO_WHICH_FC="none"
fi

AC_SUBST([CC])
AC_SUBST([QUO_WHICH_CC])
AC_SUBST([CFLAGS])
AC_SUBST([CPPFLAGS])
AC_SUBST([CXXFLAGS])
AC_SUBST([CXXCPPFLAGS])
AC_SUBST([FC])
AC_SUBST([QUO_WHICH_FC])
AC_SUBST([FFLAGS])
AC_SUBST([FCFLAGS])
AC_SUBST([LDFLAGS])
AC_SUBST([LIBS])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/fortran/Makefile
                 src/tools/Makefile
                 tests/Makefile
                 demos/Makefile
                 pkgconfig/Makefile
                 pkgconfig/libquo.pc
                 docs/Makefile
                 docs/doxy.cfg])

AC_OUTPUT

# let the user know the configuration
cat << EOF

+-------------------------------------------------------------------------------
| libquo Configuration :::::::::::::::::::::::::::::::::::::::::::::::::::::::
+-------------------------------------------------------------------------------
| version     : ${VERSION}
| target      : ${target}
| CC          : ${CC}
| CFLAGS      : ${CFLAGS}
| CPP         : ${CPP}
| CPPFLAGS    : ${CPPFLAGS}
| LDFLAGS     : ${LDFLAGS}
| LIBS        : ${LIBS}
+-------------------------------------------------------------------------------
| C++ Support Configuration ::::::::::::::::::::::::::::::::::::::::::::::::::
+-------------------------------------------------------------------------------
EOF

if test "x$AX_PROG_MPICXX_HAVE_MPICXX" = "x1"; then
cat << EOF
| CXX         : ${CXX}
| CXXFLAGS    : ${CXXFLAGS}
EOF
else
cat << EOF
| Not Enabled
EOF
fi

cat << EOF
+-------------------------------------------------------------------------------
| Fortran Support Configuration ::::::::::::::::::::::::::::::::::::::::::::::
+-------------------------------------------------------------------------------
EOF

if test "x$AX_PROG_MPIFC_HAVE_MPIFC" = "x1"; then
cat << EOF
| FC          : ${FC}
| FCFLAGS     : ${FCFLAGS}
| FCLIBS      : ${FCLIBS}
EOF
else
cat << EOF
| Not Enabled
EOF
fi
cat << EOF
+-------------------------------------------------------------------------------
| Other Configury ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
+-------------------------------------------------------------------------------
EOF
printf "| Doxygen docs Target Available: %s\n" \
    $( test "x$DOXYGEN" = "x" && printf "no" || printf "yes" )
cat << EOF
+-------------------------------------------------------------------------------
EOF
