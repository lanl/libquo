###############################################################################
# Copyright (c) 2013      Los Alamos National Security, LLC.
#                         All rights reserved.
################################################################################

AC_PREREQ([2.60])

AC_INIT([libquo], [0.1], [samuel@lanl.gov])

# Check If AM_SILENT_RULES Rule Are Supported
# If So, Build Using AM_SILENT_RULES
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_SRCDIR([src])
AC_CONFIG_MACRO_DIR([m4])
AM_CONFIG_HEADER([config.h])

# note: -Wall -Werror are automake options.  they are NOT compiler flags
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_USE_SYSTEM_EXTENSIONS

CFLAGS_SAVE="$CFLAGS"
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CC_C99
AC_GNU_SOURCE
CFLAGS="$CFLAGS_SAVE"

LT_INIT

# checks for libraries.
# none at this point

# checks for header files.
AC_CHECK_HEADERS([\
inttypes.h limits.h stdint.h stdlib.h string.h unistd.h stdbool.h time.h \
getopt.h ctype.h netdb.h sys/socket.h netinet/in.h arpa/inet.h sys/types.h
])

# checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strerror strtoul])

AX_PKG_HWLOC
AS_IF([test "x$HAVE_PKG_HWLOC" != "x1"],dnl
      [AC_MSG_ERROR([dnl
*** hwloc package required but not found. please set CFLAGS and LDFLAGS flags.])
      ]
)



AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile])

AC_OUTPUT

# let the user know the configuration 

cat << EOF

libquo configuration
+-------------------
| version : $VERSION
| CC      : $CC
| CFLAGS  : $CFLAGS
| LDFLAGS : $LDFLAGS
| LIBS    : $LIBS
| CPPFLAGS: $CPPFLAGS
| CPP     : $CPP

EOF
