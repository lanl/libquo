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
#include <assert.h>

#include "mpi.h"

int
main(int argc, char **argv)
{
    QUO_context q = NULL;

    assert(MPI_SUCCESS == MPI_Init(&argc, &argv));
    assert(QUO_SUCCESS == QUO_create(&q));
    assert(QUO_SUCCESS == QUO_finalize(q));
    assert(QUO_SUCCESS == QUO_destruct(q));
    assert(MPI_SUCCESS == MPI_Finalize());
    return EXIT_SUCCESS;
}
