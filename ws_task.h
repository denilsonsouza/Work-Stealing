#ifndef WS_TASK_H_
#define WS_TASK_H_
#include "ws_dequeue.h"

/*Difine the task for work-stealing*/

typedef struct task {
  void* result;
  dequeue *list_task;
<<<<<<< HEAD
 
=======
>>>>>>> 1ccdff882f859c4f23a698024a1ca43a03668dc2
} task;


#endif /* TASK_H_ */
