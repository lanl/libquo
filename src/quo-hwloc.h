/*
 * Copyright (c) 2013-2024 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

/**
 * @file quo-hwloc.h
 */

#ifndef QUO_HWLOC_H_INCLUDED
#define QUO_HWLOC_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quo.h"
#include "quo-private.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif

#include "hwloc/include/hwloc.h"

int
quo_hwloc_construct(quo_hwloc_t **nhwloc);

int
quo_hwloc_init(quo_hwloc_t *hwloc,
               quo_mpi_t *mpi,
               QUO_create_flags_t flags);

int
quo_hwloc_destruct(quo_hwloc_t *nhwloc);

int
quo_hwloc_get_nobjs_by_type(const quo_hwloc_t *hwloc,
                            QUO_obj_type_t target_type,
                            int *out_nobjs);

int
quo_hwloc_get_nobjs_in_type_by_type(const quo_hwloc_t *hwloc,
                                    QUO_obj_type_t in_type,
                                    unsigned in_type_index,
                                    QUO_obj_type_t type,
                                    int *out_result);

int
quo_hwloc_is_in_cpuset_by_type_id(const quo_hwloc_t *hwloc,
                                  QUO_obj_type_t type,
                                  pid_t pid,
                                  unsigned type_index,
                                  int *out_result);

int
quo_hwloc_bound(const quo_hwloc_t *hwloc,
                pid_t pid,
                bool *out_bound);

int
quo_hwloc_stringify_cbind(const quo_hwloc_t *hwloc,
                          pid_t pid,
                          char **out_str);

int
quo_hwloc_rebind(const quo_hwloc_t *hwloc,
                 QUO_obj_type_t type,
                 unsigned obj_index);

int
quo_hwloc_bind_push(quo_hwloc_t *hwloc,
                    QUO_bind_push_policy_t policy,
                    QUO_obj_type_t type,
                    unsigned obj_index);

int
quo_hwloc_bind_pop(quo_hwloc_t *hwloc);

#endif
