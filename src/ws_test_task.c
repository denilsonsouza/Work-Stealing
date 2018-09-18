#include <stdio.h>
#include <stdlib.h>
#include <mpich/mpi.h>
#include <assert.h>
#include "../include/ws_task.h"
#include "../include/ws_dequeue.h"



typedef struct _worker {
  int id;
  //taskqueue* my_taskq; lista circular
  task* tasks;
  struct _worker* workers;
  long num_workers;
  int logged_worker;
} worker;


/** Global State **/
//static dequeue  localQueue;    // double ended queue of local only work
//verificar como terminar o WS
//static enum colors my_color;      // Ring-based termination detection 

static int         last_steal;    // Rank of last thread stolen from
static long        chunks_recvd;  // Total messages received
static long        chunks_sent;   // Total messages sent
static long        ctrl_recvd;    // Total messages received
static long        ctrl_sent;     // Total messages sent

/** Global Parameters: Set in ss_init() **/
static int comm_size, comm_rank;
//static const int default_polling_interval = 32;

// Inicio da funcao principal
int main(int argc, char* argv[]) {

    
    int         p;             /* number of processes  */
    int         source;        /* rank of sender       */
    int         dest;          /* rank of receiver     */
    int         tag = 0;       /* tag for messages     */
    MPI_Status  status;        /* return status for    */
                               /* receive              */
    int i;
    dequeue localQueue[p]; /* Lista para cada processador */
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

  /* initializations for test */
  for(size_t i = 0; i < p; i++)
  {
    localQueue[p]= NULL;
  }
  

// Passo 2. Envie/receba o rank
// Passo 2.1.Envia o seu rank para a tarefa p-1-rank
    MPI_Send(&comm_rank, 1, MPI_INT, p-1-comm_rank, tag, MPI_COMM_WORLD);
// Passo 2.2. Receba o rank da tarefa p-1-rank 
    MPI_Recv(&dest, 1, MPI_INT, p-1-comm_rank, tag, MPI_COMM_WORLD, &status);

    printf("Meu rank %d rank recebido %d\n", comm_rank, dest);

    MPI_Finalize();
} /* main */
