#
# SYNOPSIS
#
#   AX_PROG_MPICC()
#
# DESCRIPTION
# checks for MPI compiler support.
#
# COPYRIGHT
# Copyright (c) 2013      Los Alamos National Security, LLC.
#                         All rights reserved.
#

AC_DEFUN([AX_PROG_MPICC], [dnl
    AX_PROG_MPICC_HAVE_MPICC=0
    AC_PROG_CC
    AM_PROG_CC_C_O
    AC_CHECK_PROGS([MPICC], [mpicc mpxlc], [$CC])
    CC="$MPICC"
    AC_PROG_CC_C99
    AC_CHECK_FUNC([MPI_Init],
                  [AX_PROG_MPICC_HAVE_MPICC=1], [])
])

AC_DEFUN([AX_PROG_MPIFORT], [dnl
    AC_PROG_FC
    dnl FIXME - add proper checks here
    AC_CHECK_PROGS([MPIFORT], [mpifort mpif90 mpif77], [$FC])
    FC="$MPIFORT"
])
