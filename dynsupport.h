#ifndef DYNSUPPORT_H
#define DYNSUPPORT_H 1

#include "defs.h"
#include "memory.h"

/* do not call these functions from C, they don't quite obey the usual
 * calling mechanism (return values in eax & ecx)
 */

extern int dynsupport_readdataword(meminfo* mem, uint5 address,
  tlbentry* tlb);

extern void dynsupport_writeword(meminfo* mem, uint5 address, uint5 data,
  tlbentry* tlb);

#endif
