#include <stdlib.h>

#include "fifo.h"
#include "cnew.h"

fifo_info* fifo_create(uint5 size)
{
  fifo_info* fifo = cnew(fifo_info);
  fifo->data = cnewarray(uint3, size);
  fifo->length = size;
  fifo_reset(fifo);
  return fifo;
}

void fifo_reset(fifo_info* fifo)
{
  fifo->head = 0;
  fifo->tail = 0;
}

void fifo_delete(fifo_info* fifo)
{
  free(fifo->data);
  free(fifo);
}

void fifo_write(fifo_info* fifo, uint3 byte)
{
  if ((fifo->tail-fifo->head & (fifo->length-1))==1) return;
  fifo->data[fifo->head++] = byte;
  if (fifo->head == fifo->length) fifo->head = 0;
}

uint5 fifo_read(fifo_info* fifo)
{
  if (fifo->tail==fifo->head) return -1;
  return fifo->data[fifo->tail++];
}
