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
  
  return pstate;
}

void profile_startchunk(profile_state* pstate, uint5 start)
{
  pstate->context = 0;
  pstate->ctxtmask = 0;
  pstate->start = start;
  pstate->length = 4;
  pstate->currenttrans = transmap_new();
}

void profile_feedaddr(meminfo* mem, profile_state* pstate, uint5 addr)
{
  // have we crossed a 4kbyte boundary?
  if ((pstate->start & 0xfffff000) != ((pstate->start+addr) & 0xfffff000))
  {
    fprintf(stderr, "Crossed 4k boundary\n");
    assert(addr > pstate->start);
    pstate->currenttrans->length = 4096 - (pstate->start & 0xfff);
    /* invalidate current block & recur */
    pstate->start = -1;
    profile_feedaddr(mem, pstate, addr);
  }
  else
  {
    if (pstate->start == -1u)
    {
      /* current block state has been invalidated. Either we need to
       * reinstate a previous block & increase its usage count, or
       * start a new block afresh.
       */
      transmap_entry* entryataddr = transmap_getentry(mem, addr);

      if (entryataddr)
      {
        pstate->currenttrans->usecount++;
      }
      else
      {
        profile_startchunk(pstate, addr);
        transmap_addentry(mem, addr, pstate->currenttrans);
      }
    }
    else if (addr == pstate->start + pstate->currenttrans->length)
    {
      /* current block is valid. Are we one step beyond the previously-known
       * maximum length?
       */
      pstate->currenttrans->length += 4;
    }
    else if (addr>=pstate->start &&
             addr < pstate->start + pstate->currenttrans->length)
    {
      /* that's OK */
    }
    else
    {
      /* outside block, invalidate & recur */
      pstate->start = -1;
      profile_feedaddr(mem, pstate, addr);
    }
  }
}
