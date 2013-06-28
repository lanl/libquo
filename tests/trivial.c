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
#include <assert.h>

#include "mpi.h"

int
main(int argc, char **argv)
{
    QUO_t *q = NULL;

    assert(MPI_SUCCESS == MPI_Init(&argc, &argv));
    assert(QUO_SUCCESS == QUO_construct(&q));
    assert(QUO_SUCCESS == QUO_init(q));
    assert(QUO_SUCCESS == QUO_finalize(q));
    assert(QUO_SUCCESS == QUO_destruct(q));
    assert(MPI_SUCCESS == MPI_Finalize());
    return EXIT_SUCCESS;
}
