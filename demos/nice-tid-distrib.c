/**
 * Copyright (c) 2018      Los Alamos National Security, LLC
 *                         All rights reserved.
 */

#include "quo.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "mpi.h"

// An example of how one might get a 'nice distribution' of stids per node when
// faced with a potentially uneven number of selected tids per some given
// resource.

typedef struct inf_t {
    // My ID in MPI_COMM_WORLD.
    int rank;
    // Number of processors in MPI_COMM_WORLD.
    int nranks;
    // Yup, a QUO context.
    QUO_context quo;
    // Machine-local ID assigned by QUO.
    int qid;
    // Number of MPI processes that share a node with me (includes me).
    int nqid;
    // Number of machines participating in initializing communicator.
    int n_machines;
    // My machine ID (globally unique across machines).
    int machine_id;
    // Comm used to communicate between processes on a single machine.
    MPI_Comm machine_comm;
    // Comm used for communication between QID zeros across machines.
    MPI_Comm machine_delegate_comm;
    // Flag indicating whether or not I've been selected ---I'm an stid.
    int selected;
    // List of total number of stids per machine (not resource) that allows us
    // to understand how many stids are 'active' per node.  Valid only at
    // MPI_COMM_WORLD rank 0.
    int *nstids;
} inf_t;

typedef struct tuple_t {
    int first;
    int second;
} tuple_t;

static void
machine_comm_setup(inf_t *inf)
{
    // Get a communicator that has all the processes that share a machine.
    QUO_get_mpi_comm_by_type(inf->quo, QUO_OBJ_MACHINE, &inf->machine_comm);
}

static void
delegate_comm_setup(inf_t *inf)
{
    // Split MPI_COMM_WORLD into two groups:
    //     Group 1: contains all QID zeros in the job.
    //     Group 2) contains everyone else.
    MPI_Comm_split(
        MPI_COMM_WORLD,
        inf->qid == 0 ? 0 : 1 /* Only two groups. */,
        inf->rank,
        &inf->machine_delegate_comm
    );
    // Because we want it to be erroneous for a non-zero QID to communicate over
    // the machine_delegate_comm, invalidate for those that aren't part of Group
    // 1 (above), so errors are easy to catch---it is an error to communicate
    // over MPI_COMM_NULL.
    if (inf->qid != 0) {
        MPI_Comm_free(&inf->machine_delegate_comm);
        inf->machine_delegate_comm = MPI_COMM_NULL;
    }
}

static void
node_id_setup(inf_t *inf)
{
    // Name the machines with IDs 0 to N-1, where N is the total number of
    // machines in the job (because MPI_COMM_WORLD was used in QUO_create).
    if (inf->machine_delegate_comm != MPI_COMM_NULL) {
        // The reason this works is because each node has exactly one process in
        // the machine_delegate_comm communicator---and they happen to be nice
        // IDs: 0 to N-1.
        MPI_Comm_rank(inf->machine_delegate_comm, &inf->machine_id);
        // Not strictly needed---a sanity check.
        int sanity = -1;
        MPI_Comm_size(inf->machine_delegate_comm, &sanity);
        assert(sanity == inf->n_machines);
    }
    // Share the name with everyone on a node.
    MPI_Bcast(&inf->machine_id, 1, MPI_INT, 0, inf->machine_comm);
}

static void
auto_distrib(
    inf_t *inf,
    QUO_obj_type_t target_res,
    int max_stids_per_res
) {
    QUO_auto_distrib(
        inf->quo,
        target_res,
        max_stids_per_res,
        &inf->selected
    );
}

static void
gather_nstid(
    inf_t *inf
) {
    // Sum up number of stids on my machine. This is easy for C (0 or 1 are
    // returned), but may require more work for a Fortran code (logicals).
    int local_nstid = -1;
    MPI_Reduce(
        &inf->selected,
        &local_nstid /* Valid only at root. */,
        1,
        MPI_INT,
        MPI_SUM,
        0,
        inf->machine_comm
    );
    if (inf->rank == 0) {
        // Create list large enough to hold nstids totals across all machines.
        inf->nstids = calloc(inf->n_machines, sizeof(int));
    }
    else {
        inf->nstids = NULL;
    }
    // MPI_COMM_WORLD rank 0 will always be a node delegate. We also know that
    // the number of nodes (machines) reported by QUO will be equal to the size
    // of machine_delegate_comm.
    if (inf->machine_delegate_comm != MPI_COMM_NULL) {
        MPI_Gather(
            &local_nstid,
            1,
            MPI_INT,
            inf->nstids,
            1,
            MPI_INT,
            0,
            inf->machine_delegate_comm
        );
    }
    if (inf->rank == 0) {
        for (int i = 0; i < inf->n_machines; ++i) {
            printf(
                "# rank %d says that node %d has %d stid%s\n",
                inf->rank, i, inf->nstids[i],
                (0 == inf->nstids[i] || inf->nstids[i] > 1) ? "s." : "."
            );
        }
    }
    // Not needed. Only used for nice output.
    MPI_Barrier(MPI_COMM_WORLD);
}

static int
init(inf_t *inf)
{
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &(inf->nranks));
    MPI_Comm_rank(MPI_COMM_WORLD, &(inf->rank));
    // QUO stuff
    QUO_create(&inf->quo, MPI_COMM_WORLD);
    QUO_id(inf->quo, &inf->qid);
    QUO_nqids(inf->quo, &inf->nqid);
    QUO_nnodes(inf->quo, &inf->n_machines);
    //
    inf->machine_comm = MPI_COMM_NULL;
    inf->machine_delegate_comm = MPI_COMM_NULL;
    //
    return 0;
}

static void
fini(inf_t *inf)
{
    // QUO communicator cleanup.
    if (inf->machine_comm != MPI_COMM_NULL) {
        MPI_Comm_free(&inf->machine_comm);
    }
    if (inf->machine_delegate_comm != MPI_COMM_NULL) {
        MPI_Comm_free(&inf->machine_delegate_comm);
    }
    if (inf->nstids) free(inf->nstids);
    //
    QUO_free(inf->quo);
    //
    MPI_Finalize();
}

static int
sum(
    int *a,
    int na
) {
    int res = 0;
    for (int i = 0; i < na; ++i) res += a[i];
    return res;
}

static int
all_same(
    int *a,
    int na
) {
    int first = a[0];
    for (int i = 1; i < na; ++i) {
        if (first != a[i]) return 0;
    }
    return 1;
}

static void
optimize_distribution(
    inf_t *inf,
    int max_stids
) {
    if (inf->rank == 0) {
        printf(
            "# rank %d is optimizing for max_stids = %d\n",
            inf->rank, max_stids
        );
        //
        const int nstids_len = inf->n_machines;
        // Easy case.
        if (all_same(inf->nstids, nstids_len)) {
#if 0 // TODO
            printf(
                "#\teven distribution of stids: %d per machine...done!\n",
                inf->nstids[0]
            );
            goto done;
#endif
        }
        int total_stids = sum(inf->nstids, nstids_len);
        printf("#\ttotal_stids = %d.\n", total_stids);
        // Easy case.
        if (total_stids <= max_stids) {
            printf("#\ttotal_stids <= max_stids...done!\n");
            goto done;
        }
        // The harder case.
        // How many stids do we have to unselect?
        int nunsel = max_stids - total_stids;
    }
done:
    MPI_Barrier(MPI_COMM_WORLD);
}

int
main(void)
{
    inf_t info;
    //
    init(&info);
    //
    machine_comm_setup(&info);
    //
    delegate_comm_setup(&info);
    //
    node_id_setup(&info);
    // Modify here for testing different configurations.
    const QUO_obj_type_t target_res = QUO_OBJ_SOCKET;
    const int max_stids_per_res = 1; // Per node.
    const int max_stids = 1; // Total target across all machines.
    //
    auto_distrib(&info, target_res, max_stids_per_res);
    //
    gather_nstid(&info);
    //
    optimize_distribution(&info, max_stids);
    // Cleanup.
    fini(&info);
    // All done.
    return EXIT_SUCCESS;
}
