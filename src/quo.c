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
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

/* ////////////////////////////////////////////////////////////////////////// */
/* global state                                                               */
/* ////////////////////////////////////////////////////////////////////////// */
static bool quo_initialized = 0;

/* ////////////////////////////////////////////////////////////////////////// */
/* quo_t type definition */
typedef struct quo_t {
    pid_t pid;
    quo_hwloc_t *hwloc;
    bool initialized;
    bool bound;
    int nsockets;
    int ncores;
} quo_t;

/* ////////////////////////////////////////////////////////////////////////// */
/* public api */ 
/* ////////////////////////////////////////////////////////////////////////// */

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
    newq->pid = getpid();

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

    /* XXX TODO */

    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_finalize(void)
{
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_node_topo_dump(const quo_t *q)
{
    return quo_hwloc_node_topo_dump(q->hwloc);
}

/* ////////////////////////////////////////////////////////////////////////// */
#if 0 /* XXX */
int
quo_amount_sys_memory
#endif

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_nsockets(const quo_t *q,
             int *out_nsockets)
{
    int rc = QUO_ERR;

    if (NULL == q || NULL == out_nsockets) return QUO_ERR_INVLD_ARG;

    if (QUO_SUCCESS != (rc = quo_hwloc_sockets(q->hwloc, out_nsockets))) {
        return rc;
    }
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_ncores(const quo_t *q,
           int socket,
           int *out_ncores)
{
    return QUO_SUCCESS;
}