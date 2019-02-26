/*
 * Copyright (c) 2013-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>

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
main(void)
{
    int erc = EXIT_SUCCESS;
    unsigned ncores = 0;
    hwloc_cpuset_t cpu_set = hwloc_bitmap_alloc(),
                   first_bind = hwloc_bitmap_alloc();
    hwloc_topology_t topology;
    hwloc_obj_t last_core;

    /* allocate and initialize topology object. */
    hwloc_topology_init(&topology);
    /* build the topology */
    hwloc_topology_load(topology);
    /* get some info */
    ncores = hwloc_get_nbobjs_by_type(topology, HWLOC_OBJ_CORE);
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
