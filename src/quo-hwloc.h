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

typedef hwloc_const_cpuset_t quo_const_nodeset_t;

int
quo_hwloc_construct(quo_hwloc_t **nhwloc);

int
quo_hwloc_destruct(quo_hwloc_t *nhwloc);

int
quo_hwloc_node_topo_stringify(const quo_hwloc_t *hwloc,
                              char **out_str);

int
quo_hwloc_get_nobjs_by_type(const quo_hwloc_t *hwloc,
                            quo_obj_type_t target_type,
                            int *out_nobjs);

int
quo_hwloc_get_nobjs_in_type_by_type(const quo_hwloc_t *hwloc,
                                    quo_obj_type_t in_type,
                                    unsigned in_type_index,
                                    quo_obj_type_t type,
                                    int *out_result);

int
quo_hwloc_is_in_cpuset_by_type_id(const quo_hwloc_t *hwloc,
                                  quo_obj_type_t type,
                                  unsigned type_index,
                                  int *out_result);

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

int
quo_hwloc_sbitmask(const quo_hwloc_t *hwloc,
                   char **out_str);

int
quo_hwloc_stringify_cbind(const quo_hwloc_t *hwloc,
                          char **out_str);

int
quo_hwloc_rebind(const quo_hwloc_t *hwloc,
                 quo_obj_type_t type,
                 unsigned obj_index);

int
quo_hwloc_bind_push(quo_hwloc_t *hwloc,
                    quo_obj_type_t type,
                    unsigned obj_index);

int
quo_hwloc_bind_pop(quo_hwloc_t *hwloc);

#endif
