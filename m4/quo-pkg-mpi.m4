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
    AC_CHECK_FUNC([MPI_Init],
                  [AX_PKG_MPI_HAVE_MPI=1], [])
])
