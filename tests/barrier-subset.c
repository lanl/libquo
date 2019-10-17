/*
 * Copyright (c) 2013-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>

#include "mpi.h"

int
main(int argc, char **argv)
{
    int rank = 0, nranks = 0;
    assert(MPI_SUCCESS == MPI_Init(&argc, &argv));
    assert(MPI_SUCCESS == MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    assert(MPI_SUCCESS == MPI_Comm_size(MPI_COMM_WORLD, &nranks));
    if (0 == rank % 2) {
        assert(MPI_SUCCESS == MPI_Barrier(MPI_COMM_WORLD));
    }
    else {
        sleep(100);
        assert(MPI_SUCCESS == MPI_Barrier(MPI_COMM_WORLD));
    }
    assert(MPI_SUCCESS == MPI_Finalize());
    return EXIT_SUCCESS;
}
