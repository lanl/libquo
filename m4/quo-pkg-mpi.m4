#
# SYNOPSIS
#
#   AX_PKG_MPI()
#
# DESCRIPTION
# checks for MPI compiler support.
#
# COPYRIGHT
# Copyright (c) 2013      Los Alamos National Security, LLC.
#                         All rights reserved.
#

AC_DEFUN([AX_PKG_MPI], [dnl
    AX_PKG_MPI_HAVE_MPI=0
    AC_CHECK_PROGS(MPICC, mpicc mpxlc, $CC)
    CC="$MPICC"
    AC_MSG_CHECKING([if CC can compile MPI applications])
    AC_CHECK_FUNC([MPI_Init],
                  [AC_MSG_RESULT([yes])
                   AX_PKG_MPI_HAVE_MPI=1],
                  [AC_MSG_RESULT([no])])
    dnl FIXME - add proper checks here
    AS_IF([test "x$AX_PKG_MPI_HAVE_MPI" = "x1"],
          [AC_CHECK_PROGS(MPIFORT, mpifort mpif90 mpif77, $FC)], [])
          FC="$MPIFORT"
])
