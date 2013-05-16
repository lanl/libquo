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
typedef struct quo_global_state_t {
    bool quo_initialized;
} quo_global_state_t;


static quo_global_state_t qgstate = {
    .quo_initialized = false
};

/* ////////////////////////////////////////////////////////////////////////// */
/* quo_t type definition */
struct quo_t {
    pid_t pid;
    quo_hwloc_t *hwloc;
};

/* ////////////////////////////////////////////////////////////////////////// */
/* private routines */
/* ////////////////////////////////////////////////////////////////////////// */
static inline int
hadinit(void)
{
    return qgstate.quo_initialized;
}

static void
noinit_msg_emit(const char *func)
{
    fprintf(stderr, QUO_ERR_PREFIX"%s called before %s. Cannot continue.\n",
            func, "quo_init");
}

#define noinit_action                                                          \
do {                                                                           \
    if (!hadinit()) {                                                          \
        noinit_msg_emit(__func__);                                             \
        return QUO_ERR;                                                        \
    }                                                                          \
} while (0)

/* ////////////////////////////////////////////////////////////////////////// */
/* public api routines */
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
    /* nothing really to do here. placeholder */
    qgstate.quo_initialized = true;
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_construct(quo_t **q)
{
    int qrc = QUO_SUCCESS;
    quo_t *newq = NULL;

    /* make sure we are initialized before we continue */
    noinit_action;
    if (NULL == q) return QUO_ERR_INVLD_ARG;
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
        if (NULL != newq) free(newq);
        newq = NULL;
    }
    *q = newq;
    return qrc;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_destruct(quo_t *q)
{
    /* make sure we are initialized before we continue */
    noinit_action;
    if (NULL == q) return QUO_ERR_INVLD_ARG;
    /* XXX TODO */
    if (q->hwloc) (void)quo_hwloc_destruct(q->hwloc);
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_finalize(void)
{
    /* make sure we are initialized before we continue */
    noinit_action;
    qgstate.quo_initialized = false;
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_node_topo_emit(const quo_t *q)
{
    /* make sure we are initialized before we continue */
    noinit_action;
    return quo_hwloc_node_topo_emit(q->hwloc);
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
    /* make sure we are initialized before we continue */
    noinit_action;
    if (NULL == q || NULL == out_nsockets) return QUO_ERR_INVLD_ARG;
    return quo_hwloc_sockets(q->hwloc, out_nsockets);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_ncores(const quo_t *q,
           int *out_ncores)
{
    /* make sure we are initialized before we continue */
    noinit_action;
    if (NULL == q || NULL == out_ncores) return QUO_ERR_INVLD_ARG;
    return quo_hwloc_cores(q->hwloc, out_ncores);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_npus(const quo_t *q,
         int *out_npus)
{
    /* make sure we are initialized before we continue */
    noinit_action;
    if (NULL == q || NULL == out_npus) return QUO_ERR_INVLD_ARG;
    return quo_hwloc_pus(q->hwloc, out_npus);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_bound(const quo_t *q,
          bool *bound)
{
    /* make sure we are initialized before we continue */
    noinit_action;
    return quo_hwloc_bound(q->hwloc, q->pid, bound);
}
