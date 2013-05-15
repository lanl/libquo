/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quo-hwloc.h"

#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <errno.h>

/* ////////////////////////////////////////////////////////////////////////// */
/* quo_t type definition */
struct quo_hwloc_t {
    hwloc_topology_t *topo;
    /* the widest cpu set. primarily used for "is bound?" tests. */
    hwloc_cpuset_t widest_cpuset;
};

/* ////////////////////////////////////////////////////////////////////////// */
static int
init_cached_attrs(quo_hwloc_t *qh)
{
    if (NULL == qh) return QUO_ERR_INVLD_ARG;

    if (NULL == (qh->widest_cpuset = hwloc_bitmap_alloc())) {
        QUO_OOR_COMPLAIN();
        return QUO_ERR_OOR;
    }
    /* get the top-level obj -- the system */
    hwloc_obj_t sysobj = hwloc_get_root_obj(*qh->topo);
    /* stash the system's cpuset */
    hwloc_bitmap_copy(qh->widest_cpuset, sysobj->cpuset);
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_hwloc_construct(quo_hwloc_t **nhwloc)
{
    int qrc = QUO_SUCCESS;
    int rc = 0;
    quo_hwloc_t *hwloc = NULL;
    hwloc_topology_t *topo = NULL;

    if (NULL == nhwloc) return QUO_ERR_INVLD_ARG;

    if (NULL == (hwloc = calloc(1, sizeof(*hwloc)))) {
        QUO_OOR_COMPLAIN();
        qrc = QUO_ERR_OOR;
        goto out;
    }
    if (NULL == (topo = calloc(1, sizeof(*topo)))) {
        QUO_OOR_COMPLAIN();
        qrc = QUO_ERR_OOR;
        goto out;
    }
    if (0 != (rc = hwloc_topology_init(topo))) {
        fprintf(stderr, QUO_ERR_PREFIX"%s failure: (rc: %d). "
                "Cannot continue.\n", "hwloc_topology_init", rc);
        qrc = QUO_ERR_TOPO;
        goto out;
    }
    if (0 != (rc = hwloc_topology_load(*topo))) {
        fprintf(stderr, QUO_ERR_PREFIX"%s failure: (rc: %d). "
                "Cannot continue.\n", "hwloc_topology_load", rc);
        qrc = QUO_ERR_TOPO;
        goto out;
    }
    /* set before calling init_cached_attrs */
    hwloc->topo = topo;
    /* now init some cached attributes that we want to keep around for the
     * duration of the app's life. */
    if (QUO_SUCCESS != (qrc = init_cached_attrs(hwloc))) {
        fprintf(stderr, QUO_ERR_PREFIX"%s failure: (rc: %d). "
                "Cannot continue.\n", "init_cached_attrs", qrc);
        qrc = QUO_ERR;
        goto out;
    }
out:
    if (qrc != QUO_SUCCESS) {
        (void)quo_hwloc_destruct(hwloc);
        *nhwloc = NULL;
    }
    *nhwloc = hwloc;
    return qrc;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_hwloc_destruct(quo_hwloc_t *nhwloc)
{
    if (NULL == nhwloc) return QUO_ERR_INVLD_ARG;

    if (nhwloc->topo) {
        hwloc_topology_destroy(*(nhwloc->topo));
        free(nhwloc->topo);
        nhwloc->topo = NULL;
    }
    if (nhwloc->widest_cpuset) {
        hwloc_bitmap_free(nhwloc->widest_cpuset);
    }
    free(nhwloc);
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_hwloc_node_topo_emit(const quo_hwloc_t *hwloc)
{
    int topo_depth = 0;
    int depth = 0;
    unsigned int i = 0;
    char sbuf[256];

    if (NULL == hwloc) return QUO_ERR_INVLD_ARG;

    (void)memset(sbuf, '\0', sizeof(sbuf));

    topo_depth = hwloc_topology_get_depth(*(hwloc->topo));

    for (depth = 0; depth < topo_depth; ++depth) {
        fprintf(stdout, "objects at level %d\n", depth);
        for (i = 0; i < hwloc_get_nbobjs_by_depth(*(hwloc->topo), depth); ++i) {
            hwloc_obj_snprintf(sbuf, sizeof(sbuf), *(hwloc->topo),
                               hwloc_get_obj_by_depth(*(hwloc->topo), depth, i),
                               " #", 0);
            fprintf(stdout, "index %u: %s\n", i, sbuf);
        }
    }
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_hwloc_sockets(const quo_hwloc_t *hwloc,
                  int *nsockets)
{
    int depth = 0;

    if (NULL == hwloc || NULL == nsockets) return QUO_ERR_INVLD_ARG;

    depth = hwloc_get_type_depth(*(hwloc->topo), HWLOC_OBJ_SOCKET);

    if (HWLOC_TYPE_DEPTH_UNKNOWN == depth) {
        /* hwloc can't determine the number of sockets, so just return 0 */
        *nsockets = 0;
        return QUO_ERR_NOT_SUPPORTED;
    }
    else {
        *nsockets = hwloc_get_nbobjs_by_depth(*(hwloc->topo), depth);
    }
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_hwloc_bound(const quo_hwloc_t *hwloc,
                hwloc_pid_t pid,
                bool *out_bound)
{
    hwloc_topology_t *topo = NULL;
    int rc = 0;
    hwloc_cpuset_t cur_bind = NULL;

    if (NULL == hwloc || NULL == out_bound) return QUO_ERR_INVLD_ARG;

    topo = hwloc->topo;

    if (NULL == (cur_bind = hwloc_bitmap_alloc())) {
        QUO_OOR_COMPLAIN();
        rc = QUO_ERR_OOR;
        goto out;
    }
    if (hwloc_get_cpubind(*topo, cur_bind, HWLOC_CPUBIND_PROCESS)) {
        int err = errno;
        fprintf(stderr, QUO_ERR_PREFIX"%s failure in %s: %d (%s)\n",
                "hwloc_get_proc_cpubind", __func__, err, strerror(err));
        rc = QUO_ERR_TOPO;
    }
    /* if our current binding isn't equal to the widest, then we are bound to
     * something smaller than the widest. so, at least as far as we are
     * concerned, the process is "bound." */
    *out_bound = !hwloc_bitmap_isequal(hwloc->widest_cpuset, cur_bind);

out:
    if (cur_bind) hwloc_bitmap_free(cur_bind);
    return rc;
}
