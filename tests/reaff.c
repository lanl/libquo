/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <hwloc.h>

/* a test to see if we can rebind a running process */

static void
emit_binding(const hwloc_topology_t *t)
{
    char *str = NULL;
    hwloc_cpuset_t cpu_set = hwloc_bitmap_alloc();

    hwloc_get_cpubind(*t, cpu_set, HWLOC_CPUBIND_PROCESS);
    hwloc_bitmap_asprintf(&str, cpu_set);
    printf("%d's cpubind bitmap is: %s\n", (int)getpid(), str);

    free(str);
    hwloc_bitmap_free(cpu_set);
}

int
main(int argc, char **argv)
{
    int erc = EXIT_SUCCESS;
    unsigned ncores = 0;
    hwloc_cpuset_t cpu_set = hwloc_bitmap_alloc(), first_bind = hwloc_bitmap_alloc();
    hwloc_topology_t topology;
    hwloc_obj_t first_core, last_core;

    /* allocate and initialize topology object. */
    hwloc_topology_init(&topology);
    /* build the topology */
    hwloc_topology_load(topology);
    /* get some info */
    ncores = hwloc_get_nbobjs_by_type(topology, HWLOC_OBJ_CORE);
    first_core = hwloc_get_obj_by_type(topology, HWLOC_OBJ_CORE, 0);
    last_core = hwloc_get_obj_by_type(topology, HWLOC_OBJ_CORE, ncores - 1);
    /* stash current binding */
    hwloc_get_cpubind(topology, first_bind, HWLOC_CPUBIND_PROCESS);
    /* start the madness */
    printf("starting rebinding test on pid %d\n", getpid());

    emit_binding(&topology);

    printf("changing binding...\n");

    hwloc_bitmap_copy(cpu_set, last_core->cpuset);
    hwloc_bitmap_singlify(cpu_set);
    hwloc_set_cpubind(topology, cpu_set, HWLOC_CPUBIND_PROCESS);

    emit_binding(&topology);

    printf("reverting binding...\n");

    hwloc_set_cpubind(topology, first_bind, HWLOC_CPUBIND_PROCESS);

    emit_binding(&topology);

    printf("done with rebinding test\n");

    /* cleanup */
    hwloc_bitmap_free(cpu_set);
    hwloc_bitmap_free(first_bind);
    hwloc_topology_destroy(topology);

    return erc;
}
