#include <stdio.h>
#include <stdlib.h>

#include "dynarec/recompile.h"
#include "dynarec/pheta.h"
#include "dynarec/phetadism.h"
#include "core/machine.h"
#include "dynarec/palloc.h"
#include "dynarec/arch/x86/genx86.h"
#include "dynarec/arch/x86/x86asm.h"
#include "libjtype/cnew.h"

nativeblockinfo* recompile_chunk(machineinfo* machine, uint5 start, uint5 end)
{
  pheta_chunk* mychunk;
  nativeblockinfo* nat;

  mychunk = pheta_translatechunk(machine, start, (end-start)/4);
/*  phetadism_chunk(mychunk);*/
  fprintf(stderr, "Initialising\n");
  palloc_init(mychunk);
  fprintf(stderr, "Constant allocating\n");
  palloc_constant(mychunk);
/*  fprintf(stderr, "Outputting gdl prior to branch optimisation\n");
  pheta_dotprint(mychunk, "controlgraph.dot");*/
  fprintf(stderr, "Transitive branch optimising\n");
  pheta_optimise_transitive_branch(mychunk);
  pheta_cull_unused_nodes(mychunk);
//  palloc_nonorthog(mychunk);
//  palloc_fetchmem(mychunk);
  fprintf(stderr, "Using dfs to get parents\n");
  pheta_dfs(mychunk);
/*  fprintf(stderr, "Commit shuffling\n");
  palloc_shufflecommit(mychunk);*/
  fprintf(stderr, "Getting predecessors\n");
  pheta_predecessor(mychunk);
  fprintf(stderr, "Finding strongly-connected components\n");
  pheta_scc(mychunk);
  fprintf(stderr, "Fixing up flags & predicates\n");
  pheta_fixup_flags(mychunk);
  phetadism_chunk(mychunk);
  fprintf(stderr, "Outputting dotty graph\n");
/*  pheta_davinciprint(mychunk, "controlgraph.daVinci");*/
  pheta_dotprint(mychunk, "controlgraph.dot");
  fprintf(stderr, "Finding live ranges\n");
  palloc_clearmarkers(mychunk);
  palloc_findspans(mychunk, mychunk->root, 0);
  fprintf(stderr, "Source-destination aliasing\n");
  palloc_srcdestalias(mychunk);
  fprintf(stderr, "Freeing previous spans\n");
  palloc_deletespans(mychunk);
  fprintf(stderr, "Re-running span finder\n");
  palloc_clearmarkers(mychunk);
  palloc_findspans(mychunk, mychunk->root, 0);
  palloc_printspans(mychunk);
  fprintf(stderr, "Doing linear scan allocation\n");
/*  fprintf(stderr, "Allocation state:\n");
  palloc_print(mychunk);*/

  mychunk->parentmachine = machine;
  palloc_linearscan(mychunk, machine->mem);
  jt_pqueue_delete(mychunk->active);
  fprintf(stderr, "Completing allocation\n");
  genx86_complete_alloc(mychunk);
  fprintf(stderr, "Allocation state:\n");
  palloc_print(mychunk);
  fprintf(stderr, "Inserting register load/store code\n");
  genx86_insert_spill_code(mychunk);
  fprintf(stderr, "Allocation state:\n");
  palloc_print(mychunk);
  fprintf(stderr, "Flattening code\n");
  nat = genx86_flatten_code(mychunk);
  
  jt_delete(mychunk);
  
/*  nativesupport_invoke2(machine, nat);*/

  return nat;
}
