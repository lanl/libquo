/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#ifndef QUO_DEMO_P1_H
#define QUO_DEMO_P1_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quodemo-multilib-common.h"
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
