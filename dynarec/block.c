#include <stdio.h>
#include <assert.h>

#include "defs.h"
#include "machine.h"
#include "cnew.h"
#include "block.h"
#include "pseudo.h"
#include "decode.h"
#include "execute.h"
#include "analyse.h"
#include "codegen.h"
#include "x86dism.h"
#include "nativesupport.h"

blockinfo* block_new(void)
{
  blockinfo* block = cnew(blockinfo);
	
	block->length = block->usecount = 1;
	block->psb = 0;
//	block->info = 0;
	block->lastset = 0;
	block->nextused = 0;
	block->native = 0;
	
	return block;
}

// Attempt to minimise space requirements for a recompiled block
void block_clean(blockinfo* block)
{
  pseudo_deleteblock(block->psb);
//  free(block->info);
  hash_nuke(block->lastset, 0);
  hash_nuke(block->nextused, 0);
  block->psb = 0;
//  block->info = 0;
  block->lastset = block->nextused = 0;
  block->native->base = realloc(block->native->base, block->native->length);
  block->native->size = block->native->length;
}

// Don't call this, it looks broken
void block_delete(blockinfo* block)
{
  if (block->psb) pseudo_deleteblock(block->psb);
	if (block->native) free(block->native);
	free(block);
}

void block_dotranslation(machineinfo* machine, blockinfo* thisblock, uint5 pc)
{
  uint5 i;
  #ifdef DEBUG
	fprintf(stderr, "Code block executed more than %d times; translating "
				          "to intermediate code...\n", EXECTHRESHOLD);
  #endif
  assert(machine->live = hash_lookup(machine->blocks, pc));
	pseudo_translateblock(machine, thisblock, pc);
//free(dag_build(thisblock->psb));
  #ifdef DEBUG
  fprintf(stderr, "Analysing block...\n");
  pseudodism_block(thisblock->psb);
  #endif
	analyse_block(thisblock);
  #ifdef DEBUG
	fprintf(stderr, "Attempting recompilation...\n");
  #endif
	codegen_recompile(machine, thisblock);
  #ifdef DEBUG
	fprintf(stderr, "Generated intermediate code:\n");
	pseudodism_block(thisblock->psb);
  #endif
  block_clean(thisblock);
  #ifdef DEBUG
	x86dism_block(thisblock->native);
  for (i=0; i<16; i++)
    fprintf(stderr, "r%d=%x ", i, machine->reg->r[i]);
  fprintf(stderr, "\n");

  fprintf(stderr, "Running native code at %x for first time\n",
    thisblock->native->base);
  #endif
  #ifdef DEBUG
  for (i=0; i<16; i++)
    fprintf(stderr, "r%d=%x ", i, machine->reg->r[i]);
  fprintf(stderr, "\n");
  #endif
}

// extend current block or begin new block
uint5 block_gather(machineinfo* machine, uint5 pc)
{
  blockinfo* thisblock = machine->live ? machine->live->data : 0;
  uint5 start, end, newblock = 0;

  if (!thisblock)
  {
    hashentry* h = hash_lookup(machine->blocks, pc);
    
    if (h)  // been here before...
    {
      thisblock = h->data;

      if (thisblock->native)
      {
        nativesupport_invoke(machine, thisblock->native);
        machine->live = 0;
        return 1;
      }
      else
      {
        if (++thisblock->usecount > EXECTHRESHOLD)
		    {
          block_dotranslation(machine, thisblock, pc);
          nativesupport_invoke(machine, thisblock->native);
          machine->live = 0;
          return 1;
		    }
        machine->live = h;
        thisblock = machine->live->data;
      }
    }
    else
    {
		  h = hash_insert(machine->blocks, pc);
		  h->data = block_new();
  	  machine->live = h;
      thisblock = machine->live->data;
      return 0;
    }
  }

  start = machine->live->key;
  end = start+thisblock->length*sizeof(uint5);

  if (pc==end)
  {
    thisblock->length++;
  }
  else if (pc<start || pc>end)
  {
		machine->live = 0;
    return block_gather(machine, pc);  // eeeew!
  }

  return 0;
}

/*
void block_findrules(machineinfo* machine, blockinfo* blk)
{
  uint5 i, asgnset[6], aptr;
	gencontext c;
	
	c.machine = machine;
	c.srcbase = blk->psb->base;
	c.info = blk->psb->dag;
	c.lastblockstart = 0;
	for (i=0; i<32; i++) c.rloc[i] = in_REGBLK;
	for (i=0; i<8; i++) r.ireg[i] = reg_UNUSED;
	c.spill = 0;
	
	for (i=0; i<psb->length; i++)
	{
	  if (c.info[i].flags & flag_BLOCKSTART) c.lastblockstart = i;
	  aptr = 0;
	  codegen_match(machine->translaterules, &c, asgnset, &aptr, i);
		if (psbase[i].parts.next) i++;
	}
}
*/
