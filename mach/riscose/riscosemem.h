#ifndef RISCOSEMEM_H
#define RISCOSEMEM_H 1

#include "defs.h"
#include "memory.h"

extern const mem_readbank mem_rrom0;
extern const mem_writebank mem_wnull;
extern const mem_readbank mem_rbank0;
extern const mem_writebank mem_wbank0;

extern void memory_subinitialise(meminfo* mem);

void memory_writebytebank0(meminfo* mem, uint5 addr, uint5 data);
void memory_writehalfbank0(meminfo* mem, uint5 addr, uint5 data);
void memory_writewordbank0(meminfo* mem, uint5 addr, uint5 data);

uint5 memory_readrom0(meminfo* mem, uint5 addr);
uint5 memory_readhalfrom0(meminfo* mem, uint5 addr);
uint5 memory_readbyterom0(meminfo* mem, uint5 addr);

uint5 memory_readbytebank0(meminfo* mem, uint5 addr);
uint5 memory_readhalfbank0(meminfo* mem, uint5 addr);
uint5 memory_readwordbank0(meminfo* mem, uint5 addr);

#endif
