#ifndef X86DISM_H
#define X86DISM_H 1

#include "x86asm.h"

extern void x86dism_partblock(nativeblockinfo* blk, uint5 start, uint5 length);
extern void x86dism_block(nativeblockinfo* blk);

#endif
