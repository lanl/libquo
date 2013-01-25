/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quo.h"
#include "quo-private.h"
#include "quo-hwloc.h"

#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif

/* ////////////////////////////////////////////////////////////////////////// */
/* global state                                                               */
/* ////////////////////////////////////////////////////////////////////////// */
static bool quo_initialized = 0;

/* ////////////////////////////////////////////////////////////////////////// */
/* quo_t type definition */
typedef struct quo_t {
    quo_hwloc_t *hwloc;
    bool initialized;
    bool bound;
    int nsockets;
    int ncores;
} quo_t;

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_version(int *version,
            int *subversion)
{
    if (NULL == version || NULL == subversion) return QUO_ERR_INVLD_ARG;

    *version = QUO_VER;
    *subversion = QUO_SUBVER;

    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_init(void)
{
    quo_initialized = true;
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_construct(quo_t **q)
{
    int qrc = QUO_SUCCESS;
    quo_t *newq = NULL;

    if (NULL == q) return QUO_ERR_INVLD_ARG;

    /* make sure we are initialized before we continue */
    if (!quo_initialized) {
        fprintf(stderr, QUO_ERR_PREFIX"%s called before %s. Cannot continue.\n",
                __func__, "quo_init");
    }
    if (NULL == (newq = calloc(1, sizeof(*newq)))) {
        QUO_OOR_COMPLAIN();
        return QUO_ERR_OOR;
    }
    if (QUO_SUCCESS != (qrc = quo_hwloc_construct(&newq->hwloc))) {
        fprintf(stderr, QUO_ERR_PREFIX"%s failed. Cannot continue.\n",
                "quo_hwloc_construct");
        goto out;
    }
    int tmp = -1;
    quo_hwloc_sockets(newq->hwloc, &tmp);

    printf ("#socks: %d\n", tmp);

out:
    if (QUO_SUCCESS != qrc) {
        if (NULL != newq) {
            free(newq);
        }
        newq = NULL;
    }
    *q = newq;

    return qrc;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_destruct(quo_t *q)
{

    if (NULL == q) return QUO_ERR_INVLD_ARG;

    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_finalize(void)
{
    return QUO_SUCCESS;
}
