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
extern void rtasm_opsize16(nativeblockinfo* nat);
extern void rtasm_offsize16(nativeblockinfo* nat);
extern void rtasm_mem(nativeblockinfo* nat, rtasm_mtype* mem);
extern rtasm_mtype rtasm_reg(uint3 r);
extern rtasm_mtype rtasm_ind(uint3 r);
extern rtasm_mtype rtasm_ind8(uint3 base, uint3 offset);
extern rtasm_mtype rtasm_ind32(uint3 base, uint5 offset);
extern rtasm_mtype rtasm_scind(uint3 base, uint3 index, uint3 scale);
extern rtasm_mtype rtasm_scind8(uint3 base, uint3 index, uint3 scale,
                                uint3 offset);
extern rtasm_mtype rtasm_scind32(uint3 base, uint3 index, uint3 scale,
                                 uint5 offset);
extern rtasm_mtype rtasm_addr(uint5 address);

#include "rtasm_fns.h"

#endif
