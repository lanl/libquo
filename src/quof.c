/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#include "quo.h"
#include "quof-private.h"


/* ////////////////////////////////////////////////////////////////////////// */
/* fortran public api routines */
/* ////////////////////////////////////////////////////////////////////////// */

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_version_f(int *version,
              int *subversion,
              int *ierr)
{
    int cerr = QUO_version(version, subversion);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_VERSION,
                          quo_version,
                          quo_version_,
                          quo_version__,
                          quo_version_f,
                          (int *version, int *subversion, int *ierr),
                          (version, subversion, ierr) )

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_construct_f(QUO_f_t *q,
                int *ierr)
{
    QUO_t *context = NULL;
    int cerr = QUO_construct(&context);
    *q = (QUO_f_t)context;
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_CONSTRUCT,
                          quo_construct,
                          quo_construct_,
                          quo_construct__,
                          quo_construct_f,
                          (QUO_f_t *q, int *ierr),
                          (q, ierr) )

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_init_f(QUO_f_t *q,
           int *ierr)
{
    int cerr = QUO_init((QUO_t *)*q);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_INIT,
                          quo_init,
                          quo_init_,
                          quo_init__,
                          quo_init_f,
                          (QUO_f_t *q, int *ierr),
                          (q, ierr) )

void
quo_nsockets_f(int *q,
             int *out_nsockets)
{
    //int cerr = QUO_nsockets(
}
#if 0
/* ////////////////////////////////////////////////////////////////////////// */
void
QUO_free(void *target)
{
    if (target) free(target);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_init(QUO_t *q)
{
    int rc = QUO_ERR;
    if (!q) return QUO_ERR_INVLD_ARG;
    /* if this context is already initialized, then just return success */
    if (q->initialized) return QUO_SUCCESS_ALREADY_DONE;
    /* else init the context */
    if (QUO_SUCCESS != (rc = quo_mpi_init(q->mpi))) {
        fprintf(stderr, QUO_ERR_PREFIX"%s failed. Cannot continue.\n",
                "quo_mpi_init");
        return rc;
    }
    q->initialized = true;
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_initialized(const QUO_t *q,
                int *out_initialized)
{
    if (!out_initialized || !q) return QUO_ERR_INVLD_ARG;
    *out_initialized = (int)q->initialized;
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_construct(QUO_t **q)
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
        QUO_destruct(newq);
        *q = NULL;
    }
    *q = newq;
    return qrc;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_destruct(QUO_t *q)
{
    int nerrs = 0;
    if (!q) return QUO_ERR_INVLD_ARG;
    /* we can call destruct before init. useful in error paths. */
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
QUO_finalize(QUO_t *q)
{
    if (!q) return QUO_ERR_INVLD_ARG;
    noinit_action(q);
    /* nothing really to do here at this point, but we may need this routine at
     * some point so keep it around. */
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_node_topo_stringify(const QUO_t *q,
                        char **out_str)
{
    /* make sure we are initialized before we continue */
    noinit_action(q);
    if (!q || !out_str) return QUO_ERR_INVLD_ARG;
    return quo_hwloc_node_topo_stringify(q->hwloc, out_str);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_get_nobjs_in_type_by_type(const QUO_t *q,
                              QUO_obj_type_t in_type,
                              int in_type_index,
                              QUO_obj_type_t type,
                              int *out_result)
{
    /* make sure we are initialized before we continue */
    noinit_action(q);
    if (!q || !out_result) return QUO_ERR_INVLD_ARG;
    return quo_hwloc_get_nobjs_in_type_by_type(q->hwloc,
                                               in_type,
                                               (unsigned)in_type_index,
                                               type,
                                               out_result);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_cur_cpuset_in_type(const QUO_t *q,
                       QUO_obj_type_t type,
                       int in_type_index,
                       int *out_result)
{
    /* make sure we are initialized before we continue */
    noinit_action(q);
    if (!q || !out_result) return QUO_ERR_INVLD_ARG;
    return quo_hwloc_is_in_cpuset_by_type_id(q->hwloc, type, q->pid,
                                             (unsigned)in_type_index,
                                             out_result);
}

/* ////////////////////////////////////////////////////////////////////////// */
/**
 * caller is responsible for freeing *out_smpranks.
 */
int
QUO_smpranks_in_type(const QUO_t *q,
                     QUO_obj_type_t type,
                     int in_type_index,
                     int *n_out_smpranks,
                     int **out_smpranks)
{
    int rc = QUO_ERR;
    int tot_smpranks = 0;
    int nsmpranks = 0;
    int *smpranks = NULL;

    if (!q || !n_out_smpranks || !out_smpranks) return QUO_ERR_INVLD_ARG;
    *n_out_smpranks = 0; *out_smpranks = NULL;
    /* figure out how many node ranks on the node */
    if (QUO_SUCCESS != (rc = QUO_nnoderanks(q, &tot_smpranks))) return rc;
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
            int *newsmpranks = NULL;
            newsmpranks = realloc(smpranks, (nsmpranks + 1) * sizeof(int));
            if (!newsmpranks) {
                rc = QUO_ERR_OOR;
                goto out;
            }
            smpranks = newsmpranks;
            /* add the newly found rank to the list and increment num found */
            smpranks[nsmpranks++] = rank;
        }
    }
    *n_out_smpranks = nsmpranks;
    *out_smpranks = smpranks;
out:
    if (QUO_SUCCESS != rc) {
        if (smpranks) free(smpranks);
    }
    return rc;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_get_nobjs_by_type(const QUO_t *q,
                      QUO_obj_type_t target_type,
                      int *out_nobjs)
{
    /* make sure we are initialized before we continue */
    noinit_action(q);
    if (!q || !out_nobjs) return QUO_ERR_INVLD_ARG;
    return quo_hwloc_get_nobjs_by_type(q->hwloc, target_type, out_nobjs);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_nsockets(const QUO_t *q,
             int *out_nsockets)
{
    /* make sure we are initialized before we continue */
    noinit_action(q);
    if (!q || !out_nsockets) return QUO_ERR_INVLD_ARG;
    return quo_hwloc_get_nobjs_by_type(q->hwloc, QUO_OBJ_SOCKET, out_nsockets);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_ncores(const QUO_t *q,
           int *out_ncores)
{
    /* make sure we are initialized before we continue */
    noinit_action(q);
    if (!q || !out_ncores) return QUO_ERR_INVLD_ARG;
    return quo_hwloc_get_nobjs_by_type(q->hwloc, QUO_OBJ_CORE, out_ncores);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_npus(const QUO_t *q,
         int *out_npus)
{
    /* make sure we are initialized before we continue */
    noinit_action(q);
    if (!q || !out_npus) return QUO_ERR_INVLD_ARG;
    return quo_hwloc_get_nobjs_by_type(q->hwloc, QUO_OBJ_PU, out_npus);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_bound(const QUO_t *q,
          int *bound)
{
    int rc = QUO_ERR;
    bool bound_b = false;
    /* make sure we are initialized before we continue */
    noinit_action(q);
    if (!q || !bound) return QUO_ERR_INVLD_ARG;
    if (QUO_SUCCESS != (rc = quo_hwloc_bound(q->hwloc, q->pid, &bound_b))) {
        return rc;
    }
    *bound = (int)bound_b;
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_stringify_cbind(const QUO_t *q,
                    char **cbind_str)
{
    noinit_action(q);
    if (!q || !cbind_str) return QUO_ERR_INVLD_ARG;
    return quo_hwloc_stringify_cbind(q->hwloc, q->pid, cbind_str);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_nnodes(const QUO_t *q,
           int *out_nodes)
{
    noinit_action(q);
    if (!q || !out_nodes) return QUO_ERR_INVLD_ARG;
    return quo_mpi_nnodes(q->mpi, out_nodes);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_nnoderanks(const QUO_t *q,
               int *out_nnoderanks)
{
    noinit_action(q);
    if (!q || !out_nnoderanks) return QUO_ERR_INVLD_ARG;
    return quo_mpi_nnoderanks(q->mpi, out_nnoderanks);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_noderank(const QUO_t *q,
             int *out_noderank)
{
    noinit_action(q);
    if (!q || !out_noderank) return QUO_ERR_INVLD_ARG;
    return quo_mpi_noderank(q->mpi, out_noderank);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_bind_push(QUO_t *q,
              QUO_bind_push_policy_t policy,
              QUO_obj_type_t type,
              int obj_index)
{
    noinit_action(q);
    if (!q) return QUO_ERR_INVLD_ARG;
    return quo_hwloc_bind_push(q->hwloc, policy, type, (unsigned)obj_index);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_bind_pop(QUO_t *q)
{
    noinit_action(q);
    if (!q) return QUO_ERR_INVLD_ARG;
    return quo_hwloc_bind_pop(q->hwloc);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_ranks_on_node(const QUO_t *q,
                  int *out_nranks,
                  int **out_ranks)
{
    noinit_action(q);
    if (!out_nranks || !out_ranks) return QUO_ERR_INVLD_ARG;
    return quo_mpi_ranks_on_node(q->mpi, out_nranks, out_ranks);
}
#endif
