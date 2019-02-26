/*
 * Copyright (c) 2013-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

/**
 * @file quo-mpi.h
 */

#ifndef QUO_MPI_H_INCLUDED
#define QUO_MPI_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quo-private.h"
#include "quo.h"

#include "mpi.h"

int
quo_mpi_construct(quo_mpi_t **nmpi);

int
quo_mpi_init(quo_mpi_t *nmpi,
             MPI_Comm comm);

int
quo_mpi_destruct(quo_mpi_t *nmpi);

int
quo_mpi_nnodes(const quo_mpi_t *mpi,
               int *nnodes);

int
quo_mpi_nnoderanks(const quo_mpi_t *mpi,
                   int *nnoderanks);

int
quo_mpi_noderank(const quo_mpi_t *mpi,
                 int *noderank);

int
quo_mpi_smprank2pid(quo_mpi_t *mpi,
                    int smprank,
                    pid_t *out_pid);

int
quo_mpi_ranks_on_node(const quo_mpi_t *mpi,
                      int *out_nranks,
                      int **out_ranks);

int
quo_mpi_sm_barrier(const quo_mpi_t *mpi);

int
quo_mpi_xchange_uniq_path(quo_mpi_t *mpi,
                          const char *module_name,
                          char **result);

int
quo_mpi_get_node_comm(quo_mpi_t *mpi,
                      MPI_Comm *comm);

int
quo_mpi_bcast(void *buffer,
              int count,
              MPI_Datatype datatype,
              int root,
              MPI_Comm comm);

int
quo_mpi_allgather(const void *sendbuf,
                  int sendcount,
                  MPI_Datatype sendtype,
                  void *recvbuf,
                  int recvcount,
                  MPI_Datatype recvtype,
                  MPI_Comm comm);
int
quo_mpi_get_comm_by_type(const quo_mpi_t *mpi,
                         QUO_obj_type_t target_type,
                         MPI_Comm *out_comm);
#endif
