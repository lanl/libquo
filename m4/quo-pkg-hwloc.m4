#
# SYNOPSIS
#
#   AX_PKG_HWLOC()
#
# DESCRIPTION
# checks for hwloc headers and library.
#
# LICENSE
# Copyright (c) 2013      Los Alamos National Security, LLC.
#                         All rights reserved.
#

AC_DEFUN([AX_PKG_HWLOC], [dnl
    AC_SEARCH_LIBS([hwloc_topology_init], [hwloc],dnl
                   [HAVE_PKG_HWLOC=1],dnl
                   [HAVE_PKG_HWLOC=0])
])
