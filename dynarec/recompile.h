#ifndef RECOMPILE_H
#define RECOMPILE_H 1

#include "defs.h"
#include "x86asm.h"
#include "machine.h"

extern nativeblockinfo* recompile_chunk(machineinfo* machine, uint5 start,
  uint5 end);

#endif
