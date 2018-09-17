#include <stdio.h>
#include <stdlib.h>
<<<<<<< HEAD
#include <mpi.h>
#include <assert.h>
#include "ws_task.h"
#include "ws_dequeue.h"



/** Global State **/
static dequeue     localQueue;    // double ended queue of local only work

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

   // int         my_rank;       /* rank of process      */
    int         p;             /* number of processes  */
    int         source;        /* rank of sender       */
    int         dest;          /* rank of receiver     */
    int         tag = 0;       /* tag for messages     */
    MPI_Status  status;        /* return status for    */
                               /* receive              */
// Passo 1. Inicializacao
// Passo 1.1. Inicilialize o MPI
    MPI_Init(&argc, &argv);
// Passo 1.2. Determine o numero da tarefa
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
// Passo 1.3. Determine o numero de tarefas
    MPI_Comm_size(MPI_COMM_WORLD, &p);
// Passo 2. Envie/receba o rank
// Passo 2.1.Envia o seu rank para a tarefa p-1-rank
    MPI_Send(&comm_rank, 1, MPI_INT, p-1-comm_rank, tag, MPI_COMM_WORLD);
// Passo 2.2. Receba o rank da tarefa p-1-rank 
    MPI_Recv(&dest, 1, MPI_INT, p-1-comm_rank, tag, MPI_COMM_WORLD, &status);
// Passo 3. Imprima o rank recebido
    printf("Meu rank %d rank recebido %d\n", comm_rank, dest);
// Passo 4. Finalize o MPI
    MPI_Finalize();
} /* main */
=======
#include <pthread.h>
#include <assert.h>
#include <utmpx.h>
#include <unistd.h>
#include "ws_task.h"

>>>>>>> 1ccdff882f859c4f23a698024a1ca43a03668dc2
