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
    int comm_rank;
    /* flag indicating whether or not i'm in the p1 group */
    int incomm;
} p1context_t;

static p1context_t p1;

static inline void
p1_emit_sync(const p1context_t *p1c)
{
    MPI_Barrier(p1c->comm);
    usleep((p1c->comm_rank) * 1000);
}

int
p1_init(context_t *c,
        int np1s /* number of participants |p1who| */,
        int *p1who /* the participating ranks (MPI_COMM_WORLD) */)
{
    int rc = QUO_SUCCESS;
    if (0 == c->noderank) {
        printf("ooo [rank %d] %d p1pes initializing p1\n", c->rank, np1s);
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
    MPI_Group world_group;
    MPI_Group p1_group;
    /* figure out what MPI_COMM_WORLD ranks share a node with me */
    if (MPI_SUCCESS != MPI_Comm_group(MPI_COMM_WORLD, &world_group)) {
        rc = QUO_ERR_MPI;
        goto out;
    }
    if (MPI_SUCCESS != MPI_Group_incl(world_group, np1s,
                                      p1who, &p1_group)) {
        rc = QUO_ERR_MPI;
        goto out;
    }
    if (MPI_SUCCESS != MPI_Comm_create(MPI_COMM_WORLD,
                                       p1_group,
                                       &(p1.comm))) {
        rc = QUO_ERR_MPI;
        goto out;
    }
    /* am i in the new communicator? */
    /* XXX is this the correct way of determining this? */
    p1.incomm = (MPI_COMM_NULL != p1.comm) ? 1 : 0;
    if (p1.incomm) {
        if (MPI_SUCCESS != MPI_Comm_size(p1.comm, &p1.comm_size)) {
            rc = QUO_ERR_MPI;
            goto out;
        }
        if (MPI_SUCCESS != MPI_Comm_rank(p1.comm, &p1.comm_rank)) {
            rc = QUO_ERR_MPI;
            goto out;
        }
    }
    /* for pretty print */
    usleep((c->rank) * 1000);
out:
    if (MPI_SUCCESS != MPI_Group_free(&world_group)) return 1;
    return (QUO_SUCCESS == rc) ? 0 : 1;
}

int
p1_fini(void)
{
    if (p1.incomm) {
        if (MPI_SUCCESS != MPI_Comm_free(&p1.comm)) return 1;
    }
    return 0;
}

int
p1_entry_point(context_t *c)
{
    printf("ooo [rank %d] p1pe rank %d doing science in p1!\n",
           c->rank, p1.comm_rank);
    fflush(stdout);
    p1_emit_sync(&p1);
    return 0;
}
