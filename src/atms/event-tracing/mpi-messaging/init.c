/*
 * Copyright (c) 2017      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

/**
 * @file init.c
 */

#include <stdio.h>
#include "mpi.h"

int
MPI_Init(int *argc,
         char ***argv)
{
    int mpirc = PMPI_Init(argc, argv);
    printf("hi!\n");

    return mpirc;
}
