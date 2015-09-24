#include "mpi.h"
#include "quo.h"
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>

QUO_context context;
int rank, size;

void toString(char *text){
  printf("%s: Rank %d, CPU: %u\n", text, rank, sched_getcpu());
  fflush(stdout);
}

void main() {
  int number;
  
  MPI_Init(NULL, NULL);
  
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  QUO_create(&context);

  if(rank < 4){
    QUO_bind_push(context, QUO_BIND_PUSH_PROVIDED, QUO_OBJ_SOCKET, rank%2);
    if (rank > 0) 
      MPI_Recv(&number, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, NULL);
    omp_set_num_threads(6);
#pragma omp parallel proc_bind(close) 
    {
      toString("First configuration");
    }
    if(rank < 3)
      MPI_Send(&number, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
    QUO_barrier(context);
  }
  else
    QUO_barrier(context);
  
  if(rank==0) {
    printf("\n");
    fflush(stdout);
  }
  MPI_Barrier(MPI_COMM_WORLD);

  /*Set up second configuration: 1 rank per socket, 10 Threads per
    rank*/
  if(rank > 1)  
    QUO_barrier(context);
  else{ 
    QUO_bind_push(context, QUO_BIND_PUSH_PROVIDED, QUO_OBJ_SOCKET, rank);
    if (rank==1) 
      MPI_Recv(&number, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, NULL);
#pragma omp parallel num_threads(10) proc_bind(close)
    {
      toString("Second configuration");
    }
    if (rank==0)
      MPI_Send(&number, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
     
    QUO_bind_pop(context);  
    QUO_barrier(context);
  }
  
  if(rank==0) {
    printf("\n");
    fflush(stdout);
  }
  MPI_Barrier(MPI_COMM_WORLD);

  /*Return to first configuration: 2 ranks per socket, 5 Threads per
    rank*/
  if(rank < 4){
    if (rank > 0) 
      MPI_Recv(&number, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, NULL);
#pragma omp parallel num_threads(5) proc_bind(close) 
    {
      toString("First Configuration(2)");
    }
    if(rank < 3)
      MPI_Send(&number, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
    QUO_barrier(context);
  }
  else
    QUO_barrier(context);
  
  if(rank==0) {
    printf("\n");
    fflush(stdout);
  }
  MPI_Barrier(MPI_COMM_WORLD);

  /*Set up third configuration: 5 ranks per socket, 2 Threads per
    rank*/
  if(rank < 10) {
    QUO_bind_push(context, QUO_BIND_PUSH_PROVIDED, QUO_OBJ_SOCKET, rank%2);
    if (rank > 0) 
      MPI_Recv(&number, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, NULL);
#pragma omp parallel num_threads(2) proc_bind(close) 
    {
      toString("Third Configuration");
    }
    if(rank < 9)
      MPI_Send(&number, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
    QUO_bind_pop(context);
    QUO_barrier(context);
  }
  else
    QUO_barrier(context);

  if(rank==0) {
    printf("\n");
    fflush(stdout);
  }
  MPI_Barrier(MPI_COMM_WORLD);

  /*Return to first configuration: 2 ranks per socket, 5 Threads per
    rank*/   
  if(rank < 4){
    if (rank > 0) 
      MPI_Recv(&number, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, NULL);
#pragma omp parallel num_threads(5) proc_bind(close) 
    {
      toString("First Configuration(3)");
    }
    if(rank < 3)
      MPI_Send(&number, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
    QUO_barrier(context);
  }
  else
    QUO_barrier(context);

  if(rank==0) {
    printf("\n");
    fflush(stdout);
  }
  MPI_Barrier(MPI_COMM_WORLD);

 /*Set up second configuration: 1 rank per socket, 10 Threads per
    rank*/
  if(rank > 1)  
    QUO_barrier(context);
  else{ 
    QUO_bind_push(context, QUO_BIND_PUSH_PROVIDED, QUO_OBJ_SOCKET, rank);
    if (rank==1) 
      MPI_Recv(&number, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, NULL);
#pragma omp parallel num_threads(10) proc_bind(close)
    {
      toString("Second Configuration(2)");
    }
    if (rank==0)
      MPI_Send(&number, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
     
    QUO_bind_pop(context);  
    QUO_barrier(context);
  }
  
  if(rank==0) {
    printf("\n");
    fflush(stdout);
  }
  MPI_Barrier(MPI_COMM_WORLD);

  /*Return to first configuration: 2 ranks per socket, 5 Threads per
    rank*/     
  if(rank < 4){
    if (rank > 0) 
      MPI_Recv(&number, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, NULL);
#pragma omp parallel num_threads(5) proc_bind(close) 
    {
      printf("First Configuration(4): Rank %d, CPU: %u\n", rank, sched_getcpu());
      fflush(stdout);
    }
    if(rank < 3)
      MPI_Send(&number, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
    QUO_barrier(context);
  }
  else
    QUO_barrier(context);
  
  if(rank==0) {
    printf("\n");
    fflush(stdout);
  }
  MPI_Barrier(MPI_COMM_WORLD);

  /*Set up third configuration: 5 ranks per socket, 2 Threads per
    rank*/
  if(rank < 10) {
    QUO_bind_push(context, QUO_BIND_PUSH_PROVIDED, QUO_OBJ_SOCKET, rank%2);
    if (rank > 0) 
      MPI_Recv(&number, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, NULL);
#pragma omp parallel num_threads(2) proc_bind(close) 
    {
      toString("Third Configuration(2)");
    }
    if(rank < 9)
      MPI_Send(&number, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
    QUO_bind_pop(context);
    QUO_barrier(context);
  }
  else
    QUO_barrier(context);
  
  QUO_free(context);

  MPI_Finalize();
}
