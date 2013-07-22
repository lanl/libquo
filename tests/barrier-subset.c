/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif

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
    }
    assert(MPI_SUCCESS == MPI_Finalize());
    return EXIT_SUCCESS;
}
