#ifndef OSTIMER_H
#define OSTIMER_H 1

#include "memory.h"

extern void ostimer_write(meminfo* mem, uint5 address, uint5 data);
extern uint5 ostimer_read(meminfo* mem, uint5 address);

#endif
