#ifndef FIFO_H
#define FIFO_H 1

#include "defs.h"

typedef struct {
  uint3* data;
  uint5 length;
  uint5 head;
  uint5 tail;
} fifo_info;

extern fifo_info* fifo_create(uint5 size);
extern void fifo_reset(fifo_info*);
extern void fifo_delete(fifo_info*);
extern void fifo_write(fifo_info* fifo, uint3 byte);
extern uint5 fifo_read(fifo_info* fifo);
extern uint5 fifo_empty(fifo_info* fifo);
extern uint5 fifo_full(fifo_info* fifo);

#endif
