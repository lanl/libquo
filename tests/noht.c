/*
 * Copyright (c) 2013-2024 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

#include "quo.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "mpi.h"

int
main(int argc, char **argv)
{
    QUO_context q = NULL;
    int nproc = 0, proc_rank = 0;

    assert(MPI_SUCCESS == MPI_Init(&argc, &argv));
    setbuf(stdout, NULL);

    assert(MPI_SUCCESS == MPI_Comm_size(MPI_COMM_WORLD, &nproc));
    assert(MPI_SUCCESS == MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank));

    assert(QUO_SUCCESS == QUO_create_with_flags(&q, MPI_COMM_WORLD, QUO_CREATE_NO_MT));

    assert(QUO_SUCCESS == QUO_free(q));
    assert(MPI_SUCCESS == MPI_Finalize());

    return EXIT_SUCCESS;
}
