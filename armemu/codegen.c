#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>

#include "cnew.h"
#include "defs.h"
#include "machine.h"
#include "codegen.h"
#include "allocate.h"
#include "hash.h"
#include "pseudo.h"
#include "registers.h"
#include "x86asm.h"
#include "block.h"
#include "analyse.h"
#include "codegen_p.h"
#include "pqueue.h"

/*  no longer used
x86node* codegen_newnode(void)
{
	x86node* n = cnew(x86node);
	n->branch = 0;
//	n->rule = 0;
	return n;
}*/

// merge an entry into instruction pattern tree
void codegen_merge(hashtable* ht, gen_fn rule, ...)
{
  va_list ap;
	regclass c;
  hashentry* entry;

	va_start(ap, rule);

	while (c=va_arg(ap, uint5))
	{
		entry = hash_insert(ht, c);
		if (!entry->data) entry->data = hash_new(3);
    ht = (hashtable*) entry->data;
	}

	// stick the code in
	entry = hash_insert(ht, CODEGEN);
	entry->data = rule;

	va_end(ap);
}

// return either previously-assigned or new placeholder for a register
static uint5 queryreg(matchstate* ms, uint5 reg)
{
  const static uint5 rtypemap[] = {REGX, REGY, REGZ, REGT, REGU, REGV};
  uint5 i;

	for (i=0; i<ms->num; i++)
    if (ms->regset[i].parts.original==reg) return rtypemap[i];

	ms->regset[ms->num].parts.original = reg;

  return rtypemap[ms->num++];
}

// don't use this function
static void killreg(matchstate* ms, uint5 reg)
{
  uint5 i;
  
  for (i=0; i<ms->num; i++)
    if (ms->regset[i].parts.original==reg)
      ms->regset[i].parts.original = reg_UNUSED;
}

// try to follow a branch in pattern tree; returns 1 if successful
static uint5 follow(hashtable** patfsm, uint5 key)
{
  hashentry* e = *patfsm ? hash_lookup(*patfsm, key) : 0;
  #ifdef DEBUG2
	static const char* typestring[] = {"done", "ignore", "imm", "regx", "regy",
	                                   "regz", "regt", "regu", "regv", "codegen"};
	extern const char* opname[];
//	x86node* node = e->data;

  fprintf(stderr, "%x: ", *patfsm);

  if (key & (1U<<16))
	  fprintf(stderr, "%s following op '%s'\n", e ? "successfully" :
		                "unsuccessfully", opname[key&65535U]);
	else
  	fprintf(stderr, "%s following '%s'\n", e ? "successfully" :
		        "unsuccessfully", typestring[key]);
  #endif
	if (!e) return 0;

	*patfsm = (hashtable*)e->data;
	return 1;
}

// see if an instruction matches, and if it does, set the code generation rule
// to the one from the pattern tree
uint5 codegen_rulify(hashtable* root, hashtable* itree, blockinfo* blk, 
                     gencontext* ctxt, uint5 srcptr)
{
	hashentry* thing;
	gen_fn rulefn=0;
	
	if (!(blk->psb->info)[srcptr].rule)
	{
  	if (codegen_match(root, itree, blk, ctxt, srcptr, &rulefn, 0))
		{
  		// fill in rule code
			if (!rulefn) return 0;
    	blk->psb->info[srcptr].rule = rulefn;
			blk->psb->info[srcptr].mstate = ctxt->mstate;
//			fprintf(stderr, "Overwrite=%x\n", ctxt->mstate.overwrite);
		  return 1;
		}
	}
	else
	{
    #ifdef DEBUG4
	  fprintf(stderr, "Already a rule at %x\n", srcptr);
    #endif
	  return 1;
	}
	return 0;
}

// match an instruction, possibly assimilating previous instructions
uint5 codegen_match(hashtable* root, hashtable* itree, blockinfo* blk, 
                    gencontext* ctxt, uint5 srcptr, gen_fn* rulefn,
                    hashtable** writeback)
{
  pseudoformat* inst = (pseudoformat*)&blk->psb->base[srcptr];
	endpoint edgekey, last, next;
	hashentry* edge, *elast, *enext;
	hashentry* r;
  #ifdef DEBUG4
	const static char* secname[] = {"rd", "rm", "rn", "ro", "cflag", "vflag",
  			                          "nflag", "zflag"};
		
  fprintf(stderr, "Matching instruction at %x\n", srcptr);
  #endif
	
	// see if <instruction> matches
	if (!follow(&itree, OP|inst->parts.opcode)) return 0;

	// see if <rd> matches in this context
	if (inst->parts.rd != reg_UNUSED)
	{
	  if (!follow(&itree, queryreg(&ctxt->mstate, inst->parts.rd)))
		  return 0;
		/*if (inst->parts.rd != inst->parts.rm &&
		    inst->parts.rd != inst->parts.rn &&
				inst->parts.rd != inst->parts.ro) */
    ctxt->mstate.overwrite |= 1;
	}

  // preprocessor abuse follows:
	
#define CHECKBACK(S,N,B) \
  edgekey.parts.address = srcptr; \
	edgekey.parts.section = (S); \
	elast = hash_lookup(blk->lastset, edgekey.value); \
	last.value = elast ? (uint5) elast->data : -1; \
	enext = hash_lookup(blk->nextused, edgekey.value); \
	next.value = enext ? (uint5) enext->data : -1; \
	if (elast && codegen_match(root, itree, blk, ctxt, last.parts.address, \
      rulefn, &itree) && (!enext || (enext && codegen_rulify(root, root, blk, \
      ctxt, next.parts.address)))) \
	{ \
	  EATEN \
	  blk->psb->info[last.parts.address].rule = 0; \
		ctxt->mstate.hidden |= (B); \
	} \
	else \
	{ \
	  if (inst->parts.##N != reg_UNUSED && !follow(&itree, \
		  queryreg(&ctxt->mstate, inst->parts.##N))) return 0; \
	}

  #ifdef DEBUG4
  #define EATEN fprintf(stderr, "Assimilating instruction at %x\n", \
                last.parts.address);
  #else
  #define EATEN
  #endif

	CHECKBACK(sec_RM, rm, (1<<(ctxt->mstate.num-1)));
	CHECKBACK(sec_RN, rn, (1<<(ctxt->mstate.num-1)));
	CHECKBACK(sec_RO, ro, (1<<(ctxt->mstate.num-1)));

#undef CHECKBACK

  // if the 'next' thing is set, try to match an immediate
  if (inst->parts.next)
	{
	  // set immediate value in context
	  ctxt->mstate.imm = blk->psb->base[srcptr+1];
	  if (!follow(&itree, IMM)) return 0;
	}

  // now they've all matched successfully...
	r = hash_lookup(itree, CODEGEN);
  #ifdef DEBUG4
  fprintf(stderr, "%s rule at %x\n", r ? "Setting" : "Not setting", srcptr);
  #endif

	if (rulefn && !*rulefn) *rulefn = r ? r->data : 0;
  if (writeback) *writeback = itree;
  return 1;
}

// recompile a set of basic blocks
void codegen_recompile(machineinfo* machine, blockinfo* blk)
{
  allocationinfo* alloc = cnew(allocationinfo);
	uint5 i;
  pqueue* pq = pqueue_new();
  pqueueitem* pqi;
  psblock* psb = blk->psb;
  basicblockinfo* bb = hash_lookup(psb->basic, 0)->data;

	// All ARM registers in memory
	for (i=0; i<16; i++) alloc->vloc[i].all = var_MEM;
	// ...except temporaries
	for (i=16; i<32; i++) alloc->vloc[i].all = 0;
	// All x86 registers contain nothing
	for (i=0; i<NUMIREG; i++) alloc->rctn[i].all = 0;

  alloc->spill = 0;
	
  blk->native = x86asm_newnative();
  codegen_matchblock(machine, blk);
  
  // push start block into priority queue
  pqi = pqueue_insert(&pq, 0);
  pqi->item = bb;
  // allocated here
  bb->startalloc = *alloc;
  psb->info[0].special.flag.allocated = psb->info[0].special.flag.complete = 1;

  while (bb)
  {
    list* w;
    uint5 line;
    if (!pq->length) break;
    pqi = pqueue_extract(pq);  // get lowest-addressed thingy
    line = pqi->priority;
    bb = pqi->item;
    #ifdef DEBUG4
    fprintf(stderr, "Generating from %x to %x\n", line, line+bb->length);
    #endif
    // if 'falling into' a new basic block which has already got an allocation,
    // massage current state into the new one.
    if (psb->info[line].special.flag.allocated)
    {
      #ifdef DEBUG
//      allocate_checkalloc(alloc);
      #endif
      allocate_coerce(alloc, &bb->startalloc, blk->native);
      #ifdef DEBUG
      fprintf(stderr, "Resetting alloc at line %x\n", line);
      #endif
      *alloc = bb->startalloc;
      #ifdef DEBUG
//      allocate_checkalloc(alloc);
      #endif
    }

//  allocate_checkalloc(alloc);

    // Spill all flags to memory (if they're in registers)
    allocate_preflags(alloc, ALLFLAGS, 0, blk->native);

    // not pretty?
    for (i=reg_CPSRC; i<=reg_CPSRN; i++)
      alloc->vloc[i].all = var_MEM;

    codegen_inner(machine, blk, line, bb, alloc);
//    psb->info[line].special.flag.complete = 1;
    pqueue_deleteitem(pqi);

//    allocate_checkalloc(alloc);

    // add on all destinations
    for (w=bb->destblks; w; w=w->prev)
    {
      uint5 line = (uint5)w->data;
      if (!psb->info[line].special.flag.complete)
      {
        bb = hash_lookup(psb->basic, line)->data;
        bb->startalloc = *alloc;
        // copy this allocation state to each destination
        psb->info[line].special.flag.allocated = 1;
        pqi = pqueue_insert(&pq, line);
        psb->info[line].special.flag.complete = 1;
        #ifdef DEBUG
        fprintf(stderr, "Inserting line %d\n", line);
        #endif
        pqi->item = bb;
      }
    }
  }
  pqueue_delete(pq);
  free(alloc);
}

void codegen_matchblock(machineinfo* machine, blockinfo* blk)
{
  uint5 i;
	gencontext ctxt;

	for (i=0; i<blk->psb->length; i++)
	{
	  pseudoformat inst;
		inst.value = blk->psb->base[i];
    ctxt.mstate.num = ctxt.mstate.overwrite = ctxt.mstate.hidden = 0;
    #ifdef DEBUG4
		fprintf(stderr, "Rulifying %x\n", i);
    #endif
	  if (!codegen_rulify(machine->translaterules, machine->translaterules, blk, 
                        &ctxt, i))
		{
		  fprintf(stderr, "Failed to match anything at %x!\n", i);
			exit(1);
		}
		if (inst.parts.next) i++;
	}
}

void codegen_inner(machineinfo* machine, blockinfo* blk, uint5 start,
                   basicblockinfo* basic, allocationinfo* alloc)
{
  uint5 i;
	psblock* psb = blk->psb;
	hashentry* bas; /* = hash_lookup(psb->basic, start);
	
	assert(bas);
	basic = bas->data;*/
	
	for (i=start; i<start+basic->length; i++)
	{
    basicblockinfo* lastbasic = basic;
	  pseudoformat inst;
		inst.value = psb->base[i];

    codegen_addmapping(psb->basic, i, blk->native->base,
                       blk->native->length);
		
		if (blk->psb->info[i].rule)
	  {
		  uint5 j;
			uint5 lock = ALWAYSLOCK;
			matchstate* mstate = &blk->psb->info[i].mstate;
      uint5 flags = 0;

      #ifdef DEBUG
			fprintf(stderr, "Setting up for instruction at %x\n", i);
      #endif
/*
      allocate_registerpattern(alloc, mstate, &lock, blk->native);

      // muhahaha
      if (inst.parts.rm>=reg_CPSRC && inst.parts.rm<=reg_CPSRN)
        flags |= (1<<(inst.parts.rm-reg_CPSRC));
      if (inst.parts.rn>=reg_CPSRC && inst.parts.rn<=reg_CPSRN)
        flags |= (1<<(inst.parts.rn-reg_CPSRC));
      if (inst.parts.ro>=reg_CPSRC && inst.parts.ro<=reg_CPSRN)
        flags |= (1<<(inst.parts.ro-reg_CPSRC));

      allocate_ensureflags(alloc, mstate, flags, lock, blk->native);
*/
      #ifdef DEBUG5
      fprintf(stderr, "alloc->vloc[15].all=%x\n", alloc->vloc[15].all);
      #endif

      allocate_setup(alloc, mstate, inst.value, blk->native, &lock);

			// generate code for this pattern
		  blk->psb->info[i].rule(machine, blk->native, blk->psb->basic,
        blk->psb->info, alloc, mstate, lock, inst.parts.flags);

 //     x86asm_out(blk->native, NOP, 0);

      allocate_npostfix(alloc, mstate);
//      allocate_checkalloc(alloc);
		}

		if (inst.parts.next) i++;
	}
}

// following two functions similar in spirit & implementation to the pseudo_ 
// ones which do exactly the same thing

// return the offset of an Intel instruction corresponding to pseudocode offset
sint5 codegen_getinteladdr(hashtable* basicmap, uint5 psinst, uint5 patch)
{
  hashentry* h = hash_lookup(basicmap, psinst);
  basicblockinfo* bl = h ? (basicblockinfo*) h->data : 0;
  
  assert(h);

  if (bl->offset != -1)
    return bl->offset - (patch+4);
  
  #ifdef DEBUG4
  fprintf(stderr, "Adding x86 patch request at %x, waiting for %x\n", patch,
          psinst);
  #endif
  list_add(&bl->patchback);
  bl->patchback->data = (void*)patch;
  
  return -(patch+4);
}

void codegen_addmapping(hashtable* basicmap, uint5 psinst, uint3* base,
                        sint5 ioffset)
{
  hashentry* h = hash_lookup(basicmap, psinst);
  basicblockinfo* bl = h ? (basicblockinfo*) h->data : 0;
   
  if (!h) return;
  
  #ifdef DEBUG
  fprintf(stderr, "Setting mapping pseudo:%x to intel:%x\n", psinst, ioffset);
  #endif
  
  bl->offset = ioffset;

  #ifdef DEBUG
  if (bl->patchback) fprintf(stderr, "Patching back x86\n");
  #endif
  
  while (bl->patchback)
  {
    uint5* addr = (uint5*)((uint5)base + (uint5)bl->patchback->data);
    #ifdef DEBUG
    fprintf(stderr, "Poking %x\n", ioffset);
    #endif
    (*addr) += ioffset;
    list_removehead(&bl->patchback);
  }
}
