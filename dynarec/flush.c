#include <stdlib.h>

#include "defs.h"
#include "flush.h"
#include "block.h"
#include "hash.h"

static void flush_releaseblock(void* data)
{
  jt_hashentry* he = data;
  blockinfo* blk = he->data;
  jt_delete(blk->native);
  jt_delete(blk);
}

void flush_all(machineinfo* machine)
{
  IGNORE(machine);
#ifdef RECOMPILE2
  jt_hash_nuke(machine->blocks, flush_releaseblock);
  machine->live = 0;
  // oops, we just nuked this
  machine->blocks = jt_hash_new(17001);  // pick a number...
#endif
}
