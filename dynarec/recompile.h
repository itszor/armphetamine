#ifndef RECOMPILE_H
#define RECOMPILE_H 1

#include "libjtype/defs.h"
#include "dynarec/arch/x86/x86asm.h"
#include "core/machine.h"

extern nativeblockinfo* recompile_chunk(machineinfo* machine, uint5 start,
  uint5 end);

#endif
