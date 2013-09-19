/**
 * Copyright (c) 2013      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * Copyright 2013. Los Alamos National Security, LLC. This software was produced
 * under U.S. Government contract DE-AC52-06NA25396 for Los Alamos National
 * Laboratory (LANL), which is operated by Los Alamos National Security, LLC for
 * the U.S. Department of Energy. The U.S. Government has rights to use,
 * reproduce, and distribute this software.  NEITHER THE GOVERNMENT NOR LOS
 * ALAMOS NATIONAL SECURITY, LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR
 * ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE.  If software is modified
 * to produce derivative works, such modified software should be clearly marked,
 * so as not to confuse it with the version available from LANL.
 *
 * Additionally, redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following conditions
 * are met:
 *
 * · Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * · Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * · Neither the name of Los Alamos National Security, LLC, Los Alamos
 *   National Laboratory, LANL, the U.S. Government, nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.  THIS SOFTWARE IS
 *   PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND CONTRIBUTORS "AS IS" AND
 *   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *   ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS NATIONAL SECURITY, LLC OR
 *   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quo.h"
#include "quo-private.h"
#include "quo-set.h"
#include "quo-hwloc.h"
#include "quo-mpi.h"

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
/* QUO_t type definition */
struct QUO_t {
    bool initialized;
    pid_t pid;
    quo_hwloc_t *hwloc;
    quo_mpi_t *mpi;
};

/* ////////////////////////////////////////////////////////////////////////// */
/* private routines */
/* ////////////////////////////////////////////////////////////////////////// */
static void
noinit_msg_emit(const char *func)
{
    fprintf(stderr, QUO_ERR_PREFIX"%s called before %s. Cannot continue.\n",
            func, "QUO_init");
}

#define noinit_action(qp)                                                      \
do {                                                                           \
    if (!(qp)->initialized) {                                                  \
        noinit_msg_emit(__func__);                                             \
        return QUO_ERR_CALL_BEFORE_INIT;                                       \
    }                                                                          \
} while (0)

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
        fprintf(stderr, QUO_ERR_PREFIX"%s failed. Cannot continue.\n",
                "quo_hwloc_construct");
        goto out;
    }
    if (QUO_SUCCESS != (qrc = quo_mpi_construct(&newq->mpi))) {
        fprintf(stderr, QUO_ERR_PREFIX"%s failed. Cannot continue.\n",
                "quo_mpi_construct");
        goto out;
    }
    newq->pid = getpid();
out:
    if (QUO_SUCCESS != qrc) {
        QUO_free(newq);
        *q = NULL;
    }
    *q = newq;
    return qrc;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_create(QUO_t **q)
{
    int rc = QUO_ERR;
    QUO_t *tq = NULL;

    if (!q) return QUO_ERR_INVLD_ARG;
    /* construct a new context */
    if (QUO_SUCCESS != (rc = construct_quoc(&tq))) goto out;
    /* init the context */
    if (QUO_SUCCESS != (rc = quo_mpi_init(tq->mpi))) {
        fprintf(stderr, QUO_ERR_PREFIX"%s failed. Cannot continue with %s.\n",
                "quo_mpi_init", PACKAGE);
        goto out;
    }
    tq->initialized = true;
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
    noinit_action(q);
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
    noinit_action(q);
    return quo_hwloc_is_in_cpuset_by_type_id(q->hwloc, type, q->pid,
                                             (unsigned)in_type_index,
                                             out_result);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_nmachine_procs_in_type(QUO_t *q,
                           QUO_obj_type_t type,
                           int in_type_index,
                           int *out_nqids)
{
    int rc = QUO_ERR;
    int tot_smpranks = 0;
    int nsmpranks = 0;

    if (!q || !out_nqids) return QUO_ERR_INVLD_ARG;
    *out_nqids = 0;
    /* make sure we are initialized before we continue */
    noinit_action(q);
    /* figure out how many node ranks on the node */
    if (QUO_SUCCESS != (rc = QUO_nqids(q, &tot_smpranks))) return rc;
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
        /* if the rank's cpuset falls within the given obj, then increment */
        if (in_cpuset) nsmpranks++;
    }
    *out_nqids = nsmpranks;
out:
    return rc;
}

/* ////////////////////////////////////////////////////////////////////////// */
/**
 * caller is responsible for freeing *out_qids.
 */
int
QUO_qids_in_type(QUO_context q,
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
    noinit_action(q);
    *out_nqids = 0; *out_qids = NULL;
    /* figure out how many node ranks on the node */
    if (QUO_SUCCESS != (rc = QUO_nqids(q, &tot_smpranks))) return rc;
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
    noinit_action(q);
    return quo_hwloc_get_nobjs_by_type(q->hwloc, target_type, out_nobjs);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_nnumanodes(QUO_t *q,
               int *out_nnumanodes)
{
    if (!q || !out_nnumanodes) return QUO_ERR_INVLD_ARG;
    /* make sure we are initialized before we continue */
    noinit_action(q);
    return quo_hwloc_get_nobjs_by_type(q->hwloc, QUO_OBJ_NODE, out_nnumanodes);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_nsockets(QUO_t *q,
             int *out_nsockets)
{
    if (!q || !out_nsockets) return QUO_ERR_INVLD_ARG;
    /* make sure we are initialized before we continue */
    noinit_action(q);
    return quo_hwloc_get_nobjs_by_type(q->hwloc, QUO_OBJ_SOCKET, out_nsockets);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_ncores(QUO_t *q,
           int *out_ncores)
{
    if (!q || !out_ncores) return QUO_ERR_INVLD_ARG;
    /* make sure we are initialized before we continue */
    noinit_action(q);
    return quo_hwloc_get_nobjs_by_type(q->hwloc, QUO_OBJ_CORE, out_ncores);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_npus(QUO_t *q,
         int *out_npus)
{
    /* make sure we are initialized before we continue */
    if (!q || !out_npus) return QUO_ERR_INVLD_ARG;
    noinit_action(q);
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
    noinit_action(q);
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
    noinit_action(q);
    return quo_hwloc_stringify_cbind(q->hwloc, q->pid, cbind_str);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_nnodes(QUO_t *q,
           int *out_nodes)
{
    if (!q || !out_nodes) return QUO_ERR_INVLD_ARG;
    noinit_action(q);
    return quo_mpi_nnodes(q->mpi, out_nodes);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_nqids(QUO_t *q,
          int *out_nqids)
{
    if (!q || !out_nqids) return QUO_ERR_INVLD_ARG;
    noinit_action(q);
    return quo_mpi_nnoderanks(q->mpi, out_nqids);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_id(QUO_t *q,
       int *out_qid)
{
    if (!q || !out_qid) return QUO_ERR_INVLD_ARG;
    noinit_action(q);
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
    noinit_action(q);
    return quo_hwloc_bind_push(q->hwloc, policy, type, (unsigned)obj_index);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_bind_pop(QUO_t *q)
{
    if (!q) return QUO_ERR_INVLD_ARG;
    noinit_action(q);
    return quo_hwloc_bind_pop(q->hwloc);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_barrier(QUO_t *q)
{
    if (!q) return QUO_ERR_INVLD_ARG;
    noinit_action(q);
    return quo_mpi_sm_barrier(q->mpi);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_auto_distrib(QUO_t *q,
                 QUO_obj_type_t distrib_over_this,
                 int max_qids_per_res_type,
                 int *out_selected)
{
    /* total number of target resources. */
    int nres = 0;
    /* points to an array that stores the number of elements in the
     * rank_ids_in_res array at a particular resource index. */
    int *nranks_in_res = NULL;
    /* array of pointers that point to the smp ranks that cover a particular
     * hardware resource at a particular index. you can think of this as a 2D
     * matrix where [i][j] is the ith hardware resource that smp rank j covers. */
    int **rank_ids_in_res = NULL;
    int rc = QUO_ERR;
    /* my node (smp) rank */
    int my_smp_rank = 0, nsmp_ranks = 0;
    /* holds k set intersection info */
    int *k_set_intersection = NULL, k_set_intersection_len = 0;

    if (!q || !out_selected || max_qids_per_res_type <= 0) {
        return QUO_ERR_INVLD_ARG;
    }
    noinit_action(q);
    *out_selected = 0; /* set default */
    if (QUO_SUCCESS != (rc = QUO_nqids(q, &nsmp_ranks))) return rc;
    /* what is my node rank? */
    if (QUO_SUCCESS != (rc = QUO_id(q, &my_smp_rank))) return rc;
    /* figure out how many target things are on the system. */
    if (QUO_SUCCESS != (rc = QUO_nobjs_by_type(q, distrib_over_this,
                                                   &nres))) {
        return rc;
    }
    /* if there are no resources, then return not found */
    if (0 == nres) return QUO_ERR_NOT_FOUND;
    /* allocate some memory for our arrays */
    nranks_in_res = calloc(nres, sizeof(*nranks_in_res));
    if (!nranks_in_res) {
        QUO_OOR_COMPLAIN();
        rc = QUO_ERR_OOR;
        goto out;
    }
    /* allocate pointer array */
    rank_ids_in_res = calloc(nres, sizeof(*rank_ids_in_res));
    if (!rank_ids_in_res) {
        QUO_OOR_COMPLAIN();
        rc = QUO_ERR_OOR;
        goto out;
    }
    /* grab the smp ranks (node ranks) that cover each resource. */
    for (int rid = 0; rid < nres; ++rid) {
        rc = QUO_qids_in_type(q, distrib_over_this, rid,
                              &(nranks_in_res[rid]),
                              &(rank_ids_in_res[rid]));
        if (QUO_SUCCESS != rc) goto out;
    }

    /* calculate the k set intersection of ranks on resources. the returned
     * array will be the set of ranks that currently share a particular
     * resource. */
    rc = quo_set_get_k_set_intersection(nres, nranks_in_res,
                                        rank_ids_in_res,
                                        &k_set_intersection,
                                        &k_set_intersection_len);
    if (QUO_SUCCESS != rc) goto out;
    /* ////////////////////////////////////////////////////////////////////// */
    /* distribute workers over target resources. */
    /* ////////////////////////////////////////////////////////////////////// */

    /* !!! remember maintain "max workers per resource" !!! */

    /* completely disjoint sets, so making a local decision is easy */
    if (0 == k_set_intersection_len) {
        for (int rid = 0; rid < nres; ++rid) {
            /* if already a member, stop search */
            if (1 == *out_selected) break;
            for (int rank = 0; rank < nranks_in_res[rid]; ++rank) {
                /* if my current cpuset covers the resource in question and
                 * someone won't be assigned to that particular resource */
                if (my_smp_rank == rank_ids_in_res[rid][rank] &&
                    rank < max_qids_per_res_type) {
                    *out_selected = 1;
                }
            }
        }
    }
    /* all processes overlap - really no hope of doing anything sane. we
     * typically see this in the "no one is bound case." */
    else if (nsmp_ranks == k_set_intersection_len) {
        if (my_smp_rank < max_qids_per_res_type * nres) *out_selected = 1;
    }
    /* only a few ranks share a resource. i don't know if this case will ever
     * happen in practice, but i've seen stranger things... in the case, favor
     * unshared resources. */
    else {
        /* construct a "hash table" large enough to accommodate all possible
         * values up to nnoderanks - 1. note: these arrays are typically small, so
         * who cares. if this ever changes, then update the code to use a proper
         * hash table. */
        int *big_htab = NULL, rmapped = 0;
        size_t bhts = nsmp_ranks * sizeof(*big_htab);
        if (NULL == (big_htab = malloc(bhts))) {
            QUO_OOR_COMPLAIN();
            rc = QUO_ERR_OOR;
            goto out;
        }
        /* -1 = spot not taken */
        (void)memset(big_htab, -1, bhts);
        /* populate the hash table */
        for (int i = 0; i < k_set_intersection_len; ++i) {
            big_htab[k_set_intersection[i]] = k_set_intersection[i];
        }
        /* first only consider ranks that aren't sharing resources */
        for (int rid = 0; rid < nres; ++rid) {
            /* if already a member, stop search */
            if (1 == *out_selected) break;
            rmapped = 0;
            for (int rank = 0; rank < nranks_in_res[rid]; ++rank) {
                /* this thing is shared - skip */
                if (-1 != big_htab[rank_ids_in_res[rid][rank]]) continue;
                /* if my current cpuset covers the resource in question */
                if (my_smp_rank == rank_ids_in_res[rid][rank] &&
                    rmapped < max_qids_per_res_type) {
                        *out_selected = 1;
                        break;
                }
                ++rmapped;
            }
        }
        if (big_htab) free(big_htab);
    }
out:
    /* the resources returned by QUO_qids_in_type must be freed by us */
    if (rank_ids_in_res) {
        for (int i = 0; i < nres; ++i) {
            if (rank_ids_in_res[i]) free(rank_ids_in_res[i]);
        }
        free(rank_ids_in_res);
    }
    if (nranks_in_res) free(nranks_in_res);
    if (k_set_intersection) free(k_set_intersection);
    return rc;
}

/* ////////////////////////////////////////////////////////////////////////// */
/* fortran only. don't include in quo.h. */
int
QUO_ptr_free(void *p)
{
    if (p) free(p);
    return QUO_SUCCESS;
}
