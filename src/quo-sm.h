/*
 * Copyright (c) 2013-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

/**
 * @file quo-sm.h
 */

#ifndef QUO_SM_H_INCLUDED
#define QUO_SM_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

struct quo_sm_t;
typedef struct quo_sm_t quo_sm_t;

int
quo_sm_construct(quo_sm_t **newsm);

int
quo_sm_destruct(quo_sm_t *sm);

int
quo_sm_segment_create(quo_sm_t *qsm,
                      const char *seg_path,
                      size_t seg_size);

int
quo_sm_segment_attach(quo_sm_t *qsm,
                      const char *seg_path,
                      size_t seg_size);

int
quo_sm_unlink(quo_sm_t *qsm);

void *
quo_sm_get_basep(quo_sm_t *qsm);

#endif
