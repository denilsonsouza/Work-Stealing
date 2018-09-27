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
static int comm_size, comm_rank,msg;
static MPI_Request ws_request[WS_NUM_MSG_TYPE];
static MPI_Request wrin_request; // Incoming steal request 

void ws_make_progress(worker *s){

  MPI_Status status;
  int flag2, index, index2, victim;
  void *work;
  int attempt = 0;
  if (deq_isEmpty(s[comm_rank].ws_task)){
    // randomico processador
    //do{
    victim = 0;//random() % 4;
    printf("Rank: %d , Victim: %d\n",comm_rank,victim);
    //while(!ativo(all))
    msg = WS_RESQUEST_WORK_MSG;

    MPI_Send(&msg, 1, MPI_INT, 0, WS_RESQUEST_WORK_MSG, MPI_COMM_WORLD);
    
    
  }else{ // processa  
    
    
    while (!deq_isEmpty(s[comm_rank].ws_task)){
        //remove
        void *element;
        element = deq_popFront(s[comm_rank].ws_task); // remove
        printf("%s::%d \n", (char *)element, deq_length(s[comm_rank].ws_task));
        sleep(random() % 2);
        process_work_request(s[comm_rank].ws_task);

    }

    printf("rank %d finishing\n", comm_rank);
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


void process_work_request(dequeue s){
    
    MPI_Status status;
    int index, flag;
    
    MPI_Irecv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, WS_RESQUEST_WORK_MSG, MPI_COMM_WORLD, &(ws_request[WS_TASK_REQUEST]));
    MPI_Irecv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, WS_RESQUEST_WORK_MSG, MPI_COMM_WORLD, &(ws_request[WS_EMPTY_MSG]));
    
  
    
    
    if (deq_length(s)){
            // pop task in FIFO order
            void *element;
            element = deq_popBack(s);
            //task_t task = tasks.back();
            //tasks.pop_back();

            // synchronous user processing
            //steal(task);
            //printf("%s::%d \n", (char *)element, deq_length(s));
            sleep(random() % 2);

            // give it to the thift
            //thief.tasks.push_back(task);
    }
        // notify the thief that the operation is completed
       // thief.steal_ack.store(1, memory_order_release);
       return;
 }


//Make request of the steal...
int mpi_workstealing(worker *steal){
   
   int i,msg,victim, ws_flag, ws_index;
   MPI_Request ws_request2[WS_NUM_MSG_TYPE];
   MPI_Status ws_status;
   void * element;
   if(!deq_isEmpty(steal[comm_rank].ws_task))
     return 1;
   steal[comm_rank].logged_worker=1; // sem tarefa

  
  while(1){
    do{
      victim = rand()% steal->num_workers;
    }while(victim!=comm_rank);

    msg =  WS_RESQUEST_WORK_MSG;
    MPI_Send(&msg,1,MPI_INT,victim,WS_RESQUEST_WORK_MSG,MPI_COMM_WORLD);
    MPI_Irecv(&element, 1, MPI_INT, victim, WS_TASK_MSG,MPI_COMM_WORLD, &(ws_request[WS_TASK_REQUEST]) );
    MPI_Irecv(&element, 1, MPI_INT, victim, WS_EMPTY_MSG,MPI_COMM_WORLD, &(ws_request[WS_EMPTY_REQUEST]) );
    ws_flag = 0;
    do {
      MPI_Testany(WS_NUM_MSG_TYPE, ws_request, &ws_index,&ws_flag, &ws_status );
     // mpi_ws_process_nonblocking(workst, benchmark);
    } while ( ws_flag == 0 );
    for (i = 0; i < WS_NUM_MSG_TYPE; i ++)
      if ( i != ws_index)
      MPI_Cancel(&(ws_request[i]));
      switch ( ws_status.MPI_TAG ) {
        case WS_TASK_MSG :
          element = deq_popBack(steal[comm_size].ws_task);
          return 1;
        case WS_EMPTY_MSG :
          //set_proc_inactive(&(workst -> ws_proc_table),ws_status.MPI_SOURCE);
          break;
      }
    }
 return 0;
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
  
  if(mpi_workstealing(workers)==1){
    printf("OK.");
  }else{
    printf("Ops.");
  }

  //ws_make_progress(workers);

  MPI_Finalize();
} /* main */
