#ifndef LARTMEM_H
#define LARTMEM_H 1

#include "memory.h"

extern const mem_readbank mem_rzero;

extern const mem_readbank mem_rrom0;
extern const mem_readbank mem_rrom1;
extern const mem_readbank mem_rfault;
extern const mem_readbank mem_rnull;
extern const mem_readbank mem_rbank0;
extern const mem_readbank mem_rbank1;
extern const mem_readbank mem_rbank2;
extern const mem_readbank mem_rbank3;

extern const mem_writebank mem_wfault;
extern const mem_writebank mem_wnull;
extern const mem_writebank mem_wbank0;
extern const mem_writebank mem_wbank1;
extern const mem_writebank mem_wbank2;
extern const mem_writebank mem_wbank3;

uint5 memory_readzero(meminfo* mem, uint5 addr);

void memory_writebytebank0(meminfo* mem, uint5 addr, uint5 data);
void memory_writehalfbank0(meminfo* mem, uint5 addr, uint5 data);
void memory_writewordbank0(meminfo* mem, uint5 addr, uint5 data);

void memory_writebytebank1(meminfo* mem, uint5 addr, uint5 data);
void memory_writehalfbank1(meminfo* mem, uint5 addr, uint5 data);
void memory_writewordbank1(meminfo* mem, uint5 addr, uint5 data);

void memory_writebytebank2(meminfo* mem, uint5 addr, uint5 data);
void memory_writehalfbank2(meminfo* mem, uint5 addr, uint5 data);
void memory_writewordbank2(meminfo* mem, uint5 addr, uint5 data);

void memory_writebytebank3(meminfo* mem, uint5 addr, uint5 data);
void memory_writehalfbank3(meminfo* mem, uint5 addr, uint5 data);
void memory_writewordbank3(meminfo* mem, uint5 addr, uint5 data);

uint5 memory_readrom0(meminfo* mem, uint5 addr);
uint5 memory_readhalfrom0(meminfo* mem, uint5 addr);
uint5 memory_readbyterom0(meminfo* mem, uint5 addr);

void memory_writerom0(meminfo* mem, uint5 addr, uint5 data);

uint5 memory_readrom1(meminfo* mem, uint5 addr);
uint5 memory_readhalfrom1(meminfo* mem, uint5 addr);
uint5 memory_readbyterom1(meminfo* mem, uint5 addr);

uint5 memory_readbytebank0(meminfo* mem, uint5 addr);
uint5 memory_readhalfbank0(meminfo* mem, uint5 addr);
uint5 memory_readwordbank0(meminfo* mem, uint5 addr);

uint5 memory_readbytebank1(meminfo* mem, uint5 addr);
uint5 memory_readhalfbank1(meminfo* mem, uint5 addr);
uint5 memory_readwordbank1(meminfo* mem, uint5 addr);

uint5 memory_readbytebank2(meminfo* mem, uint5 addr);
uint5 memory_readhalfbank2(meminfo* mem, uint5 addr);
uint5 memory_readwordbank2(meminfo* mem, uint5 addr);

uint5 memory_readbytebank3(meminfo* mem, uint5 addr);
uint5 memory_readhalfbank3(meminfo* mem, uint5 addr);
uint5 memory_readwordbank3(meminfo* mem, uint5 addr);

#endif
