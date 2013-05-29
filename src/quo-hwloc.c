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

/* should be plenty */
#define BIND_STACK_SIZE 128

/* the almighty bind stack */
typedef struct bind_stack_t {
    /* top of the stack */
    int top;
    /* bind stack */
    hwloc_cpuset_t bind_stack[BIND_STACK_SIZE];
} bind_stack_t;

/* quo_hwloc_t type definition */
struct quo_hwloc_t {
    /* the system's topology */
    hwloc_topology_t topo;
    /* the widest cpu set. primarily used for "is bound?" tests. */
    hwloc_cpuset_t widest_cpuset;
    /* the bind stack */
    bind_stack_t bstack;
};

/* ////////////////////////////////////////////////////////////////////////// */
static int
ext2intobj(quo_obj_type_t external,
           hwloc_obj_type_t *internal)
{
    if (!internal) return QUO_ERR_INVLD_ARG;
    /* convert from ours to hwloc's */
    switch (external) {
        case QUO_MACHINE:
            *internal = HWLOC_OBJ_MACHINE;
            break;
        case QUO_NODE:
            *internal = HWLOC_OBJ_NODE;
            break;
        case QUO_SOCKET:
            *internal = HWLOC_OBJ_SOCKET;
            break;
        case QUO_CORE:
            *internal = HWLOC_OBJ_CORE;
            break;
        case QUO_PU:
            *internal = HWLOC_OBJ_PU;
            break;
        default:
            /* well, we'll just return the machine if something weird was passed
             * to us. check your return codes, folks! */
            *internal = HWLOC_OBJ_MACHINE;
            return QUO_ERR_INVLD_ARG;
    }
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
static int
get_cur_bind(const quo_hwloc_t *hwloc,
             hwloc_cpuset_t *out_cpuset)
{
    int rc = QUO_SUCCESS;
    hwloc_cpuset_t cur_bind = NULL;

    if (!hwloc || !out_cpuset) return QUO_ERR_INVLD_ARG;

    if (NULL == (cur_bind = hwloc_bitmap_alloc())) {
        QUO_OOR_COMPLAIN();
        rc = QUO_ERR_OOR;
        goto out;
    }
    if (hwloc_get_cpubind(hwloc->topo, cur_bind, HWLOC_CPUBIND_PROCESS)) {
        int err = errno;
        fprintf(stderr, QUO_ERR_PREFIX"%s failure in %s: %d (%s)\n",
                "hwloc_get_proc_cpubind", __func__, err, strerror(err));
        rc = QUO_ERR_TOPO;
        goto out;
    }
    /* caller is responsible for calling hwloc_bitmap_free */
    *out_cpuset = cur_bind;
out:
    /* cleanup on failure */
    if (QUO_SUCCESS != rc) {
        if (cur_bind) hwloc_bitmap_free(cur_bind);
        *out_cpuset = NULL;
    }
    return rc;
}

/* ////////////////////////////////////////////////////////////////////////// */
static bool
bind_stack_full(const quo_hwloc_t *hwloc)
{
    return hwloc->bstack.top >= BIND_STACK_SIZE;
}

/* ////////////////////////////////////////////////////////////////////////// */
static int
bind_stack_push(quo_hwloc_t *hwloc,
                hwloc_cpuset_t cpuset)
{
    unsigned top = hwloc->bstack.top;

    if (!hwloc) return QUO_ERR_INVLD_ARG;
    /* stack is full - we are out of resources */
    if (bind_stack_full(hwloc)) return QUO_ERR_OOR;
    /* pop will cleanup after this call */
    if (NULL == (hwloc->bstack.bind_stack[top] = hwloc_bitmap_alloc())) {
        QUO_OOR_COMPLAIN();
        return QUO_ERR_OOR;
    }
    /* copy the thing */
    hwloc_bitmap_copy(hwloc->bstack.bind_stack[top], cpuset);
    /* update top */
    hwloc->bstack.top++;
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
static int
bind_stack_pop(quo_hwloc_t *hwloc,
               hwloc_cpuset_t *popped)
{
    if (!hwloc || !popped) return QUO_ERR_INVLD_ARG;
    /* stack is empty -- nothing to do */
    if (hwloc->bstack.top < 0) return QUO_ERR_POP;
    if (NULL == (*popped = hwloc_bitmap_alloc())) {
        QUO_OOR_COMPLAIN();
        return QUO_ERR_OOR;
    }
    hwloc_bitmap_copy(*popped, hwloc->bstack.bind_stack[hwloc->bstack.top - 1]);
    /* free the top and adjust the top of the stack */
    hwloc_bitmap_free(hwloc->bstack.bind_stack[hwloc->bstack.top--]);
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
/**
 * pushed current binding.
 */
static int
push_cur_bind(quo_hwloc_t *hwloc)
{
    int rc = QUO_SUCCESS;
    hwloc_cpuset_t cur_bind = NULL;

    if (!hwloc) return QUO_ERR_INVLD_ARG;

    if (QUO_SUCCESS != (rc = get_cur_bind(hwloc, &cur_bind))) return rc;
    if (QUO_SUCCESS != (rc = bind_stack_push(hwloc, cur_bind))) goto out;
out:
    /* push copies, so free the one we created */
    if (cur_bind) free(cur_bind);
    return rc;
}

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
    hwloc_obj_t sysobj = hwloc_get_root_obj(qh->topo);
    /* stash the system's cpuset */
    hwloc_bitmap_copy(qh->widest_cpuset, sysobj->cpuset);
    /* push our current binding */
    return push_cur_bind(qh);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_hwloc_construct(quo_hwloc_t **nhwloc)
{
    int qrc = QUO_SUCCESS;
    int rc = 0;
    quo_hwloc_t *hwloc = NULL;

    if (NULL == nhwloc) return QUO_ERR_INVLD_ARG;

    if (NULL == (hwloc = calloc(1, sizeof(*hwloc)))) {
        QUO_OOR_COMPLAIN();
        qrc = QUO_ERR_OOR;
        goto out;
    }
    if (0 != (rc = hwloc_topology_init(&(hwloc->topo)))) {
        fprintf(stderr, QUO_ERR_PREFIX"%s failure: (rc: %d). "
                "Cannot continue.\n", "hwloc_topology_init", rc);
        qrc = QUO_ERR_TOPO;
        goto out;
    }
    if (0 != (rc = hwloc_topology_load(hwloc->topo))) {
        fprintf(stderr, QUO_ERR_PREFIX"%s failure: (rc: %d). "
                "Cannot continue.\n", "hwloc_topology_load", rc);
        qrc = QUO_ERR_TOPO;
        goto out;
    }
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
    else {
        *nhwloc = hwloc;
    }
    return qrc;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_hwloc_destruct(quo_hwloc_t *nhwloc)
{
    if (NULL == nhwloc) return QUO_ERR_INVLD_ARG;

    hwloc_topology_destroy(nhwloc->topo);
    hwloc_bitmap_free(nhwloc->widest_cpuset);
    free(nhwloc);
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_hwloc_node_topo_stringify(const quo_hwloc_t *hwloc,
                              char **out_str)
{
    int topo_depth = 0;
    int depth = 0;
    int cwritten = 0;
    unsigned int i = 0;
    char sbuf[1024];
    char *retstr = NULL, *tmpstr = NULL, *tmpp = NULL;

    if (!hwloc || !out_str) return QUO_ERR_INVLD_ARG;

    (void)memset(sbuf, '\0', sizeof(sbuf));
    topo_depth = hwloc_topology_get_depth(hwloc->topo);
    for (depth = 0; depth < topo_depth; ++depth) {
        cwritten = asprintf(&tmpstr, "objects at level %d\n", depth);
        if (-1 == cwritten) return QUO_ERR_OOR;
        tmpp = retstr;
        /* just appending tmpstr to retstr */
        cwritten = asprintf(&retstr, "%s%s", retstr ? retstr : "", tmpstr);
        if (-1 == cwritten) return QUO_ERR_OOR;
        free(tmpstr);
        if (tmpp) free(tmpp);
        for (i = 0; i < hwloc_get_nbobjs_by_depth(hwloc->topo, depth); ++i) {
            hwloc_obj_snprintf(sbuf, sizeof(sbuf) - 1, hwloc->topo,
                               hwloc_get_obj_by_depth(hwloc->topo, depth, i),
                               " #", 0);
            tmpp = retstr;
            cwritten = asprintf(&retstr, "%sindex %u: %s\n", retstr, i, sbuf);
            if (-1 == cwritten) return QUO_ERR_OOR;
            free(tmpp);
        }
    }
    /* caller is responsible for freeing returned resources */
    *out_str = retstr;
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
/**
 * return the number of target_types.
 */
static int
getnx(const quo_hwloc_t *hwloc,
      hwloc_obj_type_t target_type,
      int *nx)
{
    int depth = 0;

    if (NULL == hwloc || NULL == nx) return QUO_ERR_INVLD_ARG;

    depth = hwloc_get_type_depth(hwloc->topo, target_type);
    if (HWLOC_TYPE_DEPTH_UNKNOWN == depth) {
        /* hwloc can't determine the number of x, so just return 0 and not
         * supported. */
        *nx = 0;
        return QUO_ERR_NOT_SUPPORTED;
    }
    else {
        *nx = hwloc_get_nbobjs_by_depth(hwloc->topo, depth);
    }
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_hwloc_sockets(const quo_hwloc_t *hwloc,
                  int *nsockets)
{
    if (NULL == hwloc || NULL == nsockets) return QUO_ERR_INVLD_ARG;
    return getnx(hwloc, HWLOC_OBJ_SOCKET, nsockets);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_hwloc_cores(const quo_hwloc_t *hwloc,
                int *ncores)
{
    if (NULL == hwloc || NULL == ncores) return QUO_ERR_INVLD_ARG;
    return getnx(hwloc, HWLOC_OBJ_CORE, ncores);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_hwloc_pus(const quo_hwloc_t *hwloc,
              int *npus)
{
    if (NULL == hwloc || NULL == npus) return QUO_ERR_INVLD_ARG;
    return getnx(hwloc, HWLOC_OBJ_PU, npus);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_hwloc_bound(const quo_hwloc_t *hwloc,
                bool *out_bound)
{
    int rc = 0;
    hwloc_cpuset_t cur_bind = NULL;

    if (NULL == hwloc || NULL == out_bound) return QUO_ERR_INVLD_ARG;

    if (QUO_SUCCESS != (rc = get_cur_bind(hwloc, &cur_bind))) {
        goto out;
    }
    /* if our current binding isn't equal to the widest, then we are bound to
     * something smaller than the widest. so, at least as far as we are
     * concerned, the process is "bound." */
    *out_bound = !hwloc_bitmap_isequal(hwloc->widest_cpuset, cur_bind);
out:
    if (cur_bind) hwloc_bitmap_free(cur_bind);
    return rc;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_hwloc_stringify_cbind(const quo_hwloc_t *hwloc,
                          char **out_str)
{
    int rc = QUO_SUCCESS;
    hwloc_cpuset_t cur_bind = NULL;

    if (!hwloc || !out_str) return QUO_ERR_INVLD_ARG;

    if (QUO_SUCCESS != (rc = get_cur_bind(hwloc, &cur_bind))) {
        /* get_cur_bind cleans up after itself on failure */
        return rc;
    }
    /* caller is responsible for freeing returned resources */
    hwloc_bitmap_asprintf(out_str, cur_bind);
    if (!out_str) {
        QUO_OOR_COMPLAIN();
        rc = QUO_ERR_OOR;
    }
    if (cur_bind) hwloc_bitmap_free(cur_bind);
    return rc;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_hwloc_rebind(const quo_hwloc_t *hwloc,
                 quo_obj_type_t type,
                 unsigned obj_index)
{
    int rc = QUO_SUCCESS;
    hwloc_obj_t target_obj = NULL;
    hwloc_cpuset_t cpu_set = NULL;
    hwloc_obj_type_t real_type = HWLOC_OBJ_MACHINE;

    if (!hwloc) return QUO_ERR_INVLD_ARG;
    if (QUO_SUCCESS != (rc = ext2intobj(type, &real_type))) return rc;
    if (NULL == (target_obj = hwloc_get_obj_by_type(hwloc->topo,
                                                    real_type,
                                                    obj_index))) {
        /* there are a couple of reasons why target_obj may be NULL. if this
         * ever happens and the specified type and obj index should be valid,
         * then read the hwloc documentation and make this code mo betta. */
        return QUO_ERR_INVLD_ARG;
    }
    if (NULL == (cpu_set = hwloc_bitmap_alloc())) return QUO_ERR_OOR;
    /* void func */
    hwloc_bitmap_copy(cpu_set, target_obj->cpuset);
    if (-1 == hwloc_set_cpubind(hwloc->topo, cpu_set,
                                HWLOC_CPUBIND_PROCESS)) {
        rc = QUO_ERR_NOT_SUPPORTED;
        goto out;
    }
out:
    if (cpu_set) hwloc_bitmap_free(cpu_set);
    return rc;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_hwloc_bind_push(quo_hwloc_t *hwloc,
                    quo_obj_type_t type,
                    unsigned obj_index)
{
    int rc = QUO_SUCCESS;

    if (!hwloc) return QUO_ERR_INVLD_ARG;
    /* change binding */
    if (QUO_SUCCESS != (rc = quo_hwloc_rebind(hwloc, type, obj_index))) {
        return rc;
    }
    /* stash our shiny new binding */
    return push_cur_bind(hwloc);
}

/* ////////////////////////////////////////////////////////////////////////// */
/* XXX return popped val? */
int
quo_hwloc_bind_pop(quo_hwloc_t *hwloc)
{
    int rc = QUO_SUCCESS;
    hwloc_cpuset_t topbind = NULL;

    if (!hwloc) return QUO_ERR_INVLD_ARG;

    if (QUO_SUCCESS != (rc = bind_stack_pop(hwloc, &topbind))) return rc;
    /* revert to the top binding */
    if (-1 == hwloc_set_cpubind(hwloc->topo, topbind,
                                HWLOC_CPUBIND_PROCESS)) {
        rc = QUO_ERR_NOT_SUPPORTED;
        goto out;
    }
out:
    if (topbind) hwloc_bitmap_free(topbind);
    return rc;
}
