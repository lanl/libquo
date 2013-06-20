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
    /* my pid */
    pid_t mypid;
};

/* ////////////////////////////////////////////////////////////////////////// */
static bool
valid_bind_policy(quo_bind_push_policy_t policy)
{
    switch (policy) {
        case QUO_BIND_PUSH_PROVIDED:
        case QUO_BIND_PUSH_OBJ:
            return true;
        default:
            return false;
    }
    return false;
}

/* ////////////////////////////////////////////////////////////////////////// */
/**
 * takes a quo obj type and converts it to hwloc's equivalent.
 */
static int
ext2intobj(quo_obj_type_t external,
           hwloc_obj_type_t *internal)
{
    if (!internal) return QUO_ERR_INVLD_ARG;
    /* convert from ours to hwloc's. if you ever need more types, add them here
     * and in quo.h. */
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
/**
 * caller is responsible for freeing returned resources.
 */
static int
get_cur_bind(const quo_hwloc_t *hwloc,
             pid_t who_pid,
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
    if (hwloc_get_proc_cpubind(hwloc->topo,
                               who_pid,
                               cur_bind,
                               HWLOC_CPUBIND_PROCESS)) {
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
static int
get_obj_by_type(const quo_hwloc_t *hwloc,
                quo_obj_type_t type,
                unsigned type_index,
                hwloc_obj_t *out_obj)
{
    int rc = QUO_ERR;
    hwloc_obj_type_t real_type = HWLOC_OBJ_MACHINE;

    if (!hwloc || !out_obj) return QUO_ERR_INVLD_ARG;
    *out_obj = NULL;
    if (QUO_SUCCESS != (rc = ext2intobj(type, &real_type))) return rc;
    if (NULL == (*out_obj = hwloc_get_obj_by_type(hwloc->topo,
                                                  real_type,
                                                  type_index))) {
        /* there are a couple of reasons why target_obj may be NULL. if this
         * ever happens and the specified type and obj index should be valid,
         * then read the hwloc documentation and make this code mo betta. */
        return QUO_ERR_INVLD_ARG;
    }
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
static int
get_obj_covering_cur_bind(const quo_hwloc_t *hwloc,
                          quo_obj_type_t type,
                          hwloc_obj_t *out_obj)
{
    int rc = QUO_ERR;
    hwloc_cpuset_t curbind = NULL;
    hwloc_obj_type_t real_type = HWLOC_OBJ_MACHINE;

    if (!hwloc || !out_obj) return QUO_ERR_INVLD_ARG;
    if (QUO_SUCCESS != (rc = ext2intobj(type, &real_type))) return rc;
    if (QUO_SUCCESS != (rc = get_cur_bind(hwloc, hwloc->mypid, &curbind))) {
        return rc;
    }
    *out_obj = hwloc_get_next_obj_covering_cpuset_by_type(hwloc->topo, curbind,
                                                          real_type, NULL);
    if (!*out_obj) {
        rc = QUO_ERR_NOT_FOUND;
        goto out;
    }
out:
    if (curbind) hwloc_bitmap_free(curbind);
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
    if (!hwloc) return QUO_ERR_INVLD_ARG;
    /* stack is empty -- nothing to do */
    if (hwloc->bstack.top <= 0) return QUO_ERR_POP;
    /* remember top is the next empty slot, so decrement first */
    hwloc->bstack.top--;
    /* if the caller wants a copy, give it to them */
    if (popped) {
        if (NULL == (*popped = hwloc_bitmap_alloc())) {
            QUO_OOR_COMPLAIN();
            /* restore top's val in error path */
            hwloc->bstack.top++;
            return QUO_ERR_OOR;
        }
        hwloc_bitmap_copy(*popped, hwloc->bstack.bind_stack[hwloc->bstack.top]);
    }
    /* free the top */
    hwloc_bitmap_free(hwloc->bstack.bind_stack[hwloc->bstack.top]);
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
static int
bind_stack_top(quo_hwloc_t *hwloc,
               hwloc_cpuset_t *top_copy)
{
    if (!hwloc || !top_copy) return QUO_ERR_INVLD_ARG;
    /* stack is empty -- nothing to do */
    if (hwloc->bstack.top <= 0) return QUO_ERR_POP;
    if (NULL == (*top_copy = hwloc_bitmap_alloc())) {
        QUO_OOR_COMPLAIN();
        return QUO_ERR_OOR;
    }
    /* remember top is the next empty slot, so decrement first */
    hwloc->bstack.top--;
    /* copy */
    hwloc_bitmap_copy(*top_copy, hwloc->bstack.bind_stack[hwloc->bstack.top]);
    /* restore top */
    hwloc->bstack.top++;
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
/**
 * push current binding.
 */
static int
push_cur_bind(quo_hwloc_t *hwloc)
{
    int rc = QUO_SUCCESS;
    hwloc_cpuset_t cur_bind = NULL;

    if (!hwloc) return QUO_ERR_INVLD_ARG;

    if (QUO_SUCCESS != (rc = get_cur_bind(hwloc, hwloc->mypid, &cur_bind))) {
        return rc;
    }
    if (QUO_SUCCESS != (rc = bind_stack_push(hwloc, cur_bind))) goto out;
out:
    /* push copies, so free the one we created */
    if (cur_bind) hwloc_bitmap_free(cur_bind);
    return rc;
}

/* ////////////////////////////////////////////////////////////////////////// */
static int
init_cached_attrs(quo_hwloc_t *qh)
{
    if (NULL == qh) return QUO_ERR_INVLD_ARG;

    /* stash our pid */
    qh->mypid = getpid();
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
quo_hwloc_destruct(quo_hwloc_t *hwloc)
{
    if (NULL == hwloc) return QUO_ERR_INVLD_ARG;

    hwloc_topology_destroy(hwloc->topo);
    hwloc_bitmap_free(hwloc->widest_cpuset);
    /* pop initial binding to free up resources */
    bind_stack_pop(hwloc, NULL);
    free(hwloc);
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
int
quo_hwloc_get_nobjs_in_type_by_type(const quo_hwloc_t *hwloc,
                                    quo_obj_type_t in_type,
                                    unsigned in_type_index,
                                    quo_obj_type_t type,
                                    int *out_result)
{
    int rc = QUO_ERR;
    hwloc_obj_t obj = NULL;
    hwloc_cpuset_t cpu_set = NULL;
    hwloc_obj_type_t real_type = HWLOC_OBJ_MACHINE;
    int nobjs = 0;

    if (!hwloc || !out_result) return QUO_ERR_INVLD_ARG;
    /* set this to something nice just in case an error occurs */
    *out_result = 0;
    /* now get the "in" object. like: what's the number of PUs *in* the 0th
     * socket. target_obj in this case corresponds to the 0th socket. */
    if (QUO_SUCCESS != (rc = get_obj_by_type(hwloc,
                                             in_type,
                                             in_type_index,
                                             &obj))) {
        return rc;
    }
    if (NULL == (cpu_set = hwloc_bitmap_alloc())) {
        QUO_OOR_COMPLAIN();
        return QUO_ERR_OOR;
    }
    /* copy the cpuset of the in target -- do we need this? */
    hwloc_bitmap_copy(cpu_set, obj->cpuset);
    if (QUO_SUCCESS != (rc = ext2intobj(type, &real_type))) goto out;
    /* set to NULL so the next call works properly */
    obj = NULL;
    /* now count */
    while ((obj = hwloc_get_next_obj_inside_cpuset_by_type(hwloc->topo,
                                                           cpu_set,
                                                           real_type,
                                                           obj))) {
        ++nobjs;
    }
    *out_result = nobjs;
out:
    if (cpu_set) hwloc_bitmap_free(cpu_set);
    if (QUO_SUCCESS != rc) *out_result = 0;
    return rc;
}

/* ////////////////////////////////////////////////////////////////////////// */
/**
 * returns the total amount of objects on the system.
 */
int
quo_hwloc_get_nobjs_by_type(const quo_hwloc_t *hwloc,
                            quo_obj_type_t target_type,
                            int *out_nobjs)
{
    int depth = 0, rc = QUO_ERR;
    hwloc_obj_type_t real_type = HWLOC_OBJ_MACHINE;

    if (!hwloc || !out_nobjs) return QUO_ERR_INVLD_ARG;
    if (QUO_SUCCESS != (rc = ext2intobj(target_type, &real_type))) return rc;
    depth = hwloc_get_type_depth(hwloc->topo, real_type);
    if (HWLOC_TYPE_DEPTH_UNKNOWN == depth) {
        /* hwloc can't determine the number of x, so just return 0 and not
         * supported. */
        *out_nobjs = 0;
        return QUO_ERR_NOT_SUPPORTED;
    }
    else {
        *out_nobjs = hwloc_get_nbobjs_by_depth(hwloc->topo, depth);
    }
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_hwloc_is_in_cpuset_by_type_id(const quo_hwloc_t *hwloc,
                                  quo_obj_type_t type,
                                  pid_t pid,
                                  unsigned type_index,
                                  int *out_result)
{
    int rc = QUO_ERR;
    hwloc_obj_t obj = NULL;
    hwloc_cpuset_t cur_bind = NULL;

    if (!hwloc || !out_result) return QUO_ERR_INVLD_ARG;
    if (QUO_SUCCESS != (rc = get_obj_by_type(hwloc, type, type_index, &obj))) {
        return rc;
    }
    if (QUO_SUCCESS != (rc = get_cur_bind(hwloc, pid, &cur_bind))) return rc;
    *out_result = hwloc_bitmap_intersects(cur_bind, obj->cpuset);
    hwloc_bitmap_free(cur_bind);
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_hwloc_bound(const quo_hwloc_t *hwloc,
                pid_t pid,
                bool *out_bound)
{
    int rc = 0;
    hwloc_cpuset_t cur_bind = NULL;

    if (NULL == hwloc || NULL == out_bound) return QUO_ERR_INVLD_ARG;

    if (QUO_SUCCESS != (rc = get_cur_bind(hwloc, pid, &cur_bind))) {
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
                          pid_t pid,
                          char **out_str)
{
    int rc = QUO_SUCCESS;
    hwloc_cpuset_t cur_bind = NULL;

    if (!hwloc || !out_str) return QUO_ERR_INVLD_ARG;

    if (QUO_SUCCESS != (rc = get_cur_bind(hwloc, pid, &cur_bind))) {
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
static int
rebind(const quo_hwloc_t *hwloc,
       quo_bind_push_policy_t policy,
       quo_obj_type_t type,
       unsigned obj_index)
{
    int rc = QUO_SUCCESS;
    hwloc_obj_t target_obj = NULL;
    hwloc_cpuset_t cpuset = NULL;

    if (!hwloc) return QUO_ERR_INVLD_ARG;
    /* now get the appropriate object based on the given policy */
    if (QUO_BIND_PUSH_PROVIDED == policy) {
        rc = get_obj_by_type(hwloc, type, obj_index, &target_obj);
    }
    else if (QUO_BIND_PUSH_OBJ) {
        /* get_obj_covering_cur_bind ignores obj_index */
        rc = get_obj_covering_cur_bind(hwloc, type, &target_obj);
    }
    else {
        rc = QUO_ERR_INVLD_ARG;
    }
    if (QUO_SUCCESS != rc) goto out;
    /* now allocate and copy the given obj's cpuset */
    if (NULL == (cpuset = hwloc_bitmap_alloc())) return QUO_ERR_OOR;
    /* make a copy of the obj's cpuset */
    hwloc_bitmap_copy(cpuset, target_obj->cpuset);
    /* set the policy */
    if (-1 == hwloc_set_cpubind(hwloc->topo, cpuset, HWLOC_CPUBIND_PROCESS)) {
        rc = QUO_ERR_NOT_SUPPORTED;
        goto out;
    }
out:
    if (cpuset) hwloc_bitmap_free(cpuset);
    return rc;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_hwloc_bind_push(quo_hwloc_t *hwloc,
                    quo_bind_push_policy_t policy,
                    quo_obj_type_t type,
                    unsigned obj_index)
{
    int rc = QUO_SUCCESS;

    if (!hwloc) return QUO_ERR_INVLD_ARG;
    /* make sure that we are dealing with a valid policy */
    if (!valid_bind_policy(policy)) {
        QUO_ERR_MSG("invalid policy");
        return QUO_ERR_INVLD_ARG;
    }
    /* change binding */
    if (QUO_SUCCESS != (rc = rebind(hwloc, policy, type, obj_index))) {
        return rc;
    }
    /* stash our shiny new binding */
    return push_cur_bind(hwloc);
}

/* ////////////////////////////////////////////////////////////////////////// */
/* TODO return popped val? */
int
quo_hwloc_bind_pop(quo_hwloc_t *hwloc)
{
    int rc = QUO_SUCCESS;
    hwloc_cpuset_t topbind = NULL;

    if (!hwloc) return QUO_ERR_INVLD_ARG;
    if (QUO_SUCCESS != (rc = bind_stack_pop(hwloc, NULL))) return rc;
    /* revert to the top binding after pop (the previous binding) */
    if (QUO_SUCCESS != (rc = bind_stack_top(hwloc, &topbind))) goto out;
    if (-1 == hwloc_set_cpubind(hwloc->topo, topbind,
                                HWLOC_CPUBIND_PROCESS)) {
        rc = QUO_ERR_NOT_SUPPORTED;
        goto out;
    }
out:
    if (topbind) hwloc_bitmap_free(topbind);
    return rc;
}
