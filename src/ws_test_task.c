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

// uso posterior
static int last_steal;    // Rank of last thread stolen from
static long chunks_recvd; // Total messages received
static long chunks_sent;  // Total messages sent
static long ctrl_recvd;   // Total messages received
static long ctrl_sent;    // Total messages sent

typedef struct _worker
{
  int id;
  dequeue ws_task;
  int num_workers;
  int logged_worker;
} worker;

/** Global Parameters: Set in ss_init() **/
static int comm_size, comm_rank;
static MPI_Request ws_request[WS_NUM_MSG_TYPE];
static MPI_Request wrin_request; // Incoming steal request

void ws_make_progress(worker *s)
{

  MPI_Status status;
  void *work;
  int attempt = 0;
  while (mpi_workstealing(s) == 1)
  {
    void *element;
    element = deq_popFront(s[comm_rank].ws_task); // remove
    printf("RANK:%d - Processa: %s \n", comm_rank, (char *)element);
    sleep(random() % 2);
    mpi_process_nonblocking(s);
  }
  printf("rank %d finishing\n", comm_rank);

} //end-method

void ss_init(worker *s, int num)
{
  dequeue localQueue;
  for (size_t i = 0; i < num; i++)
  {
    s[i].id = i;
    s[i].num_workers = num;
    localQueue = deq_create();
    s[i].logged_worker = 1; //marca todos com 1.. Possui tarefas!
    s[i].ws_task = localQueue;
  }

  // printf("Meu rank %d \n", comm_rank);
  //Shots ---
  for (size_t i = 0; i < 32; i++)
  {
    void *element;
    element = malloc(sizeof(void));
    sprintf(element, "shot-%zu", i);
    deq_pushBack(s[0  ].ws_task, element);
  }
}

//Make request of the steal...
int mpi_workstealing(worker *steal)
{

  int i, msg, victim, ws_flag, ws_index;
  MPI_Status ws_status;
  void *element;
  
  if (!deq_isEmpty(steal[comm_rank].ws_task)){
    return 1;
  }

  //steal[comm_rank].logged_worker = 1; // stop

  while (steal[comm_rank].logged_worker==1)
  {
    do
    {
      srand(time(NULL));
      printf("Ops!!\n");
      victim = 0;//rand() % steal->num_workers;
      if(comm_rank==0){
        return 0;
      }

    } while (victim == comm_rank);
    printf("RANK:%d - Send Menssage:%d\n", comm_rank, victim);
    msg = WS_RESQUEST_WORK_MSG;

    MPI_Send(&msg, 1, MPI_INT, victim, WS_RESQUEST_WORK_MSG, MPI_COMM_WORLD); //envia mensagem de roubo!
    MPI_Irecv(&element, 1, MPI_INT, victim, WS_EMPTY_MSG, MPI_COMM_WORLD, &(ws_request[WS_EMPTY_REQUEST]));
    MPI_Irecv(&element, 1, MPI_INT, victim, WS_TASK_MSG, MPI_COMM_WORLD, &(ws_request[WS_TASK_REQUEST]));
    
    
    ws_flag = 0;
    do
    {
     
      
      MPI_Testany(WS_NUM_MSG_TYPE, ws_request, &ws_index, &ws_flag, &ws_status);
      printf("RANK:%d - WAIT por %d \n", comm_rank,victim);
      sleep(2);
      mpi_process_nonblocking(steal);
    } while (ws_flag == 0);

    printf("Rank:%d - Passou!\n",comm_rank);
    for (i = 0; i < WS_NUM_MSG_TYPE; i++){
    printf(":::%d\n",ws_index); 
    if (i != ws_index){
        MPI_Cancel(&(ws_request[i]));
        printf("cancelou %d",i);
     }
    }
    printf("TAG: %d \n",ws_status.MPI_TAG);    
    switch (ws_status.MPI_TAG)
    {
    case WS_TASK_MSG:
      deq_pushFront(steal[comm_rank].ws_task, element); // rouba tarefa!
      printf("Recebeu??:%s\n", (char *)element);
      return 1;
    case WS_EMPTY_MSG:
      printf("RANK:%d Vazio!!-!\n",comm_rank);
      steal[comm_rank].logged_worker = 0; //marca sem tarefa!
      break;
    }
  }
  return 0;
}

int exist_active_proc(worker *s)
{
  int i, test = 1;
  for (i = 0; i < comm_rank; i++)
  {
    printf("Status Processo %d: %d\n",comm_rank,s[comm_rank].logged_worker);
    if (s[comm_rank].logged_worker == 1)
      test = 0;
    break;
  }
  if (test == 0)
  {
    return 0;
  }
  return test;
}



void mpi_process_nonblocking(worker *s)
{

  int flag = 0,msg, index; // Flag for non-blocking comm.
  MPI_Status status;        // Status for recv-like comm.

  void *element;
  //printf("Rank Test:%d \n", comm_rank);
   MPI_Irecv(&element, 1, MPI_INT, MPI_ANY_SOURCE, WS_RESQUEST_WORK_MSG, MPI_COMM_WORLD, &(ws_request[WS_TASK_REQUEST]));
   
 // MPI_Irecv(&element, 1, MPI_INT, MPI_ANY_SOURCE, WS_EMPTY_MSG, MPI_COMM_WORLD, &(ws_request[WS_EMPTY_REQUEST]));
  MPI_Test(&(ws_request[WS_TASK_REQUEST]), &flag, &status);
 
  if (flag)
  {
    ++ctrl_recvd;
   // MPI_Irecv(&element, 1, MPI_INT, MPI_ANY_SOURCE, WS_RESQUEST_WORK_MSG, MPI_COMM_WORLD, &(ws_request[WS_TASK_REQUEST]));
    index = status.MPI_SOURCE;
    //verifica se tem work
    if (!deq_isEmpty(s[comm_rank].ws_task))
    {
      printf("RANK:%d - Envia tarefa para :%d!\n", comm_rank, index);
      element = deq_popBack(s[comm_rank].ws_task);                       // remove
      printf("RANK:%d - Envia: %s \n", comm_rank, (char *)element);
      MPI_Send(&element, 1, MPI_INT, index, 3, MPI_COMM_WORLD); //envia mensagem de roubo!
    }
    else
    {
      ++ctrl_sent;
      MPI_Send(&element, 1, MPI_INT, index, 4, MPI_COMM_WORLD);
      printf("RANK:%d - Envia Sem Tarefa para %d!\n", comm_rank, index);
    }
  }
  return;
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

  ss_init(workers, p);

  ws_make_progress(workers);

  MPI_Finalize();
} /* main */
