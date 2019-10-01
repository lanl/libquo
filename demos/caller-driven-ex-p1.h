/*
 * Copyright (c) 2013-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 *
 */

#ifndef CALLER_DRIVEN_EX_P1_H
#define CALLER_DRIVEN_EX_P1_H


#include "caller-driven-ex-common.h"
#include "quo.h"

int
p1_init(context_t *c,
        int np1s,
        int *p1who);

int
p1_fini(void);

int
p1_entry_point(context_t *c);

#endif
