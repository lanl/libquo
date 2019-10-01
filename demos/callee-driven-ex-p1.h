/*
 * Copyright (c) 2013-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 *
 */

#ifndef CALLEE_DRIVEN_EX_P1_H_INCLUDED
#define CALLEE_DRIVEN_EX_P1_H_INCLUDED


#include "mpi.h"
#include "quo.h"

#include <stdbool.h>

typedef struct p1_context_t {
    /* my rank */
    int rank;
    /* number of ranks in initializing communicator */
    int nranks;
    /* number of nodes used across initializing communicator */
    int nnodes;
    /* number of ranks that share this node with me (includes myself) */
    int nnoderanks;
    /* my node rank */
    int noderank;
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
    /* flag indicating whether or not we are initially bound */
    int bound;
    /* dup of initializing communicator */
    MPI_Comm init_comm_dup;
    /* "quo communicator" containing active quo processes (workers ids). */
    MPI_Comm quo_comm;
    /* quo_comm rank */
    int qc_rank;
    /* quo_comm size */
    int qc_size;
    /* am i in quo_comm? */
    bool in_quo_comm;
    /* quo context handle */
    QUO_context quo;
} p1_context_t;


int
p1_init(p1_context_t **p1_ctx,
        MPI_Comm comm);

int
p1_entry_point(p1_context_t *c);

int
p1_fini(p1_context_t *p1_ctx);

#endif
