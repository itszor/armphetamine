#include <assert.h>

#include "transmap.h"

void transmap_begin(transmap_profilestate* pstate, uint5 start)
{
  pstate->context = 0;
  pstate->ctxtmask = 0;
  pstate->start = start;
  pstate->length = 0;
}

void transmap_feedaddr(transmap_profilestate* pstate, uint5 addr)
{
  // have we crossed a 4kbyte boundary?
  if (pstate->start & 0xfffff000 != (pstate->start+addr & 0xfffff000))
  {
    fprintf(stderr, "Crossed 4k boundary\n");
    assert(addr > pstate->start);
    pstate->length = 4096 - (pstate->start & 0xfff);
  }
  else
  {
    pstate->length = addr - pstate->start;
  }
  
  
}
