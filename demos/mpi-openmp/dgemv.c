/*
 * Copyright (c) 2017      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "mpi.h"
#include "omp.h"
#include "quo.h"

typedef struct dgemv_t {
    /* Number of MPI processes in MPI_COMM_WORLD. */
    int numpe;
    /* My rank in MPI_COMM_WORLD. */
    int pe;
    /* A QUO contex. */
    QUO_context qc;
} dgemv_t;

enum {
    SUCCESS = 0,
    FAILURE
};

#define pprintf(p, va...)                                                      \
do {                                                                           \
    if ((p)) {                                                                 \
        printf(va);                                                            \
        fflush(stdout);                                                        \
    }                                                                          \
} while(0)

static int
init_mpi(dgemv_t *d,
         int argc,
         char **argv)
{
    if (MPI_SUCCESS != MPI_Init(&argc, &argv)) goto err;

    if (MPI_SUCCESS != MPI_Comm_size(MPI_COMM_WORLD, &(d->numpe))) goto err;
    if (MPI_SUCCESS != MPI_Comm_rank(MPI_COMM_WORLD, &(d->pe)))    goto err;

    return SUCCESS;
err:
    return FAILURE;
}

static int
fini_mpi(void)
{
    if (MPI_SUCCESS != MPI_Finalize()) return FAILURE;

    return SUCCESS;
}

static int
create_quo_context(dgemv_t *d)
{
    if (QUO_SUCCESS != QUO_create(&(d->qc), MPI_COMM_WORLD)) return FAILURE;

    return SUCCESS;
}

static int
free_quo_context(dgemv_t *d)
{
    if (QUO_SUCCESS != QUO_free(d->qc)) return FAILURE;

    return SUCCESS;
}

int
main(int argc, char **argv)
{
    int rc = SUCCESS;
    dgemv_t dgem;

    /* Init MPI library. */
    if (SUCCESS != (rc = init_mpi(&dgem, argc, argv))) goto out;
    /* Now we can create a QUO context. */
    if (SUCCESS != (rc = create_quo_context(&dgem))) goto out;

    if (SUCCESS != (rc = free_quo_context(&dgem))) goto out;
    if (SUCCESS != (rc = fini_mpi())) goto out;

out:
    return (SUCCESS == rc) ? EXIT_SUCCESS : EXIT_FAILURE;
}
