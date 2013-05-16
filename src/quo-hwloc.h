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

#include <hwloc.h>

struct quo_hwloc_t;
typedef struct quo_hwloc_t quo_hwloc_t;

int
quo_hwloc_construct(quo_hwloc_t **nhwloc);

int
quo_hwloc_destruct(quo_hwloc_t *nhwloc);

int
quo_hwloc_node_topo_stringify(const quo_hwloc_t *hwloc,
                              char **out_str);

int
quo_hwloc_sockets(const quo_hwloc_t *hwloc,
                  int *nsockets);

int
quo_hwloc_cores(const quo_hwloc_t *hwloc,
                int *ncores);

int
quo_hwloc_pus(const quo_hwloc_t *hwloc,
              int *npus);

int
quo_hwloc_bound(const quo_hwloc_t *hwloc,
                bool *out_bound);

#endif
