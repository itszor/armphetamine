#ifndef RTASM_H
#define RTASM_H 1

#include "defs.h"
#include "x86asm.h"

typedef struct {
  uint3 mod;
  uint3 rm;
  uint3 scale;
  uint3 index;
  uint3 base;
  uint5 disp;
} rtasm_mtype;

extern void rtasm_putbyte(nativeblockinfo* nat, uint3 byte);
extern void rtasm_mem(nativeblockinfo* nat, rtasm_mtype* mem);

#include "rtasm_fns.h"

#endif
