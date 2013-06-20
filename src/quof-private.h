/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#ifndef QUO_PRIVATE_H_INCLUDED
#define QUO_PRIVATE_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif

/* library version */
#define QUO_VER    1
#define QUO_SUBVER 0

/* ////////////////////////////////////////////////////////////////////////// */
/* convenience macros                                                         */
/* ////////////////////////////////////////////////////////////////////////// */

#define QUO_STRINGIFY(x) #x
#define QUO_TOSTRING(x)  QUO_STRINGIFY(x)

#define QUO_ERR_AT       __FILE__ ": "QUO_TOSTRING(__LINE__)""
#define QUO_ERR_PREFIX   "-["PACKAGE" ERROR: "QUO_ERR_AT"]- "
#define QUO_WARN_PREFIX  "-["PACKAGE" WARNING]- "

#define QUO_OOR_COMPLAIN()                                                     \
do {                                                                           \
    fprintf(stderr, QUO_ERR_PREFIX "out of resources\n");                      \
    fflush(stderr);                                                            \
} while (0)

#define QUO_ERR_MSG(whystr)                                                    \
do {                                                                           \
    fprintf(stderr, QUO_ERR_PREFIX"%s failed: %s.\n", __func__, (whystr));     \
} while (0)

#endif
