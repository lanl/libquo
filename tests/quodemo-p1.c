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
    if (0 == c->noderank) {
        printf("ooo [rank %d] %d ranks doing work...\n", c->rank, np1s);
        printf("ooo [rank %d] and they are: ", c->rank);
        fflush(stdout);
        for (int i = 0; i < np1s; ++i) {
            printf("%d ", p1who[i]); fflush(stdout);
            if (i + 1 == np1s) printf("\n"); fflush(stdout);
        }
    }
    /* ////////////////////////////////////////////////////////////////////// */
    /* now create our own communicator based on the rank ids passed here */
    /* ////////////////////////////////////////////////////////////////////// */
    return 0;
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
