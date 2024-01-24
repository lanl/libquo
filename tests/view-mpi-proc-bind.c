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
    int nproc = 0, proc_rank = 0, proc_name_len = 0;
    char proc_name[MPI_MAX_PROCESSOR_NAME + 1];
    char *binds = NULL;

    assert(MPI_SUCCESS == MPI_Init(&argc, &argv));
    setbuf(stdout, NULL);

    assert(MPI_SUCCESS == MPI_Comm_size(MPI_COMM_WORLD, &nproc));
    assert(MPI_SUCCESS == MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank));

    assert(MPI_SUCCESS == MPI_Get_processor_name(proc_name, &proc_name_len));

    assert(QUO_SUCCESS == QUO_create(&q, MPI_COMM_WORLD));
    assert(QUO_SUCCESS == QUO_stringify_cbind(q, &binds));

    fprintf(stdout, "Rank %08d of %d on host %s is bound to %s\n",
            proc_rank, nproc, proc_name, binds);

    assert(QUO_SUCCESS == QUO_free(q));
    assert(MPI_SUCCESS == MPI_Finalize());
    free(binds);
    return EXIT_SUCCESS;
}
