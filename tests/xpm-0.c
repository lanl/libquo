/*
 * Copyright (c) 2017      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

#include "quo.h"
#include "quo-xpm.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "mpi.h"

int
main(int argc, char **argv)
{
    QUO_context q = NULL;
    QUO_xpm_context xpm = NULL;

    int numpe = 0, rank = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numpe);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int qid = 0, nqid = 0;
    QUO_create(&q, MPI_COMM_WORLD);
    QUO_id(q, &qid);
    QUO_nqids(q, &nqid);

    int ni = 10;
    size_t local_size = ni * sizeof(int);

    QUO_xpm_allocate(q, local_size, &xpm);

    QUO_xpm_view_t my_view;

    QUO_xpm_view_local(xpm, &my_view);

    int *my_array = (int *)my_view.base;

    for (int i = 0; i < ni; ++i) {
        my_array[i] = rank;
    }

    QUO_barrier(q);
    for (int i = 0; i < ni; ++i) {
        printf("%d: my_array[i] = %d\n", qid, my_array[i]);
    }
    QUO_barrier(q);

    if (0 == qid) {
        for (int i = 0; i < ni * numpe; ++i) {
            my_array[i] = rank;
        }
    }
    QUO_barrier(q);

    for (int i = 0; i < ni; ++i) {
        printf("%d: new my_array[i] = %d\n", qid, my_array[i]);
    }

    QUO_xpm_free(xpm);

    QUO_free(q);
    MPI_Finalize();

    return EXIT_SUCCESS;
}
