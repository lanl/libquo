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

int
main(int argc, char **argv)
{
    int erc = EXIT_SUCCESS;
    char *str = NULL;
    hwloc_cpuset_t cpu_set = hwloc_bitmap_alloc();
    hwloc_topology_t topology;

    /* allocate and initialize topology object. */
    hwloc_topology_init(&topology);
    /* build the topology */
    hwloc_topology_load(topology);

    printf("starting reaffinitization test on pid %d\n", (int)getpid());

    hwloc_get_cpubind(topology, cpu_set, HWLOC_CPUBIND_PROCESS);

    hwloc_bitmap_asprintf(&str, cpu_set);

    printf("TEST: %s\n", str);

    printf("done with reaffinitization test\n");


    hwloc_bitmap_free(cpu_set);
    free(str);

    return erc;
}
