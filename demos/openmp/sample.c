/*
 * Copyright (c) 2013-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mpi.h"
#include "quo.h"

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_OMP_H
#include <omp.h>
#endif
#ifdef HAVE_SCHED_H
#include <sched.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYSCALL_H
#include <sys/syscall.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

// TODO FIXME - global state is bad. Don't do this.
QUO_context context;
int rank, size;

void toString(char *text){
    printf("%s: Rank %d, thread: %d, PID: %d, CPU: %u\n",
           text, rank, omp_get_thread_num(), getpid(), sched_getcpu());
    fflush(stdout);
}

int main() {
    int number, i;

    if (MPI_SUCCESS != MPI_Init(NULL, NULL)) return 1;

    if (MPI_SUCCESS != MPI_Comm_rank(MPI_COMM_WORLD, &rank)) goto err;

    if (MPI_SUCCESS !=  MPI_Comm_size(MPI_COMM_WORLD, &size)) goto err;

    if (QUO_SUCCESS != QUO_create(&context)) goto err;

    if (QUO_SUCCESS !=
        QUO_bind_push(context, QUO_BIND_PUSH_PROVIDED, QUO_OBJ_SOCKET, rank%2))
    {
        printf("QUO_bind failed\n");
        goto err;
    }

    omp_set_nested(1);

#pragma omp parallel num_threads(4)
    {
        if (QUO_SUCCESS != QUO_bind_threads(context, QUO_OBJ_SOCKET, rank%2))
            printf("QUO_bind_threads failed\n");

        toString("First configuration");

#pragma omp parallel num_threads(2)
        {
            if (QUO_SUCCESS != QUO_bind_threads(context,
                                                QUO_OBJ_SOCKET, rank%2))
                printf("QUO_bind_threads failed\n");

            toString("Second configuration");
        }
    }

err:
    if(context)
        QUO_free(context);

    MPI_Finalize();
    return 0;
}
