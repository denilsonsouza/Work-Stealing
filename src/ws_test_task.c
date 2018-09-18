#include <stdio.h>
#include <stdlib.h>
#include <mpich/mpi.h>
#include <assert.h>
#include "../include/ws_task.h"
#include "../include/ws_dequeue.h"

typedef struct _worker
{
  int id;
 // task *tasks;
  struct _worker *workers;
  dequeue ws_task;
  long num_workers;
  int logged_worker;
} worker;

/** Global State **/
//static dequeue  localQueue;    // double ended queue of local only work
//verificar como terminar o WS
//static enum colors my_color;      // Ring-based termination detection

static int last_steal;    // Rank of last thread stolen from
static long chunks_recvd; // Total messages received
static long chunks_sent;  // Total messages sent
static long ctrl_recvd;   // Total messages received
static long ctrl_sent;    // Total messages sent

/** Global Parameters: Set in ss_init() **/
static int comm_size, comm_rank;
//static const int default_polling_interval = 32;

void ws_make_progress(dequeue *s)
{
 
  //size_t victim;
  printf("Retorno : %d \n",deq_isEmpty(s[comm_rank]));
  if (!deq_isEmpty(s[comm_rank]))
  {
     // randomico processador
    //victim = random() % data->num_workers;
    // sender mensagem
    // se tem retorno
    // faz work-stealing
    
    //senao
    //tenta N vezes.
 }
  else{ // processa

    while (1)
    {
      MPI_Status status;
      int flag, index;
      //void       *work;
      for (index = 0; index < 100; index++)
      {
        printf("Rank while: %d \n", comm_rank);
      }

      if (index >= 100)
      {
        printf("rank %d finishing\n", comm_rank);
        break;
      }
    }

  } //end-if

} //end-method

// Inicio da funcao principal
int main(int argc, char *argv[]){

  int p;             /* number of processes       */
  int source;        /* rank of sender            */
  int dest;          /* rank of receiver          */
  int tag = 0;       /* tag for messages          */
  MPI_Status status; /* return status for receive */
  int i;
  dequeue localQueue[p]; /* Lista para cada processador */
  worker workers[p]; /* Works for Work-stealing*/
  
  
/*   for(int i = 0; i < p; i++)
  {
    workers[i].id = i;
    workers[i].num_workers = i;
    workers[i].workers = workers;
  } */
  


  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  /* initializations for test */
   printf("Inicializando...");
  for (size_t i = 0; i < p; i++)
  {
    printf("%zd \n", i);
    localQueue[i] = deq_create();
  } 


  ws_make_progress(localQueue);
 
  MPI_Send(&comm_rank, 1, MPI_INT, p - 1 - comm_rank, tag, MPI_COMM_WORLD);
  MPI_Recv(&dest, 1, MPI_INT, p - 1 - comm_rank, tag, MPI_COMM_WORLD, &status);
  printf("Meu rank %d rank recebido %d\n", comm_rank, dest);

  MPI_Finalize();
} /* main */
