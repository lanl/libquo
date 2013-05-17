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

m4_define([AX_PKG_MPI_testbody], [
    #include "mpi.h"
    MPI_Finalize();
])

AC_DEFUN([AX_PKG_MPI], [dnl
    AX_PKG_MPI_HAVE_MPI=0
    AC_MSG_CHECKING([if CC can compile MPI applications])
    AC_LINK_IFELSE([AC_LANG_PROGRAM([AX_PKG_MPI_testbody])],
        [AC_MSG_RESULT([yes])
         AX_PKG_MPI_HAVE_MPI=1],
        [AC_MSG_RESULT([no])]
    )
])
