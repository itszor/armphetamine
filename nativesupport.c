#include <stdio.h>

#include "defs.h"
#include "registers.h"
#include "memory.h"
#include "nativesupport.h"
#include "execute.h"
#include "fakesys.h"
#include "hash.h"
#include "block.h"

void nativesupport_invoke2(machineinfo* machine, nativeblockinfo* code)
{
  int o1, o2;
  registerinfo* reg = machine->reg;
  
  fprintf(stderr, "--> entering native code\n");
  
  asm __volatile__(
    "pushl %%ebp\n\t"
    "movl %2,%%ebp\n\t"
    "call *%3\n\t"
    "popl %%ebp"
    : "=r" (o1), "=r" (o2)
    : "a" (reg), "b" (code->base)
    : "cx", "dx", "si", "di", "memory");
  
  fprintf(stderr, "<-- leaving native code\n");
}

#ifdef I386

// glue code used to execute native code fragments
void nativesupport_invoke(machineinfo* machine, nativeblockinfo* code)
{
  registerinfo* reg = machine->reg;
  hashentry* h;
  blockinfo* blk;
  uint5 i;
  extern uint5 nativehack;

  reg->cflag = FLAG(c);
  reg->vflag = FLAG(v);
  reg->nflag = FLAG(n);
  reg->zflag = FLAG(z);
  reg->r[15] &= 0x3fffffc;
  
  // for RISC OS SWI faking
  nativehack = 1;

  do {
    int o1, o2;
/*    fprintf(stderr, "Calling code at %x\n", code->base);
    for (i=0; i<16; i++) fprintf(stderr, "r%d=%x ", i, machine->reg[i]);
    fprintf(stderr, "\n");*/
    // "q" means registers a to d - not perfect, but OK
    // doesn't seem to understand clobbering %ebp, whoops
    asm __volatile__("pushl %%ebp\n\t" \
        "movl %2,%%ebp\n\t" \
        "call *%3\n\t" \
        "popl %%ebp"
        : "=r" (o1), "=r" (o2)
        : "a" (reg), "b" (code->base)
        : "cx", "dx", "si", "di", "memory");
  /*  fprintf(stderr, "Returning\n", code->base);
    for (i=0; i<16; i++) fprintf(stderr, "r%d=%x ", i, machine->reg[i]);
    fprintf(stderr, "\n");*/
    h = hash_lookup(machine->blocks, reg->r[15]-8);
    if (h) blk = h->data; else blk = 0;
    if (blk) code = blk->native; else code = 0;
  } while (code);

  nativehack = 0;
  
      /*"m" (reg->cflag),
        "m" (reg->vflag), "m" (reg->nflag),
        "m" (reg->zflag)*/
  FLAG(c) = reg->cflag;
  FLAG(v) = reg->vflag;
  FLAG(n) = reg->nflag;
  FLAG(z) = reg->zflag;
}
#else
void nativesupport_invoke(machineinfo* machine, nativeblockinfo* code)
{
}
#endif

void nativesupport_swi(machineinfo* machine, uint5 number)
{
  // just one more level of indirection...
  fake_syscall(machine, number);
}

void nativesupport_writebyte(meminfo* mem, uint5 address, uint5 byte)
{
  uint3* addr = (uint3*)memory_lookup(mem, (uint5*)address);
  *addr = byte;
//  fprintf(stderr, "STB @ %x\n", *(uint5*)memory_lookup(mem, (uint5*)0));
  #ifdef VIRTUALFRAMEBUFFER
  memory_postwrite(mem, (uint5*)address);
  #endif
}

// QUICKWRITE assumes words are always written to word boundaries
void nativesupport_writeword(meminfo* mem, uint5 address, uint5 word)
{
  uint5* addr = memory_lookup(mem, (uint5*)address);
  #ifndef QUICKWRITE
  addr = (uint5*)((uint5)addr&~3);
  #endif
  *addr = word;
  #ifdef VIRTUALFRAMEBUFFER
  memory_postwrite(mem, (uint5*)address);
  #endif
//  fprintf(stderr, "Writing %x to %x\n", word, address);
}

uint5 nativesupport_readbyte(meminfo* mem, uint5 address)
{
  uint3* addr = (uint3*)memory_lookup(mem, (uint5*)address);
//  fprintf(stderr, "LDB @ %x\n", *(uint5*)memory_lookup(mem, (uint5*)0));
  return *addr;
}

// QUICKREAD assumes words are always read from word boundaries
uint5 nativesupport_readword(meminfo* mem, uint5 address)
{
  uint5* addr = memory_lookup(mem, (uint5*)address);
#ifdef QUICKREAD
  return *addr;
#else
  uint5 byteoffset = (uint5)addr & 3;
  addr = (uint5*) ((uint5)addr&~3);
  return ROR(*addr, byteoffset*8);
#endif
}

uint5 nativesupport_reconstructpc(uint5 addr, registerinfo* reg)
{
#ifdef ARM26BIT
  uint5 val = addr | (reg->mode) | (reg->intmask<<26) | (reg->vflag<<28) |
              (reg->cflag<<29) | (reg->zflag<<30) | (reg->nflag<<31);
#else
  uint5 val = addr;
#endif
//  fprintf(stderr, "reconstructing pc, value %x (addr %x)\n", val, addr);
  return val;
}
