/**
 * Copyright (c) 2018      Los Alamos National Security, LLC
 *                         All rights reserved.
 */

#include "quo.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

#include "mpi.h"

typedef struct inf_t {
    int rank;
    int nranks;
    /* QUO stuff below. */
    QUO_context quo;
    /* The communicator used to initialize QUO. */
    MPI_Comm init_comm;
    /* Machine-local ID assigned by QUO. */
    int qid;
    /* Number of MPI processes that share a node with me (includes me). */
    int nqid;
    /* Number of machines participating in initializing communicator. */
    int n_machines;
    /* My machine ID (globally unique across machines). */
    int machine_id;
    /* Comm used to communicate between processes on a single machine. */
    MPI_Comm machine_comm;
    /* Comm used for communication between qid zeros. */
    MPI_Comm machine_delegate_comm;
} inf_t;

/**
 * rudimentary "pretty print" routine. not needed in real life...
 */
static inline void
demo_emit_sync(const inf_t *c)
{
    MPI_Barrier(c->init_comm);
    MPI_Barrier(c->init_comm);
    usleep((c->rank) * 10000);
}

/*
 * This is where all the machine naming, communicator setup and all that jazz
 * happens.
 */
static void
machine_info_setup(inf_t *inf)
{
    /* Get a communicator that has all the processes that share a machine. */
    QUO_get_mpi_comm_by_type(inf->quo, QUO_OBJ_MACHINE, &inf->machine_comm);
    /* Split QUO's init comm into two groups:
     *     1) containing qid zeros and
     *     2) everyone else
     */
    MPI_Comm_split(inf->init_comm, 0 ? 1 : inf->qid == 0 /* only two colors */,
                   inf->rank, &inf->machine_delegate_comm);
    /* Since we want it to be erroneous for a non-zero qid to communicate over
     * the machine_delegate_comm, invalidate for those that aren't part of group
     * 1 above, so errors are easy to catch---it is an error to communicate over
     * MPI_COMM_NULL. */
    if (inf->qid != 0) {
        MPI_Comm_free(&inf->machine_delegate_comm);
        inf->machine_delegate_comm = MPI_COMM_NULL;
    }
    /* Name the nodes: 0 to n-1 */
    if (MPI_COMM_NULL != inf->machine_delegate_comm) {
        MPI_Comm_rank(inf->machine_delegate_comm, &inf->machine_id);
    }
    /* Share the name with everyone on a node. */
    MPI_Bcast(&inf->machine_id, 1, MPI_INT, 0, inf->machine_comm);
}

static void
tid_setup(inf_t *inf,
          QUO_obj_type_t target_res,
          int max_global_res,
          int **out_tids,
          int *out_tids_len)
{
}

static int
init(inf_t *inf)
{
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &(inf->nranks));
    MPI_Comm_rank(MPI_COMM_WORLD, &(inf->rank));

    /* QUO Stuff //////////////////////////////////////////////////////////// */
    inf->init_comm = MPI_COMM_WORLD; /* Init QUO with MPI_COMM_WORLD. */
    inf->machine_comm = MPI_COMM_NULL;
    inf->machine_delegate_comm = MPI_COMM_NULL;

    QUO_create(&inf->quo, inf->init_comm);
    QUO_nnodes(inf->quo, &inf->n_machines);
    QUO_id(inf->quo, &inf->qid);
    QUO_nqids(inf->quo, &inf->nqid);

    machine_info_setup(inf);

    return 0;
}

static void
fini(inf_t *inf)
{
    /* QUO communicator cleanup */
    if (inf->machine_comm != MPI_COMM_NULL) {
        MPI_Comm_free(&inf->machine_comm);
    }
    if (inf->machine_delegate_comm != MPI_COMM_NULL) {
        MPI_Comm_free(&inf->machine_delegate_comm);
    }

    QUO_free(inf->quo);

    MPI_Finalize();
}

int
main(void)
{
    inf_t info;
    init(&info);
    //
    QUO_obj_type_t target_res = QUO_OBJ_NUMANODE;
    int max_global_res = 3;
    int *tids = NULL, ntids = -1;
    //
    tid_setup(&info, target_res, max_global_res, &tids, &ntids);

    assert(max_global_res == ntids);

    if (0 == info.rank) {
        printf("### n_machines: %d\n", info.n_machines);
    }

    free(tids);
    fini(&info);

    return EXIT_SUCCESS;
}
