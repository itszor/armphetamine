#include "defs.h"
#include "flush.h"
#include "block.h"
#include "hash.h"

static void flush_releaseblock(void* data)
{
  hashentry* he = data;
  blockinfo* blk = he->data;
  free(blk->native);
  free(blk);
}

void flush_all(machineinfo* machine)
{
#ifdef RECOMPILE
  hash_nuke(machine->blocks, flush_releaseblock);
  machine->live = 0;
  // oops, we just nuked this
  machine->blocks = hash_new(17001);  // pick a number...
#endif
}
