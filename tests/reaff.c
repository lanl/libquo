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
    hwloc_topology_t topology;
    hwloc_cpuset_t cpu_set = hwloc_bitmap_alloc();

    /* allocate and initialize topology object. */
    hwloc_topology_init(&topology);

    hwloc_get_cpubind(topology, cpu_set, 0);

    char *str = NULL;
    hwloc_bitmap_asprintf(&str, cpu_set);

    printf("TEST: %s\n", str);

    printf("starting reaffinitization test on pid %d\n", (int)getpid());

    return erc;
}
