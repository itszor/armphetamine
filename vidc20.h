#ifndef VIDC20_H
#define VIDC20_H 1

#include "defs.h"
#include "memory.h"

extern void vidc20_writebyte(meminfo* mem, uint5 address, uint5 data);
extern void vidc20_writeword(meminfo* mem, uint5 address, uint5 data);

#endif
