#ifndef NATIVESUPPORT_H
#define NATIVESUPPORT_H 1

#include "defs.h"
#include "registers.h"
#include "memory.h"
#include "x86asm.h"
#include "machine.h"

extern void nativesupport_invoke2(machineinfo* machine, uint3* code);
extern void nativesupport_invoke(machineinfo* machine, nativeblockinfo* code);
extern void nativesupport_swi(machineinfo* machine, uint5 number);
extern void nativesupport_writebyte(meminfo* mem, uint5 address, uint5 byte);
extern void nativesupport_writeword(meminfo* mem, uint5 address, uint5 byte);
extern uint5 nativesupport_readbyte(meminfo* mem, uint5 address);
extern uint5 nativesupport_readword(meminfo* mem, uint5 address);
extern uint5 nativesupport_reconstructpc(uint5 addr, registerinfo* reg);

#endif
