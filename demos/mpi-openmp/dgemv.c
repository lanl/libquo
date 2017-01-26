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
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

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
    /* Local number of rows. */
    int64_t m;
    /* Local number of columns. */
    int64_t n;
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

static bool
starts_with(char *s,
            char *prefix)
{
    size_t n = strlen( prefix );
    if (strncmp(s, prefix, n)) return false;
    return true;
}

static int
init_dgem(dgemv_t *d,
          int argc,
          char **argv)
{
    static const int n_params = 2;
    static const int64_t defaultv = 4096;
    int64_t iparams[] = {defaultv, defaultv};
    /* Local sizes. */
    static char *params[16] = {
        "--m=",
        "--n="
    };

    /* Parse argv. */
    for (int i = 1; i < argc; ++i) {
        for (int pi = 0; pi < n_params; ++pi) {
            if (starts_with(argv[i], params[pi])) {
                if (sscanf(argv[i] + strlen(params[pi]),
                           "%" PRId64, iparams + pi) != 1 || iparams[pi] < 1) {
                    iparams[pi] = defaultv;
                }
            }
        }
    }

    d->m = iparams[0];
    d->n = iparams[1];

    pprintf(0 == d->pe,
            "# Local Matrix:\n"
            "# m=%" PRId64 "\n"
            "# n=%" PRId64 "\n"
            "# Global Matrix:\n"
            "# m=%" PRId64 "\n"
            "# n=%" PRId64 "\n",
            d->m, d->n, d->m * d->numpe, d->n);

    return SUCCESS;
}

int
main(int argc, char **argv)
{
    int rc = SUCCESS;
    dgemv_t dgem;

    /* Init MPI library. */
    if (SUCCESS != (rc = init_mpi(&dgem, argc, argv))) goto out;
    /* Problem init. */
    if (SUCCESS != (rc = init_dgem(&dgem, argc, argv))) goto out;
    /* Create a QUO context (can be done anytime after MPI_Init). */
    if (SUCCESS != (rc = create_quo_context(&dgem))) goto out;

    if (SUCCESS != (rc = free_quo_context(&dgem))) goto out;
    if (SUCCESS != (rc = fini_mpi())) goto out;

out:
    return (SUCCESS == rc) ? EXIT_SUCCESS : EXIT_FAILURE;
}
