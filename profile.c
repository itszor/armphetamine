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

void profile_feedaddr(meminfo* mem, profile_state* pstate, uint5 addr)
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
      /* re-enable this as current block */
      pstate->currenttrans = entryataddr;
      pstate->start = addr;

      if (++pstate->currenttrans->usecount > RECOMPILE_THRESHOLD)
      {
        /* do the funky shit */
        fprintf(stderr, "BLING! Found block, start %.8x, end %.8x\n",
          pstate->start, pstate->currenttrans->length);
        exit(0);
      }
    }
    else
    {
      pstate->start = addr;
      pstate->length = 4;
      pstate->currenttrans = transmap_new();
      transmap_addentry(mem, addr, pstate->currenttrans);
    }
  }
  else if ((pstate->start & 0xfffff000) != ((pstate->start+addr) & 0xfffff000))
  {
    // have we crossed a 4kbyte boundary?
    fprintf(stderr, "Crossed 4k boundary\n");
/*    assert(addr > pstate->start);
    pstate->currenttrans->length = 4096 - (pstate->start & 0xfff);*/
    /* invalidate current block & recur */
    pstate->start = -1;
    profile_feedaddr(mem, pstate, addr);
  }
  else
  {
    if (addr >= pstate->start + pstate->currenttrans->length)
    {
      /* current block is valid. Are we some amount beyond the previously-known
       * maximum length?
       */
      pstate->currenttrans->length = 4+addr-pstate->start;
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
