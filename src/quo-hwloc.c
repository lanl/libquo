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
};

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
        hwloc_topology_destroy(*topo);
        qrc = QUO_ERR_TOPO;
        goto out;
    }

out:
    if (qrc != QUO_SUCCESS) {
        if (NULL != topo) free(topo);
        if (NULL != hwloc) free(hwloc);
        topo = NULL;
        hwloc = NULL;
    }
    else {
        hwloc->topo = topo;
    }
    *nhwloc = hwloc;
    return qrc;
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

        fprintf(stdout, "Objects at level %d\n", depth);
        for (i = 0; i < hwloc_get_nbobjs_by_depth(*(hwloc->topo), depth); ++i) {
            hwloc_obj_snprintf(sbuf, sizeof(sbuf), *(hwloc->topo),
                               hwloc_get_obj_by_depth(*(hwloc->topo), depth, i),
                               "#", 0);
            fprintf(stdout, "Index %u: %s\n", i, sbuf);

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
    int rc = 0;
    hwloc_cpuset_t set = NULL;

    if (NULL == hwloc || NULL == out_bound) return QUO_ERR_INVLD_ARG;

    if (NULL == (set = hwloc_bitmap_alloc())) {
        QUO_OOR_COMPLAIN();
        rc = QUO_ERR_OOR;
        goto out;
    }
    if (hwloc_get_proc_cpubind(*(hwloc->topo), pid, set, 0)) {
        int err = errno;
        fprintf(stderr, QUO_ERR_PREFIX"%s failure in %s: %d (%s)\n",
                "hwloc_get_proc_cpubind", __func__, err, strerror(err));
        rc = QUO_ERR_TOPO;
    }

    char *s = NULL;
    hwloc_bitmap_asprintf(&s, set);
    printf("%s\n", s);
    free(s);


    *out_bound = false;

out:
    hwloc_bitmap_free(set);
    return rc;
}
