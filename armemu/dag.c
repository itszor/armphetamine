#include "cnew.h"
#include "machine.h"
#include "block.h"
//#include "dag.h"
#include "pseudodism.h"

static const uint3 nbit[] =
{
  0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  4, 5, 5, 4, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

// returns number of set bits in a word
// not actually used at the moment, but I feel it should be here ;-)
uint5 setbits(uint5 x)
{
  return nbit[x&0xff] + nbit[(x>>8)&0xff]
	     + nbit[(x>>16)&0xff] + nbit[(x>>24)&0xff];
}

// Build vector containing free-variable and instruction dependency
// from a pseudocode block
dagnode* dag_build(psblock* psb)
{
//  sint5 lastuse[32];
	dagnode* dag = cnewarray(dagnode, psb->length);
	uint5 i, live = 0;
	
	// unset register last-use
//	for (i=0; i<32; i++) lastuse[i] = -1;
	// clear instruction flags
	for (i=0; i<psb->length; dag[i++].flags.value = 0);
	
	// zeroth pass: initialise array
/*	for (i=0; i<psb->length; i++)
	  dag[i].ref = dag[i].rm = dag[i].rn = dag[i].ro = 0;
	
	dag[0].ref++;
	
	// first pass: determine start and end of basic blocks within this block
	for (i=0; i<psb->length; i++)
	{
	  pseudoformat inst;
		inst.value = psb->base[i];		

		if (inst.parts.opcode>=op_BEQ && inst.parts.opcode<=op_BAL)
		{
	//	  dag[i].tag |= tag_BLOCKEND;
			dag[i+2].ref++;  // immediately following
			dag[psb->base[i+1]].ref++;  // jump location
		}
		
//	if (inst.parts.opcode>=op_SWI && inst.parts.opcode<=op_END)
//	{
//	  dag[i].tag |= tag_BLOCKEND;
//	}

		if (inst.parts.next) i++;
	}*/
	
	// second pass: live variable analysis & referencing
	for (i=0; i<psb->length; i++)
	{
	  pseudoformat inst;
		inst.value = psb->base[i];

		if (inst.parts.rm != reg_UNUSED)
		{
		  live |= 1<<inst.parts.rm;
//			dag[i].rm = lastuse[inst.parts.rm];
		}

		if (inst.parts.rn != reg_UNUSED)
		{
		  live |= 1<<inst.parts.rn;
//			dag[i].rn = lastuse[inst.parts.rn];
		}

		if (inst.parts.ro != reg_UNUSED)
		{
		  live |= 1<<inst.parts.ro;
//			dag[i].ro = lastuse[inst.parts.ro];
		}

		if (inst.parts.rd != reg_UNUSED)
		{
		  live &= ~(1<<inst.parts.rd);
//			lastuse[inst.parts.rd] = i;
		}
		
		if (inst.parts.flags & CFLAG)
		{
		  live &= ~bit_CPSRC;
//			lastuse[reg_CPSRC] = i;
		}
		
		if (inst.parts.flags & VFLAG)
		{
		  live &= ~bit_CPSRV;
//			lastuse[reg_CPSRV] = i;
		}
		
		if (inst.parts.flags & ZFLAG)
		{
		  live &= ~bit_CPSRZ;
//			lastuse[reg_CPSRZ] = i;
		}
		
		if (inst.parts.flags & NFLAG)
		{
		  live &= ~bit_CPSRN;
//			lastuse[reg_CPSRN] = i;
		}

		dag[i].live = live;

		if (inst.parts.opcode>=op_BEQ && inst.parts.opcode<=op_BAL)
		{
		  dag[psb->base[i+1]].flags |= flag_BLOCKSTART;
		}

//		printf("Liveness at %x: %x (%d set bits)", i, live, setbits(live));
 /*		if (dag[i].ref)
  		printf(" referenced %d times\n", dag[i].ref);
		else*/
//		printf("\n");

		if (inst.parts.next)
		{
		  i++;
			dag[i].flags |= flag_PREV;
		}
	}
	
	dag_backpass(psb, dag);
	
	return dag;
}

// make another pass over the extra info array, annotating with next-use
// information (needed by later parts of the code generator, may also be
// useful for optimisation). Also kill unnecessary flag modification,
// and remove any instructions whose results are subsequently overwritten.
void dag_backpass(psblock* psb, dagnode* dag)
{
  sint5 nextuse[32];
  sint5 i, nextblock;

//  printf("Scanning intermediate code backwards\n");

	for (i=0; i<32; i++) nextuse[i] = -1;
	
	for (i=psb->length-1; i>=0; i--)
	{
	  pseudoformat inst;
		inst.value = psb->base[i];

	  if (dag[i].flags & flag_PREV) i--;

		if (inst.parts.rd != reg_UNUSED)
		{
		  dag[i].rd = nextuse[inst.parts.rd];
			nextuse[inst.parts.rd] = -1;
		}

	  if (inst.parts.rm != reg_UNUSED)
		{
		  dag[i].rm = nextuse[inst.parts.rm];
			nextuse[inst.parts.rm] = i;
		}

		if (inst.parts.rn != reg_UNUSED)
		{
		  dag[i].rn = nextuse[inst.parts.rn];
			nextuse[inst.parts.rn] = i;
		}

		if (inst.parts.ro != reg_UNUSED)
		{
		  dag[i].ro = nextuse[inst.parts.ro];
			nextuse[inst.parts.ro] = i;
		}

		if (dag[i].flags & flag_BLOCKSTART) nextblock = i;
	}

  // print results of this pass
	for (i=0; i<psb->length; i++)
	{
	  pseudoformat inst;
		inst.value = psb->base[i];
		
		printf("%x: ", i);
		if (inst.parts.rd != reg_UNUSED)
		{
		  if (dag[i].rd==-1)
				printf("%s(rd):last ", regname[inst.parts.rd]);
			else
		  	printf("%s(rd):%x ", regname[inst.parts.rd], dag[i].rd);
		}
		if (inst.parts.rm != reg_UNUSED)
		{
		  if (dag[i].rm==-1)
				printf("%s:last ", regname[inst.parts.rm]);
			else
		  	printf("%s:%x ", regname[inst.parts.rm], dag[i].rm);
		}
		if (inst.parts.rn != reg_UNUSED)
		{
		  if (dag[i].rn==-1)
				printf("%s:last ", regname[inst.parts.rn]);
			else
		  	printf("%s:%x ", regname[inst.parts.rn], dag[i].rn);
		}
		if (inst.parts.ro != reg_UNUSED)
		{
		  if (dag[i].ro==-1)
				printf("%s:last ", regname[inst.parts.ro]);
			else
		  	printf("%s:%x ", regname[inst.parts.ro], dag[i].ro);
		}
		printf("\n");

		if (inst.parts.next) i++;
	}
}
