/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quo.h"

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

#include "quodemo-p1.h"

int
p1_entry_point(context_t *c)
{
    fprintf(stdout, "ooo [rank %d] in %s\n", c->rank, __func__);
    fflush(stdout);
    /* for pretty print */
    usleep((c->rank) * 1000);
    return 0;
}
