#include <assert.h>
#include <stdio.h>

#include "cnew.h"
#include "machine.h"
#include "block.h"
#include "pseudo.h"
#include "analyse.h"

static void addedge(hashtable* hash, endpoint start, endpoint end)
{
  hashentry* d;
  if (start.parts.address!=-1 && end.parts.address!=-1)
	{
	  d = hash_insert(hash, start.value);
		d->data = (void*) end.value;
	}
}

basicblockinfo* analyse_newbasicblock(uint5 length)
{
  basicblockinfo* basic = cnew(basicblockinfo);
	
	basic->length = length;
	basic->destblks = 0;
	basic->offset = -1;
	basic->patchback = 0;
	
	return basic;
}

void analyse_block(blockinfo* blk)
{
  psblock* psb = blk->psb;
	imapinfo* info;
	hashtable* lastset, *nextused;
	endpoint usetab[32];
	sint5 i, j;
	uint5 blockcount = 1, blockend;
	hashtable* basic;
	hashentry* basicentry;
	hashentry* thisblock = 0, *lastblock = 0;
		
	info = blk->psb->info = cnewarray(imapinfo, psb->length);
	lastset = blk->lastset = hash_new(psb->length);
	nextused = blk->nextused = hash_new(psb->length);

	// initialise data
	for (i=0; i<psb->length; i++)
	{
	  info[i].special.value = 0;
		info[i].rule = 0;
	}

	// partition this code block into basic blocks

  // start of this chunk is always a leader
	info[0].special.flag.leader = 1;

	for (i=0; i<32; usetab[i++].value=-1);

	// find leaders, make block traversable backwards, find last-set info
	for (i=0; i<psb->length; i++)
	{
	  pseudoformat inst;
		endpoint s;
		inst.value = psb->base[i];

    if (info[i].special.flag.leader)
		{
		  uint5 j;
			for (j=0; j<32; usetab[j++].value=-1);
		}
		
		if (inst.parts.rm != reg_UNUSED)
		{
		  s.parts.address = i;
			s.parts.section = sec_RM;

			addedge(lastset, s, usetab[inst.parts.rm]);
		}
		
		if (inst.parts.rn != reg_UNUSED)
		{
		  s.parts.address = i;
			s.parts.section = sec_RN;
			
			addedge(lastset, s, usetab[inst.parts.rn]);
		}
		
		if (inst.parts.ro != reg_UNUSED)
		{
		  s.parts.address = i;
			s.parts.section = sec_RO;
			
			addedge(lastset, s, usetab[inst.parts.ro]);
		}

    if (inst.parts.rd != reg_UNUSED)
		{
		  usetab[inst.parts.rd].parts.address = i;
			usetab[inst.parts.rd].parts.section = sec_RD;
		}
		
    if (inst.parts.opcode>=op_BEQ && inst.parts.opcode<=op_BAL)
		{
		  // destination of a branch is a leader
			if (!info[psb->base[i+1]].special.flag.leader) blockcount++;
		  info[psb->base[i+1]].special.flag.leader = 1;
			if (inst.parts.opcode != op_BAL)
			{
			  // if jump is conditional, instruction following branch is a leader too
				if (!info[i+2].special.flag.leader) blockcount++;
				info[i+2].special.flag.leader = 1;
//				info[i+2].special.flag.follows = 1;
			}
		}

		if (inst.parts.next)
		{
		  if (i+2 < psb->length) info[i+2].special.flag.prev = 1;
			i++;
		}
	}

	basic = psb->basic = hash_new(blockcount);  // more arbitrariness
	blockend = psb->length;

	for (i=0; i<32; usetab[i++].value=-1);

	// calculate next-use information
	for (i=psb->length-1; i>=0; i--)
	{
	  pseudoformat inst;
		endpoint e;
		hashentry* d;
		inst.value = psb->base[i];

		if (inst.parts.rd != reg_UNUSED)
		{
		  e.parts.address = i;
			e.parts.section = sec_RD;

      addedge(nextused, e, usetab[inst.parts.rd]);
			
		  usetab[inst.parts.rd].value = -1;
		}

		if (inst.parts.rm != reg_UNUSED)
		{
			e.parts.address = i;
			e.parts.section = sec_RM;
			
			addedge(nextused, e, usetab[inst.parts.rm]);
			
		  usetab[inst.parts.rm] = e;
		}
		
		if (inst.parts.rn != reg_UNUSED)
		{
		  e.parts.address = i;
			e.parts.section = sec_RN;

      addedge(nextused, e, usetab[inst.parts.rn]);
			
			usetab[inst.parts.rn] = e;
		}

		if (inst.parts.ro != reg_UNUSED)
		{
		  e.parts.address = i;
			e.parts.section = sec_RO;

      addedge(nextused, e, usetab[inst.parts.ro]);
			
			usetab[inst.parts.ro] = e;
		}
		
    // beginning of a new block: clear last-use array
		if (info[i].special.flag.leader)
		{
		  uint5 j;
		  for (j=0; j<32; usetab[j++].value=-1);
			basicentry = hash_insert(basic, i);
			basicentry->data = analyse_newbasicblock(blockend-i);
			blockend = i;
		}

		if (info[i].special.flag.prev) i--;
	}
	
  thisblock = 0;
  
	// trace flow-of-control between basic blocks
	for (i=0; i<psb->length; i++)
	{
	  pseudoformat inst;
	  list* newdest;
		uint5 j;

		inst.value = psb->base[i];
		
    #ifdef DEBUG3
		for (j=0; j<7; j++)
		{
		  endpoint s;
			hashentry* entry;
			const static char* secname[] = {"rd", "rm", "rn", "ro", "cflag", "vflag",
			                                "nflag", "zflag"};
			s.parts.address = i;
			s.parts.section = j;
			if (entry = hash_lookup(blk->nextused, s.value))
			{
			  endpoint e;
				e.value = (uint5) entry->data;
			  fprintf(stderr, "Thing %s:%x next used at %s:%x\n", 
				        secname[s.parts.section], s.parts.address,
								secname[e.parts.section], e.parts.address);
			}
			if (entry = hash_lookup(blk->lastset, s.value))
			{
			  endpoint e;
				e.value = (uint5) entry->data;
				fprintf(stderr, "Thing %s:%x last set at %s:%x\n",
				        secname[s.parts.section], s.parts.address,
								secname[e.parts.section], e.parts.address);
			}
		}
    #endif

//		fprintf(stderr, "Seeking leader at %d\n", i);
		
	  if (info[i].special.flag.leader)
    {
      lastblock = thisblock;
      thisblock = hash_lookup(basic, i);
      if (lastblock)
      {
        newdest = list_add(&((basicblockinfo*)lastblock->data)->destblks);
        newdest->data = (void*)i;
      }
    }
 
		if (inst.parts.opcode>=op_BEQ && inst.parts.opcode<=op_BAL)
		{
      uint5 dest = psb->base[i+1];

      // add to list of follow-on blocks for this block
      newdest = list_add(&((basicblockinfo*)thisblock->data)->destblks);
			newdest->data = (void*)dest;
    }

		if (inst.parts.next) i++;
	}
}
