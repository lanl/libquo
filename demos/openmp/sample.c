#define _GNU_SOURCE

#include "mpi.h"
#include "quo.h"
#include <omp.h>
 <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>

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
