#include <stdio.h>
#include <assert.h>

#include "libjtype/defs.h"
#include "core/machine.h"
#include "libjtype/cnew.h"
#include "dynarec/block.h"
#include "core/decode.h"
#include "core/execute.h"
#include "dynarec/arch/x86/x86dism.h"
#include "dynarec/arch/x86/nativesupport.h"

blockinfo* block_new(void)
{
  blockinfo* block = jt_new(blockinfo);
	
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
//  jt_delete(block->info);
  jt_hash_nuke(block->lastset, 0);
  jt_hash_nuke(block->nextused, 0);
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
	if (block->native) jt_delete(block->native);
	jt_delete(block);
}


