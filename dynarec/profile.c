#include <stdio.h>
#include <assert.h>

#include "profile.h"
#include "transmap.h"
#include "cnew.h"
#include "nativesupport.h"
#include "recompile.h"

profile_state* profile_initialise(void)
{
  profile_state* pstate = jt_new(profile_state);
  
  pstate->start = -1;
  pstate->currenttrans = 0;
  
  return pstate;
}

void profile_feedseqaddr(profile_state* pstate, uint5 addr)
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
  else
  {
    assert(!"Invalid block");
  }
}

uint5 profile_feednseqaddr(machineinfo* machine, profile_state* pstate,
  uint5 addr)
{
  meminfo* mem = machine->mem;
  /* We're starting at a new leader address. Either we need to
   * reinstate a previous block & increase its usage count, or
   * start a new block afresh.
   */
  transmap_entry* entryataddr = transmap_getentry(mem, addr);
  pstate->start = addr;

  if (entryataddr)
  {
    if (entryataddr->code)
    {
      /* run recompiled code */
      fprintf(stderr, "Running code for %.8x\n", pstate->start);
      nativesupport_invoke2(machine, entryataddr->code);
      pstate->start = -1;
      /* & after that, returning one reiterates */
      return 1;
    }
    else  // no code, gathering further data
    {
      if (++pstate->currenttrans->usecount > RECOMPILE_THRESHOLD)
      {
        nativeblockinfo* nat;
        /* re-enable this as current block */
        pstate->currenttrans = entryataddr;
        pstate->start = addr;
        /* do the funky shit */
        fprintf(stderr, "BLING! Found block, start %.8x, end %.8x\n",
          pstate->start, pstate->start+pstate->currenttrans->length);
        
        nat = recompile_chunk(machine, pstate->start, pstate->start+
          pstate->currenttrans->length);
        
        entryataddr->code = nat->base;
        jt_list_destroy(nat->reloc);
        free(nat);
        fprintf(stderr, "Running code for first time %.8x\n", pstate->start);
        nativesupport_invoke2(machine, entryataddr->code);
        pstate->start = -1;
        return 1;
      }
      assert(pstate->start != -1u);
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
  return 0;
}
