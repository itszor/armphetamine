#include <stdio.h>
#include <assert.h>

#include "defs.h"
#include "machine.h"
#include "cnew.h"
#include "block.h"
#include "decode.h"
#include "execute.h"
#include "x86dism.h"
#include "nativesupport.h"

blockinfo* block_new(void)
{
  blockinfo* block = cnew(blockinfo);
	
	block->length = block->usecount = 1;
//	block->psb = 0;
//	block->info = 0;
	block->lastset = 0;
	block->nextused = 0;
	block->native = 0;
	
	return block;
}

// Attempt to minimise space requirements for a recompiled block
void block_clean(blockinfo* block)
{
//  pseudo_deleteblock(block->psb);
//  free(block->info);
  hash_nuke(block->lastset, 0);
  hash_nuke(block->nextused, 0);
//  block->psb = 0;
//  block->info = 0;
  block->lastset = block->nextused = 0;
  block->native->base = realloc(block->native->base, block->native->length);
  block->native->size = block->native->length;
}

// Don't call this, it looks broken
void block_delete(blockinfo* block)
{
//  if (block->psb) pseudo_deleteblock(block->psb);
	if (block->native) free(block->native);
	free(block);
}


