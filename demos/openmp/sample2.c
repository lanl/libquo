#define _GNU_SOURCE

#include "mpi.h"
#include "quo.h"
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>

QUO_context context;
int rank, size;

/* void pin_threads() { */
/*   int thread_num; */
/*   cpu_set_t set; */
  

/*   get_cur_bind((context->hwloc, getpid(), &set); */
  
/*   print("count: %d\n", CPU_COUNT(&set)); */

/*   return; */
  
/*   CPU_ZERO(&set); */
  
/*   thread_num = omp_get_thread_num(); */
/*   CPU_SET(thread_num, &set); */
/*   sched_setaffinity(syscall(SYS_gettid), sizeof(cpu_set_t), &set);   */
/* } */

void toString(char *text){
  printf("%s: Rank %d, thread: %d, PID: %d, CPU: %u\n", text, rank, omp_get_thread_num(), getpid(), sched_getcpu());
  fflush(stdout);
}

void main() {
  int number, i;
  
  MPI_Init(NULL, NULL);
  
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  QUO_create(&context);

  QUO_bind_push(context, QUO_BIND_PUSH_PROVIDED, QUO_OBJ_SOCKET, 0);
  omp_set_nested(1);
    /* if (rank > 0)  */
    /*   MPI_Recv(&number, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, NULL); */
    
#pragma omp parallel num_threads(4)
    {  
      QUO_bind_threads(context, QUO_OBJ_SOCKET, 0);
      /* toString("1 configuration"); */
      /* sleep(1); */
      /* printf("\n"); */
      /* sleep(1); */
      
/* #pragma omp parallel num_threads(2) */
/*       { */
/* 	QUO_bind_threads(context, QUO_OBJ_SOCKET, rank%2); */
/* 	/\* toString("2 configuration"); *\/ */
/*       } */
    }
    
    /* if(rank < 4) */
    /*   MPI_Send(&number, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD); */

    QUO_free(context);

    MPI_Finalize();
}
