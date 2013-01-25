/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#ifndef QUO_HWLOC_H_INCLUDED
#define QUO_HWLOC_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quo-private.h"
#include "quo.h"

/* XXX add configury for hwloc later */
#include <hwloc.h>

typedef struct quo_hwloc_t {
    hwloc_topology_t *topo;
} quo_hwloc_t;

int
quo_hwloc_construct(quo_hwloc_t **nhwloc);

int
quo_hwloc_sockets(quo_hwloc_t *hwloc,
                  int *nsockets);


#endif
