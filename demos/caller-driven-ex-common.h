/*
 * Copyright (c) 2013-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 *
 */

#ifndef CRD_EXC
#define CRD_EXC

#include "quo.h"
#include "mpi.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

/* shared context. note that the only think that really needs to be shared is
 * the quo context pointer -- *quo in this case. passing this around makes the
 * demo code simpler. */
typedef struct context_t {
    /* my rank */
    int rank;
    /* number of ranks in MPI_COMM_WORLD */
    int nranks;
    /* number of nodes in our job */
    int nnodes;
    /* number of ranks that share this node with me (includes myself) */
    int nnoderanks;
    /* my node rank */
    int noderank;
    /* whether or not mpi is initialized */
    bool mpi_inited;
    /* number of NUMA nodes on the node */
    int nnumanodes;
    /* number of sockets on the node */
    int nsockets;
    /* number of cores on the node */
    int ncores;
    /* number of pus (processing units - e.g hw threads) */
    int npus;
    /* quo major version */
    int qv;
    /* quo minor version */
    int qsv;
    /* pointer to initial stringification of our cpuset */
    char *cbindstr;
    /* flag indicating whether or not we are initially bound */
    int bound;
    /* a pointer to our quo context (the thing that gets passed around all over
     * the place). filler words that make this comment line look mo better... */
    QUO_context quo;
} context_t;


/* ////////////////////////////////////////////////////////////////////////// */
/* lazy coder section -- inlined here because i already have too many files   */
/* ////////////////////////////////////////////////////////////////////////// */
/**
 * rudimentary "pretty print" routine. not needed in real life...
 */
static inline void
demo_emit_sync(const context_t *c)
{
    MPI_Barrier(MPI_COMM_WORLD);
    usleep((c->rank) * 1000);
}

static inline int
emit_bind_state(const context_t *c,
                char *msg_prefix)
{
    char *cbindstr = NULL, *bad_func = NULL;
    int bound = 0;

    if (QUO_SUCCESS != QUO_stringify_cbind(c->quo, &cbindstr)) {
        bad_func = "QUO_stringify_cbind";
        goto out;
    }
    if (QUO_SUCCESS != QUO_bound(c->quo, &bound)) {
        bad_func = "QUO_bound";
        goto out;
    }
    printf("%s [rank %d] process %d [%s] bound: %s\n",
           msg_prefix, c->rank, (int)getpid(),
           cbindstr, bound ? "true" : "false");
    fflush(stdout);
out:
    if (cbindstr) free(cbindstr);
    if (bad_func) {
        fprintf(stderr, "%s: %s failure :-(\n", __func__, bad_func);
        return 1;
    }
    return 0;
}

#endif