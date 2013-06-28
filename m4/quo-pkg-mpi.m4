#
# SYNOPSIS
#
#   AX_PROGS_MPICOMPS()
#
# DESCRIPTION
# checks for MPI wrapper compiler support.
#
# COPYRIGHT
# Copyright (c) 2013      Los Alamos National Security, LLC.
#                         All rights reserved.
#

AC_DEFUN([AX_PROGS_MPICOMPS], [dnl
    dnl MPI CC support
    AX_PROGS_MPICOMPS_HAVE_MPICC=0
    AC_CHECK_FUNC([MPI_Init],
                  [AX_PROGS_MPICOMPS_HAVE_MPICC=1], [])
    dnl MPI Fortran support
    AC_LANG_PUSH([Fortran])
    AX_PROGS_MPICOMPS_HAVE_MPIFC=0
    AC_MSG_CHECKING([if FC can compile MPI applications])
    AC_LINK_IFELSE([AC_LANG_PROGRAM([],[      call MPI_INIT])],dnl
                    [AX_PROGS_MPICOMPS_HAVE_MPIFC=1
                     AC_MSG_RESULT([yes])],dnl
                    [AC_MSG_RESULT([no])])
    AC_LANG_POP([Fortran])
])
