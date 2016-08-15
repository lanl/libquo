#
# SYNOPSIS
#
#   AX_PROG_MPICC()
#   AX_PROG_MPIFC()
#
# DESCRIPTION
# checks for MPI wrapper compiler support.
#
# COPYRIGHT
# Copyright (c) 2013-2016 Los Alamos National Security, LLC.
#                         All rights reserved.
#

AC_DEFUN([AX_PROG_MPICC], [dnl
    dnl MPI CC support
    AC_LANG_PUSH([C])
    AX_PROG_MPICC_HAVE_MPICC=0
    AC_CHECK_FUNC([MPI_Init],
                  [AX_PROG_MPICC_HAVE_MPICC=1], [])
    AC_LANG_POP([C])
])

AC_DEFUN([AX_PROG_MPIFC], [dnl
    dnl MPI Fortran support
    AC_LANG_PUSH([Fortran])
    AX_PROG_MPIFC_HAVE_MPIFC=0
    AC_MSG_CHECKING([if FC can compile MPI applications])
    AC_LINK_IFELSE([AC_LANG_PROGRAM([],[      call MPI_INIT])],dnl
                    [AX_PROG_MPIFC_HAVE_MPIFC=1
                     AC_MSG_RESULT([yes])],dnl
                    [AC_MSG_RESULT([no])])
    AC_LANG_POP([Fortran])
])
