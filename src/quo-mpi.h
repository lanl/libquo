/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#ifndef QUO_MPI_H_INCLUDED
#define QUO_MPI_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quo-private.h"
#include "quo.h"

struct quo_mpi_t;
typedef struct quo_mpi_t quo_mpi_t;

int
quo_mpi_construct(quo_mpi_t **nmpi);

int
quo_mpi_destruct(quo_mpi_t *nmpi);

#endif
