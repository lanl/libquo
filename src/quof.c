/*
 * Copyright (c) 2013-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

/**
 * @file quof.c Fortran interface helpers.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quo.h"
#include "quo-private.h"
#include "quo-mpi.h"

/**
 * Simply a wrapper for our Fortran interface to C interface. No need to expose
 * in quo.h header at this point, since it is only used by our Fortran module.
 */
int
QUO_create_f2c(QUO_t **q,
               MPI_Fint comm)
{
    MPI_Comm c_comm = MPI_Comm_f2c(comm);
    //
    return QUO_create(q, c_comm);
}

/**
 * Simply a wrapper for our Fortran interface to C interface. No need to expose
 * in quo.h header at this point, since it is only used by our Fortran module.
 */
int
QUO_get_mpi_comm_by_type_f2c(QUO_t *q,
                             QUO_obj_type_t target_type,
                             MPI_Fint *out_comm)
{
    MPI_Comm c_comm;
    int rc = QUO_get_mpi_comm_by_type(q, target_type, &c_comm);
    *out_comm = MPI_Comm_c2f(c_comm);

    return rc;
}

/**
 * Used to free up allocated memory on the Fortran side - don't include in
 * quo.h.
 *
 * @param[in] p Pointer to allocated memory.
 * */
int
QUO_ptr_free(void *p)
{
    if (p) free(p);
    return QUO_SUCCESS;
}
