/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quo-mpi.h"

#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <errno.h>

#include "mpi.h"

/* ////////////////////////////////////////////////////////////////////////// */
/* quo_mpi_t type definition */
struct quo_mpi_t {
    int rank;
    int nranks;
    int nsmpranks;
};

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_mpi_construct(quo_mpi_t **nmpi)
{
    int initialized = 0;

    if (!nmpi) return QUO_ERR_INVLD_ARG;

    if (MPI_SUCCESS != MPI_Initialized(&initialized)) return QUO_ERR_MPI;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_mpi_destruct(quo_mpi_t *nmpi)
{
    return QUO_ERR_NOT_SUPPORTED;
}
