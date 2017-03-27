#
# SYNOPSIS
#
#   AX_PKG_QUO_ATMS([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
#
# DESCRIPTION
# Enables atms support in QUO and checks for support.
#
# COPYRIGHT
# Copyright (c) 2017      Los Alamos National Security, LLC.
#                         All rights reserved.
#

AC_DEFUN([AX_PKG_QUO_ATMS], [
dnl
AC_ARG_ENABLE([atms],
    AS_HELP_STRING([--enable-atms],
                   [Enable atms support (disabled by default).]))

AS_IF([test "x$enable_atms" = "xyes"], [
    AC_MSG_NOTICE([atms support requested... checking for support.])
])
dnl
]) dnl AX_PKG_QUO_ATMS
