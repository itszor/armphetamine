#ifndef IOMD_H
#define IOMD_H 1

#include "memory.h"

extern void iomd_writeword(meminfo* mem, uint5 address, uint5 data);
extern uint5 iomd_readword(meminfo* mem, uint5 address);
extern void iomd_writebyte(meminfo* mem, uint5 address, uint5 data);
extern uint5 iomd_readbyte(meminfo* mem, uint5 address);

#endif
