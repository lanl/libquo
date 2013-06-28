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
    AX_PROGS_MPICOMPS_HAVE_MPICC=0
    AC_CHECK_FUNC([MPI_Init],
                  [AX_PROGS_MPICOMPS_HAVE_MPICC=1], [])
])
