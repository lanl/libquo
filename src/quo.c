/*
 * Copyright (c) 2013-2024 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

/**
 * @file quo.c
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quo.h"
#include "quo-private.h"
#include "quo-set.h"
#include "quo-hwloc.h"
#include "quo-mpi.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
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
static int
init_cached_attrs(QUO_t *q)
{
    int rc = QUO_SUCCESS;

    if (!q) return QUO_ERR_INVLD_ARG;

    q->pid = getpid();

    if (QUO_SUCCESS != (rc = QUO_nqids(q, &q->nqid))) {
        QUO_ERR_MSGRC("QUO_nqids", rc);
        goto out;
    }
    if (QUO_SUCCESS != (rc = QUO_id(q, &q->qid))) {
        QUO_ERR_MSGRC("QUO_id", rc);
        goto out;
    }
out:
    return rc;
}

/* ////////////////////////////////////////////////////////////////////////// */
static int
construct_quoc(QUO_t **q)
{
    int qrc = QUO_SUCCESS;
    QUO_t *newq = NULL;

    if (!q) return QUO_ERR_INVLD_ARG;

    if (NULL == (newq = calloc(1, sizeof(*newq)))) {
        QUO_OOR_COMPLAIN();
        return QUO_ERR_OOR;
    }
    if (QUO_SUCCESS != (qrc = quo_hwloc_construct(&newq->hwloc))) {
        QUO_ERR_MSGRC("quo_hwloc_construct", qrc);
        goto out;
    }
    if (QUO_SUCCESS != (qrc = quo_mpi_construct(&newq->mpi))) {
        QUO_ERR_MSGRC("quo_mpi_construct", qrc);
        goto out;
    }
out:
    if (QUO_SUCCESS != qrc) {
        QUO_free(newq);
        *q = NULL;
    }
    *q = newq;
    return qrc;
}

/* ////////////////////////////////////////////////////////////////////////// */
/* public api routines */
/* ////////////////////////////////////////////////////////////////////////// */

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_version(int *version,
            int *subversion)
{
    if (!version || !subversion) return QUO_ERR_INVLD_ARG;
    *version = QUO_VER;
    *subversion = QUO_SUBVER;
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_create(QUO_t **q,
           MPI_Comm comm)
{
    return QUO_create_with_flags(q, comm, (QUO_create_flags_t)0);
}

int
QUO_create_with_flags(QUO_context *q,
                      MPI_Comm comm,
                      QUO_create_flags_t flags)
{
    int rc = QUO_ERR;
    QUO_t *tq = NULL;

    if (!q) return QUO_ERR_INVLD_ARG;
    /* construct a new context */
    if (QUO_SUCCESS != (rc = construct_quoc(&tq))) goto out;
    /* We need some MPI bits for hwloc init, so init MPI first. */
    if (QUO_SUCCESS != (rc = quo_mpi_init(tq->mpi, comm))) {
        QUO_ERR_MSGRC("quo_mpi_init", rc);
        goto out;
    }
    if (QUO_SUCCESS != (rc = quo_hwloc_init(tq->hwloc, tq->mpi, flags))) {
        QUO_ERR_MSGRC("quo_hwloc_init", rc);
        goto out;
    }
    tq->initialized = true;
    /* Since we use internal QUO_ calls that require an initialized context, do
     * this after we set the initialized flag to true. */
    if (QUO_SUCCESS != (rc = init_cached_attrs(tq))) {
        QUO_ERR_MSGRC("init_cached_attrs", rc);
        goto out;
    }
out:
    if (QUO_SUCCESS != rc) *q = NULL;
    else *q = tq;
    return rc;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_free(QUO_t *q)
{
    int nerrs = 0;
    /* okay to pass NULL here. just return success */
    if (!q) return QUO_SUCCESS;
    /* we can call free before init. useful in error paths. */
    if (q->hwloc) {
        if (QUO_SUCCESS != quo_hwloc_destruct(q->hwloc)) nerrs++;
    }
    if (q->mpi) {
        if (QUO_SUCCESS != quo_mpi_destruct(q->mpi)) nerrs++;
    }
    free(q);
    return nerrs == 0 ? QUO_SUCCESS : QUO_ERR;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_nobjs_in_type_by_type(QUO_t *q,
                          QUO_obj_type_t in_type,
                          int in_type_index,
                          QUO_obj_type_t type,
                          int *out_result)
{
    if (!q || !out_result) return QUO_ERR_INVLD_ARG;
    /* make sure we are initialized before we continue */
    QUO_NO_INIT_ACTION(q);
    return quo_hwloc_get_nobjs_in_type_by_type(q->hwloc,
                                               in_type,
                                               (unsigned)in_type_index,
                                               type,
                                               out_result);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_cpuset_in_type(QUO_t *q,
                   QUO_obj_type_t type,
                   int in_type_index,
                   int *out_result)
{
    if (!q || !out_result) return QUO_ERR_INVLD_ARG;
    /* make sure we are initialized before we continue */
    QUO_NO_INIT_ACTION(q);
    return quo_hwloc_is_in_cpuset_by_type_id(q->hwloc, type, q->pid,
                                             (unsigned)in_type_index,
                                             out_result);
}

/* ////////////////////////////////////////////////////////////////////////// */
/**
 * caller is responsible for freeing *out_qids.
 */
int
QUO_qids_in_type(QUO_t *q,
                 QUO_obj_type_t type,
                 int in_type_index,
                 int *out_nqids,
                 int **out_qids)
{
    int rc = QUO_ERR;
    int tot_smpranks = 0;
    int nsmpranks = 0;
    int *smpranks = NULL;

    if (!q || !out_nqids || !out_qids) return QUO_ERR_INVLD_ARG;
    /* make sure we are initialized before we continue */
    QUO_NO_INIT_ACTION(q);
    *out_nqids = 0; *out_qids = NULL;
    /* set how many node ranks on the node */
    tot_smpranks = q->nqid;
    /* smp ranks are always monotonically increasing starting at 0 */
    for (int rank = 0; rank < tot_smpranks; ++rank) {
        /* whether or not the particular pid is in the given obj type */
        int in_cpuset = 0;
        /* the smp rank's pid */
        pid_t rpid = 0;
        if (QUO_SUCCESS != (rc = quo_mpi_smprank2pid(q->mpi, rank, &rpid))) {
            /* rc set in failure path */
            goto out;
        }
        rc = quo_hwloc_is_in_cpuset_by_type_id(q->hwloc, type, rpid,
                                               in_type_index, &in_cpuset);
        if (QUO_SUCCESS != rc) goto out;
        /* if the rank's cpuset falls within the given obj, then add it */
        if (in_cpuset) {
            int *newsmpranks = realloc(smpranks, (nsmpranks + 1) * sizeof(int));
            if (!newsmpranks) {
                rc = QUO_ERR_OOR;
                goto out;
            }
            /* add the newly found rank to the list and increment num found */
            newsmpranks[nsmpranks++] = rank;
            smpranks = newsmpranks;
        }
    }
    *out_nqids = nsmpranks;
    *out_qids = smpranks;
out:
    if (QUO_SUCCESS != rc) {
        if (smpranks) free(smpranks);
    }
    return rc;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_nobjs_by_type(QUO_t *q,
                  QUO_obj_type_t target_type,
                  int *out_nobjs)
{
    if (!q || !out_nobjs) return QUO_ERR_INVLD_ARG;
    /* make sure we are initialized before we continue */
    QUO_NO_INIT_ACTION(q);
    return quo_hwloc_get_nobjs_by_type(q->hwloc, target_type, out_nobjs);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_nnumanodes(QUO_t *q,
               int *out_nnumanodes)
{
    if (!q || !out_nnumanodes) return QUO_ERR_INVLD_ARG;
    /* make sure we are initialized before we continue */
    QUO_NO_INIT_ACTION(q);
    return quo_hwloc_get_nobjs_by_type(q->hwloc,
                                       QUO_OBJ_NUMANODE,
                                       out_nnumanodes);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_nsockets(QUO_t *q,
             int *out_nsockets)
{
    if (!q || !out_nsockets) return QUO_ERR_INVLD_ARG;
    /* make sure we are initialized before we continue */
    QUO_NO_INIT_ACTION(q);
    return quo_hwloc_get_nobjs_by_type(q->hwloc, QUO_OBJ_SOCKET, out_nsockets);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_ncores(QUO_t *q,
           int *out_ncores)
{
    if (!q || !out_ncores) return QUO_ERR_INVLD_ARG;
    /* make sure we are initialized before we continue */
    QUO_NO_INIT_ACTION(q);
    return quo_hwloc_get_nobjs_by_type(q->hwloc, QUO_OBJ_CORE, out_ncores);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_npus(QUO_t *q,
         int *out_npus)
{
    if (!q || !out_npus) return QUO_ERR_INVLD_ARG;
    /* make sure we are initialized before we continue */
    QUO_NO_INIT_ACTION(q);
    return quo_hwloc_get_nobjs_by_type(q->hwloc, QUO_OBJ_PU, out_npus);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_bound(QUO_t *q,
          int *bound)
{
    int rc = QUO_ERR;
    bool bound_b = false;

    if (!q || !bound) return QUO_ERR_INVLD_ARG;
    /* make sure we are initialized before we continue */
    QUO_NO_INIT_ACTION(q);
    if (QUO_SUCCESS != (rc = quo_hwloc_bound(q->hwloc, q->pid, &bound_b))) {
        return rc;
    }
    *bound = (int)bound_b;
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_stringify_cbind(QUO_t *q,
                    char **cbind_str)
{
    if (!q || !cbind_str) return QUO_ERR_INVLD_ARG;
    QUO_NO_INIT_ACTION(q);
    return quo_hwloc_stringify_cbind(q->hwloc, q->pid, cbind_str);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_nnodes(QUO_t *q,
           int *out_nodes)
{
    if (!q || !out_nodes) return QUO_ERR_INVLD_ARG;
    QUO_NO_INIT_ACTION(q);
    return quo_mpi_nnodes(q->mpi, out_nodes);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_nqids(QUO_t *q,
          int *out_nqids)
{
    if (!q || !out_nqids) return QUO_ERR_INVLD_ARG;
    QUO_NO_INIT_ACTION(q);
    return quo_mpi_nnoderanks(q->mpi, out_nqids);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_id(QUO_t *q,
       int *out_qid)
{
    if (!q || !out_qid) return QUO_ERR_INVLD_ARG;
    QUO_NO_INIT_ACTION(q);
    return quo_mpi_noderank(q->mpi, out_qid);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_bind_push(QUO_t *q,
              QUO_bind_push_policy_t policy,
              QUO_obj_type_t type,
              int obj_index)
{
    if (!q) return QUO_ERR_INVLD_ARG;
    QUO_NO_INIT_ACTION(q);
    return quo_hwloc_bind_push(q->hwloc, policy, type, (unsigned)obj_index);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_bind_pop(QUO_t *q)
{
    if (!q) return QUO_ERR_INVLD_ARG;
    QUO_NO_INIT_ACTION(q);
    return quo_hwloc_bind_pop(q->hwloc);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_barrier(QUO_t *q)
{
    if (!q) return QUO_ERR_INVLD_ARG;
    QUO_NO_INIT_ACTION(q);
    return quo_mpi_sm_barrier(q->mpi);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_get_mpi_comm_by_type(QUO_t *q,
                         QUO_obj_type_t target_type,
                         MPI_Comm *out_comm)
{
    if (!q || !out_comm) return QUO_ERR_INVLD_ARG;
    /* make sure we are initialized before we continue */
    QUO_NO_INIT_ACTION(q);

    return quo_mpi_get_comm_by_type(q->mpi, target_type, out_comm);
}
