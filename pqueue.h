#ifndef PQUEUE_H
#define PQUEUE_H 1

#include "defs.h"

typedef struct {
  void* item;
  uint5 priority;
} pqueueitem;

typedef struct {
  pqueueitem** data;
  uint5 length, size;
} pqueue;

// Not particularly tidy, because we're counting from zero
#define pq_LEFT(X) ((((X)+1)<<1)-1)
#define pq_RIGHT(X) (((X)+1)<<1)
#define pq_PARENT(X) ((((X)+1)>>1)-1)

extern pqueue* pqueue_new(void);
extern void pqueue_delete(pqueue* pq);
extern pqueueitem* pqueue_newitem(uint5 key);
extern void pqueue_deleteitem(pqueueitem* it);
extern pqueueitem* pqueue_insert(pqueue** pq, uint5 priority);
extern pqueueitem* pqueue_head(pqueue* pq);
extern pqueueitem* pqueue_extract(pqueue* pq);
extern void pqueue_heapify(pqueue* pq, uint5 i);

#endif
