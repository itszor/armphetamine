#include <stdio.h>
#include <assert.h>

#include "profile.h"
#include "transmap.h"
#include "cnew.h"

profile_state* profile_initialise(void)
{
  profile_state* pstate = cnew(profile_state);
  
  pstate->start = -1;
  pstate->currenttrans = 0;
  
  return pstate;
}

void profile_feedseqaddr(meminfo* mem, profile_state* pstate, uint5 addr)
{
  if (pstate->start != -1u)
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
      /* that's OK, we're still inside the same block */
      return;
    }
    else
    {
      /* outside block, invalidate. This shouldn't happen */
      pstate->start = -1;
      assert(!"Non-sequential address?");
    }
  }
}

void profile_feednseqaddr(meminfo* mem, profile_state* pstate, uint5 addr)
{
  /* We're starting at a new leader address. Either we need to
   * reinstate a previous block & increase its usage count, or
   * start a new block afresh.
   */
  transmap_entry* entryataddr = transmap_getentry(mem, addr);

  if (entryataddr)
  {
    /* re-enable this as current block */
    pstate->currenttrans = entryataddr;
    pstate->start = addr;

    if (entryataddr->code)
    {
      /* run recompiled code */
      fprintf(mem->trace, "Running code for %.8x\n", pstate->start);
    }
    else if (++pstate->currenttrans->usecount > RECOMPILE_THRESHOLD)
    {
      /* do the funky shit */
      fprintf(mem->trace, "BLING! Found block, start %.8x, length %.8x\n",
        pstate->start, pstate->currenttrans->length);
      fflush(mem->trace);
      entryataddr->code = (char*)0x100;
    }
  }
  else
  {
    /* make this the new leader address */
    pstate->start = addr;
    pstate->currenttrans = transmap_new();
    /* patch us into the transmap */
    transmap_addentry(mem, addr, pstate->currenttrans);
  }
}
