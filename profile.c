#include <stdio.h>
#include <assert.h>

#include "profile.h"
#include "transmap.h"
#include "cnew.h"

profile_state* profile_initialise(void)
{
  profile_state* pstate = cnew(profile_state);
  
  pstate->start = -1;
  pstate->length = 0;
  pstate->currenttrans = 0;
}

void profile_startchunk(profile_state* pstate, uint5 start)
{
  pstate->context = 0;
  pstate->ctxtmask = 0;
  pstate->start = start;
  pstate->length = 0;
  pstate->currenttrans = cnew(transmap_entry);
}

void profile_feedaddr(meminfo* mem, profile_state* pstate, uint5 addr)
{
  // have we crossed a 4kbyte boundary?
  if ((pstate->start & 0xfffff000) != ((pstate->start+addr) & 0xfffff000))
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
