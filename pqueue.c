#include <assert.h>

#include "cnew.h"
#include "pqueue.h"

// Priority queue handling: algorithmi from The White Book (naturally)

pqueue* pqueue_new(void)
{
  pqueue* pq = cnew(pqueue);
  pq->item = cnewarray(pqueueitem*, 4);
  pq->length = 0;
  pq->size = 4;
  
  return pq;
}

void pqueue_delete(pqueue* pq)
{
  // free things pointed to by pq->data[n]->item...
  free(pq->item);
  free(pq);
}

pqueueitem* pqueue_newitem(uint5 priority)
{
  pqueueitem* pqi = cnew(pqueueitem);
  
  pqi->priority = priority;
  pqi->data = 0;
  
  return pqi;
}

void pqueue_deleteitem(pqueueitem* pqi)
{
  free(pqi);
}

pqueueitem* pqueue_insert(pqueue* pq, uint5 priority)
{
  sint5 i;
  
  if (++pq->length==pq->size)
    pq->item = realloc(pq->item, sizeof(pqueueitem*) * (pq->size*=2));
  
  i = pq->length-1;

  while (i>0 && pq->item[pq_PARENT(i)]->priority>priority)
  {
    pq->item[i] = pq->item[pq_PARENT(i)];
    i = pq_PARENT(i);
  }
  
  return pq->item[i] = pqueue_newitem(priority);
}

pqueueitem* pqueue_head(pqueue* pq)
{
  return pq->length ? pq->item[0] : 0;
}

// you're expected to deallocate the item yourself after calling this
pqueueitem* pqueue_extract(pqueue* pq)
{
  pqueueitem* max;

  if (!pq->length) return 0;
  
  max = pq->item[0];
  pq->item[0] = pq->item[--pq->length];
  pqueue_heapify(pq, 0);
  
  return max;
}

void pqueue_heapify(pqueue* pq, uint5 i)
{
  uint5 l = pq_LEFT(i), r = pq_RIGHT(i), largest;

  largest = (l<pq->length && pq->item[l]->priority<pq->item[i]->priority)
              ? l : i;

  if (r<pq->length && pq->item[r]->priority<pq->item[largest]->priority)
    largest = r;

  if (largest != i)
  {
    pqueueitem* temp = pq->item[i];
    pq->item[i] = pq->item[largest];
    pq->item[largest] = temp;
    pqueue_heapify(pq, largest);
  }
}

