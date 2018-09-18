#ifndef WS_TASK_H_
#define WS_TASK_H_
#include "ws_dequeue.h"

/*Difine the task for work-stealing*/

typedef struct task {
  void* result;
  struct task* parent;
  struct task** children; 
} task;


#endif /* TASK_H_ */
