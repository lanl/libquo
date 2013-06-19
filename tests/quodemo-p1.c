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

typedef struct p1context_t {
    /* communicator used by p1 */
    MPI_Comm comm;
    /* size of p1_comm */
    int comm_size;
    /* my rank in p1_comm */
    int rank;
} p1context_t;

static inline void
p1_emit_sync(const context_t *c,
             const p1context_t *p1c)
{
    MPI_Barrier(p1c->comm);
    usleep((p1c->rank) * 1000);
}

int
p1_init(context_t *c,
        int np1s /* number of participants |p1who| */,
        int *p1who /* the participating ranks (MPI_COMM_WORLD) */)
{

}

int
p1_entry_point(context_t *c)
{
    fprintf(stdout, "ooo [rank %d] in %s\n", c->rank, __func__);
    fflush(stdout);
    /* for pretty print */
    usleep((c->rank) * 1000);
    return 0;
}
