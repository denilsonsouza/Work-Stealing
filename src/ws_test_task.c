#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>
#include "../include/ws_task.h"
#include "../include/ws_dequeue.h"
#include <string.h>

#define WS_RESQUEST_WORK_MSG 2 // tipo requisicao
#define WS_TASK_MSG 3
#define WS_EMPTY_MSG 4

#define WS_NUM_MSG_TYPE 2 // Numero de tipos de mensagens
#define WS_TASK_REQUEST 0
#define WS_EMPTY_REQUEST 1

typedef struct _worker
{
  int id;
  dequeue ws_task;
  int num_workers;
  int logged_worker;
} worker;

/* uso posterior
static int last_steal;    // Rank of last thread stolen from
static long chunks_recvd; // Total messages received
static long chunks_sent;  // Total messages sent
static long ctrl_recvd;   // Total messages received
static long ctrl_sent;    // Total messages sent
*/

/** Global Parameters: Set in ss_init() **/
static int comm_size, comm_rank;
static MPI_Request ws_request[WS_NUM_MSG_TYPE];

void ws_make_progress(worker *s)
{

  MPI_Status status;
  int msg, flag2, index, index2, victim;
  void *work;
  int attempt = 0;
  if (deq_isEmpty(s[comm_rank].ws_task))
  {
    // randomico processador
    //do{
    victim = random() % 4;

    //while(!ativo(all))
    msg = WS_RESQUEST_WORK_MSG;

    /* MPI_Send(&msg, 1, MPI_INT, victim, WS_RESQUEST_WORK_MSG, MPI_COMM_WORLD);

    MPI_Irecv(&flag2, 1, MPI_INT, victim, WS_TASK_MSG, MPI_COMM_WORLD, &ws_request[WS_TASK_REQUEST]);

    MPI_Irecv(&flag2, 1, MPI_INT, victim, WS_TASK_MSG, MPI_COMM_WORLD, &ws_request[WS_EMPTY_MSG]); */
    flag2 = 1;
    do
    {
      printf("Test tag %d!!\n", comm_rank);
    } while (flag2 == 0);
    /*   for (int i=0; i < WS_NUM_MSG_TYPE;i++)
        if(i!=index2)
          MPI_Cancel(&(ws_request[i]));
     */

    switch (status.MPI_TAG)
    {
    case WS_TASK_MSG:
      printf("Ops Task");
      break;

    case WS_EMPTY_MSG:
      printf("Ops Zerp");
      break;
    }

    printf("Tentativa: %d \n", attempt);
    attempt = attempt + 1;
  }
  else
  { // processa
    index = deq_length(s[comm_rank].ws_task);
    while (1)
    {
      MPI_Status status2;
      int flag;
      index = deq_length(s[comm_rank].ws_task);
      printf("Tamanho %d \n", deq_length(s[comm_rank].ws_task));

      while (deq_length(s[comm_rank].ws_task) > 0)
      {
        //remove
        void *element;
        element = deq_popFront(s[comm_rank].ws_task); // remove
        printf("%s::%d \n", (char *)element, deq_length(s[comm_rank].ws_task));
        sleep(random() % 2);
      }

      if (index == 0)
      {
        printf("rank %d finishing\n", comm_rank);
        break;
      }
    } //while

  } //end-if

} //end-method

void ss_init(worker *s, int num){
 dequeue localQueue;
  for (size_t i = 0; i < num; i++){
    s[i].id = i;
    s[i].num_workers = num;
    localQueue = deq_create();
    s[i].ws_task = localQueue;
  }

  // printf("Meu rank %d \n", comm_rank);
  //Shots ---
  for (size_t i = 0; i < 32; i++){
    void *element;
    element = malloc(sizeof(void));
    sprintf(element, "shot-%zu", i);
    deq_pushBack(s[0].ws_task, element);
  }
}

int main(int argc, char *argv[])
{

  int p;             /* number of processes       */
  int source;        /* rank of sender            */
  int dest;          /* rank of receiver          */
  int tag = 0;       /* tag for messages          */
  MPI_Status status; /* return status for receive */

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  worker workers[p]; /* Works for work-stealing  */
  ss_init(workers,p);
  ws_make_progress(workers);

  //MPI_Send(&comm_rank, 1, MPI_INT, p - 1 - comm_rank, tag, MPI_COMM_WORLD);
  //MPI_Recv(&dest, 1, MPI_INT, p - 1 - comm_rank, tag, MPI_COMM_WORLD, &status);
  //printf("Meu rank %d rank recebido %d\n", comm_rank, dest);

  MPI_Finalize();
} /* main */
