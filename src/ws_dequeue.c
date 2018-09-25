#include "../include/ws_dequeue.h"
#include "../include/ws_dlist.h"
#include <stdlib.h>

dequeue deq_create(){
  dequeue q = malloc(sizeof(struct dequeue));
  q -> head = dcons(NULL, NULL, NULL);
  q -> tail = dcons(NULL, NULL, NULL);
  q -> head -> next = q -> tail; //auto apontamento, para definir vazio
  q -> tail -> prev = q -> head;
  return q;
}

int deq_isEmpty(dequeue q) {
  return q -> head -> next == q -> tail;
}
/* insert an element at the front of the dqueue */
void deq_pushFront(dequeue q, void *element) {
  create_and_link(element, q->head, q->head->next);
}
/* insert an element at the back of the dqueue */ 
void deq_pushBack(dequeue q, void *element){
  create_and_link(element, q->tail->prev, q->tail);
}
/* delete an element from the front of the dqueue and return it */
void* deq_popFront(dequeue q){
  return unlink_and_free(q->head->next);
}
/* delete an element from the back of the dqueue and return it */
void* deq_popBack(dequeue q) {
  return unlink_and_free(q->tail->prev);
}

void* deq_peekFront(dequeue q){
  return q->head->next->element;
}

void* deq_peekBack(dequeue q){
  return q->tail->prev->element;
}

void deq_mkEmpty(dequeue q){
  while(!deq_isEmpty(q))
    deq_popFront(q);
}

int deq_length(dequeue q) {
	int count;
	dlist e = q->head->next;

	for (count = 0; e != q->tail; count++)
	e = e->next;

	return count;
}