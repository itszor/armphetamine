#include <stdarg.h>
#include <stdio.h>

#include "defs.h"
#include "machine.h"
#include "pseudo.h"
#include "cnew.h"
#include "execute.h"
#include "hash.h"
#include "block.h"



psblock* pseudo_newblock(uint5 startsize)
{
  psblock* psb = cnew(psblock);

	psb->base = cnewarray(uint5, 1024);
  psb->info = 0;
	psb->length = 0;
	psb->size = 1024;
	psb->pseudomap = hash_new(9);  // completely arbitrary now...
	psb->basic = 0;

	return psb;
}

// and now for the memory leaks...
void pseudo_deleteblock(psblock* psb)
{
  free(psb->base);
  free(psb->info);
	hash_nuke(psb->pseudomap, pseudo_psminfodestructor);
  hash_nuke(psb->basic, 0);
  free(psb);
}

psmapinfo* pseudo_newpsminfo(uint5 pseudo)
{
  psmapinfo* psm = cnew(psmapinfo);
	
	psm->pseudo = pseudo;
	psm->patchback = 0;
//	psm->intel = 0;
	
	return psm;
}

void pseudo_psminfodestructor(void* data)
{
  psmapinfo* psm = (psmapinfo*)data;
  while (psm->patchback) list_removehead(&psm->patchback);
  free(psm);
}

void pseudo_deletepsminfo(psmapinfo* psm)
{
  while (psm->patchback) list_removehead(&psm->patchback);
  free(psm);
}

static void extend(psblock* blk)
{
	if (++blk->length == blk->size)
	{
	  blk->size *= 2;
	  blk->base = realloc(blk->base, sizeof(uint5)*blk->size);
    fprintf(stderr, "--Extending block, base now %x--\n", blk->base);
		if (!blk->base)
		{
		  fprintf(stderr, "Block extend failed?\n");
			exit(1);
		}
	}
}

void pseudo_emit(psblock* blk, pseudoopcode op, ...)
{
  va_list ap;
	pseudoformat* psf;
	int someflags = (op & PSSETFLAGS) ? 1 : 0;
  int noflags = (op & NOFLAGS) ? 1 : 0;

	extend(blk);
	psf = (pseudoformat*) &blk->base[blk->length-1];
		
	op &= 0x7f;

	psf->parts.opcode = op;

	va_start(ap, op);
	
	if (someflags)
	  psf->parts.flags = va_arg(ap, uint5);
	else
	  psf->parts.flags = 0;
	
	switch (op)
	{
/*	  case op_Z:     // unops
		case op_N:
		case op_RRXC:*/
		case op_MOV:
		case op_MVN:
		psf->parts.rd = va_arg(ap, uint5);
		psf->parts.rm = va_arg(ap, uint5);
		psf->parts.rn = reg_UNUSED;
		psf->parts.ro = reg_UNUSED;
		psf->parts.next = 0;
		break;

		case op_RRX:
		psf->parts.rd = va_arg(ap, uint5);
		psf->parts.rm = va_arg(ap, uint5);
		psf->parts.rn = reg_CPSRC;
		psf->parts.ro = reg_UNUSED;
		psf->parts.next = 0;
    break;

		case op_LDW:
		case op_LDB:
		psf->parts.rd = va_arg(ap, uint5);
		psf->parts.rm = va_arg(ap, uint5);
		psf->parts.rn = reg_UNUSED;
		psf->parts.ro = reg_UNUSED;
		psf->parts.next = 0;
		break;

		case op_STW:
		case op_STB:
    psf->parts.rd = reg_UNUSED;
		psf->parts.rm = va_arg(ap, uint5);
		psf->parts.rn = va_arg(ap, uint5);
		psf->parts.ro = reg_UNUSED;
		psf->parts.next = 0;
		break;
		
		case op_BEQ:  // Z set
		case op_BNE:  // Z clear
		psf->parts.rd = reg_UNUSED;
		psf->parts.rm = noflags ? reg_UNUSED : reg_CPSRZ;
		psf->parts.rn = reg_UNUSED;
		psf->parts.ro = reg_UNUSED;
		psf->parts.next = 1;
		extend(blk);
		blk->base[blk->length-1] = va_arg(ap, uint5);
		break;
		
		case op_BCS:  // C set
		case op_BCC:  // C clear
		psf->parts.rd = reg_UNUSED;
		psf->parts.rm = noflags ? reg_UNUSED : reg_CPSRC;
		psf->parts.rn = reg_UNUSED;
		psf->parts.ro = reg_UNUSED;
		psf->parts.next = 1;
		extend(blk);
		blk->base[blk->length-1] = va_arg(ap, uint5);
		break;

		case op_BMI:  // N set
		case op_BPL:  // N clear
		psf->parts.rd = reg_UNUSED;
		psf->parts.rm = noflags ? reg_UNUSED : reg_CPSRN;
		psf->parts.rn = reg_UNUSED;
		psf->parts.ro = reg_UNUSED;
		psf->parts.next = 1;
		extend(blk);
		blk->base[blk->length-1] = va_arg(ap, uint5);
		break;
		
		case op_BVS:  // V set
		case op_BVC:  // V clear
		psf->parts.rd = reg_UNUSED;
		psf->parts.rm = noflags ? reg_UNUSED : reg_CPSRV;
		psf->parts.rn = reg_UNUSED;
		psf->parts.ro = reg_UNUSED;
		psf->parts.next = 1;
		extend(blk);
		blk->base[blk->length-1] = va_arg(ap, uint5);
		break;

		case op_BHI:  // C set and Z clear
		case op_BLS:  // C clear or Z set
		psf->parts.rd = reg_UNUSED;
		psf->parts.rm = noflags ? reg_UNUSED : reg_CPSRC;
		psf->parts.rn = noflags ? reg_UNUSED : reg_CPSRZ;
		psf->parts.ro = reg_UNUSED;
		psf->parts.next = 1;
		extend(blk);
		blk->base[blk->length-1] = va_arg(ap, uint5);
		break;
				
		case op_BGE:  // N set and V set, or N clear and V clear
		case op_BLT:  // N set and V clear, or N clear and V set
		psf->parts.rd = reg_UNUSED;
		psf->parts.rm = noflags ? reg_UNUSED : reg_CPSRN;
		psf->parts.rn = noflags ? reg_UNUSED : reg_CPSRV;
		psf->parts.ro = reg_UNUSED;
		psf->parts.next = 1;
		extend(blk);
		blk->base[blk->length-1] = va_arg(ap, uint5);
		break;
		
		case op_BGT:  // Z clear, and either N set and V set, or N clear and V clear
		case op_BLE:  // Z set, or N set and V clear, or N clear and V set
		psf->parts.rd = reg_UNUSED;
		psf->parts.rm = noflags ? reg_UNUSED : reg_CPSRN;
		psf->parts.rn = noflags ? reg_UNUSED : reg_CPSRV;
		psf->parts.ro = noflags ? reg_UNUSED : reg_CPSRZ;
		psf->parts.next = 1;
		extend(blk);
		blk->base[blk->length-1] = va_arg(ap, uint5);
		break;

		case op_BAL:
		psf->parts.rd = reg_UNUSED;
		psf->parts.rm = reg_UNUSED;
		psf->parts.rn = reg_UNUSED;
		psf->parts.ro = reg_UNUSED;
		psf->parts.next = 1;
		extend(blk);
		blk->base[blk->length-1] = va_arg(ap, uint5);
		break;

		case op_CONST:
		case op_SWI:    // immediate follows
		psf->parts.rd = (op==op_CONST) ? va_arg(ap, uint5) : reg_UNUSED;
		psf->parts.rm = reg_UNUSED;
		psf->parts.rn = reg_UNUSED;
		psf->parts.ro = reg_UNUSED;
		psf->parts.next = 1;
		extend(blk);
		blk->base[blk->length-1] = va_arg(ap, uint5);
		break;

		case op_ADC:
		case op_SBC:
		psf->parts.rd = va_arg(ap, uint5);
		psf->parts.rm = va_arg(ap, uint5);
		psf->parts.rn = va_arg(ap, uint5);
		psf->parts.ro = va_arg(ap, uint5);
		psf->parts.next = 0;
		break;
		
		case op_CMP:
    case op_CMN:
    case op_TEQ:
    case op_TST:
		psf->parts.rd = reg_UNUSED;
		psf->parts.rm = va_arg(ap, uint5);
		psf->parts.rn = va_arg(ap, uint5);
		psf->parts.ro = reg_UNUSED;
		psf->parts.next = 0;
		break;
		
		case op_SETPC:
		psf->parts.rd = reg_R15;
		psf->parts.rm = reg_UNUSED;
		psf->parts.rn = reg_UNUSED;
		psf->parts.ro = reg_UNUSED;
		psf->parts.next = 0;
    break;
    
    case op_RECONSTRUCTPC:
    psf->parts.rd = reg_R15;
    psf->parts.rm = reg_CPSRFLAGS;
    psf->parts.rn = reg_CPSRMODE;
    psf->parts.ro = reg_CPSRINTMASK;
    psf->parts.next = 1;
		extend(blk);
		blk->base[blk->length-1] = va_arg(ap, uint5);
    break;

		case op_XJMP:
		psf->parts.rd = reg_UNUSED;
		psf->parts.rm = reg_UNUSED;
		psf->parts.rn = reg_UNUSED;
		psf->parts.ro = reg_UNUSED;
		psf->parts.next = 1;
		extend(blk);
		blk->base[blk->length-1] = va_arg(ap, uint5);
		break;
    
		case op_END:
		psf->parts.rd = reg_UNUSED;
		psf->parts.rm = reg_UNUSED;
		psf->parts.rn = reg_UNUSED;
		psf->parts.ro = reg_UNUSED;
		psf->parts.next = 0;
		break;

		default:  // binop (all the rest)
		psf->parts.rd = va_arg(ap, uint5);
		psf->parts.rm = va_arg(ap, uint5);
		psf->parts.rn = va_arg(ap, uint5);
		psf->parts.ro = reg_UNUSED;
		psf->parts.next = 0;
		break;
	}
	va_end(ap);
}

// generates intermediate code from a block of ARM code
void pseudo_translateblock(machineinfo* machine, blockinfo* block, uint5 pc)
{
  registerinfo* reg = machine->reg;
  uint5 i, oldr15 = RGET(15);

  #ifdef DEBUG
	fprintf(stderr, "Translating code at %x, length %d\n", pc, block->length);
  #endif

	if (block->psb) pseudo_deleteblock(block->psb);

  block->psb = pseudo_newblock(block->length);  // may not be optimal size

	for (i=0; i<block->length; i++)
	{
	  instructionformat inst;
		uint5* instaddr = (uint5*)(pc+i*sizeof(uint5));
		uint5* flataddr = memory_lookup(machine->mem, instaddr);
		RPUT(15, pc+i*sizeof(uint5)+8);
		inst.instruction = *flataddr;
    #ifdef DEBUG
		fprintf(stderr, "Translating %x: ", instaddr);
		dispatch(machine, inst, &diss, 0);
    fprintf(stderr, "\n");
    #endif
		pseudo_addmapping(block->psb->pseudomap, instaddr,
		                  block->psb->base, block->psb->length);
	  dispatch(machine, inst, &pseudo, (void*)block->psb);
	}
	
	pseudo_addmapping(block->psb->pseudomap, (uint5*)(pc+i*sizeof(uint5)),
	                  block->psb->base, block->psb->length);
  // this is a far better place for this
  pseudo_emit(block->psb, op_CONST, reg_R15, pc+block->length*sizeof(uint5)+8);
	pseudo_emit(block->psb, op_END);

	RPUT(15, oldr15);
}

// return the address of a pseudocode instruction corresponding to the ARM
// instruction at 'arminst', or add pseudocode address to patchback list
uint5 pseudo_getpsaddr(hashtable* pseudomap, uint5* arminst, sint5 patch)
{
  hashentry* h = hash_lookup(pseudomap, (uint5) arminst);
	psmapinfo* ps = h ? (psmapinfo*) h->data : 0;

	if (ps && ps->pseudo!=-1) return ps->pseudo;

  if (ps) fprintf(stderr, "ps present, but unset?\n");

//  fprintf(stderr, "Erm: %x\n", patch);

  // no hashtable entry for this ARM instruction (ie, forward reference)
	if (!h)
	{
		h = hash_insert(pseudomap, (uint5) arminst);
		ps = h->data = pseudo_newpsminfo(-1);
	}

  patch++;
  #ifdef DEBUG
  fprintf(stderr, "Adding patch request at %x for inst %x\n", patch, arminst);
  #endif
	list_add(&ps->patchback);
	ps->patchback->data = (void*)patch;
	
//  fprintf(stderr, "Returning -1 at %x\n", patch);
  
	return -1;
}

void pseudo_addmapping(hashtable* pseudomap, uint5* arminst, uint5* base,
                       sint5 pseudo)
{
  hashentry* h = hash_lookup(pseudomap, (uint5) arminst);
	psmapinfo* ps = h ? (psmapinfo*) h->data : 0;
 	
//	fprintf(stderr, "Adding mapping %x == %x\n", arminst, pseudo);
	
	if (!h)
	{
	  h = hash_insert(pseudomap, (uint5) arminst);
		ps = h->data = pseudo_newpsminfo(pseudo);
	}
  
  if (ps && ps->pseudo==-1) ps->pseudo = pseudo;
	
  #ifdef DEBUG
	if (ps->patchback) fprintf(stderr, "Patching addresses back\n");
  #endif
	
	while (ps->patchback)
	{
	  uint5 addr = (uint5) ps->patchback->data;
		base[addr] = pseudo;
//    fprintf(stderr, "Writing %x into address %x\n", pseudo, &base[addr]);
		list_removehead(&ps->patchback);
	}
}

// return the next temporary register slot (dumb)
sint5 pseudo_newtemp(sint5* prevtemp)
{
  if (*prevtemp==-1) *prevtemp = reg_T0; else (*prevtemp)++;
	if (*prevtemp>reg_T3)
	{
	  fprintf(stderr, "Out of static temporaries!\n");
		exit(1);
	}
	return *prevtemp;
}

void pseudo_condition(machineinfo* machine, uint5 cond, psblock* psb)
{
	registerinfo* reg = machine->reg;
	if (cond != cc_AL)
	{
  	// (condition ^ 1) is the opposite condition to (condition)
		pseudo_jmp(machine, cond ^ 1, (uint5*)(GET(15)-4), psb);
/*		pseudo_emit(blk, op_BEQ + (cond ^ 1),
	  	pseudo_getpsaddr(psb->pseudomap, (uint5*)(GET(15)-4),
			psb->length));*/
	}
}

void pseudo_reconstitutepc(machineinfo* machine, uint5 sflag,
                           psblock* psb, sint5* tc, sint5 offset)
{
  registerinfo* reg = machine->reg;
  if (sflag)
    pseudo_emit(psb, op_RECONSTRUCTPC, GET(15)+offset);
  else
    pseudo_emit(psb, op_CONST, reg_R15, GET(15)+offset);
}

// generate different types of jump depending on locality of destination
void pseudo_jmp(machineinfo* machine, uint5 cond, uint5* dest, psblock* psb)
{
  blockinfo* arm = machine->live->data;
	uint5* base = (uint5*) machine->live->key;

  if (cond == cc_NV) return;

  #ifdef DEBUG
  fprintf(stderr, "base=%x end=%x dest=%x\n", base, &base[arm->length], dest);
  #endif

  if (dest>=base && dest<=&base[arm->length])
	{
		// within this block
		pseudo_emit(psb, op_BEQ + cond, pseudo_getpsaddr(psb->pseudomap, dest,
			        	psb->length));
  }
	else
	{
	  hashentry* h = hash_lookup(machine->blocks, (uint5)dest);
    blockinfo* toblk = h ? h->data : 0;
		
		pseudo_condition(machine, cond, psb);

		if (toblk && toblk->native)  // jump to the start of another basic block
		{
			pseudo_emit(psb, op_XJMP, toblk);
		}
		else
		{
		  // don't know any basic block at that address, fall back to emulator
			pseudo_emit(psb, op_CONST, reg_R15, dest);
			pseudo_emit(psb, op_SETPC);
		}
	}
}

int pseudo_dp(machineinfo* machine, instructionformat inst, void* blk)
{
  uint5 temp = inst.dp.operand2;
	uint5 shifttype = (temp>>5)&3, amount;
  uint3 regshift = (temp & 16) ? 1 : 0;
  // mask for logic rather than arithmetic instructions
	const uint5 logic = 0xf303;
  const uint5 affectrd = 0xf0ff;
	int islogic = logic & (1<<inst.dp.opcode);
	sint5 tc = -1;
	sint5 shiftbyreg = -1;
	uint5 op2reg, rm = temp&15;
  uint5 t, s;

	pseudo_condition(machine, inst.generic.cond, blk);

  if (inst.dp.rn==reg_R15 || rm==reg_R15)
    pseudo_reconstitutepc(machine, inst.dp.s, blk, &tc, regshift ? 4 : 0);

  if (regshift)  // shift by register
	{
	  shiftbyreg = temp>>8;
    
    if (shifttype!=3)
    {
      t = pseudo_newtemp(&tc);
      pseudo_emit(blk, op_CONST, t, 0xff);
      pseudo_emit(blk, op_AND, t, shiftbyreg, t);
    }
    
    switch (shifttype)
    {
      case 0:  // lsl / no shift
      {
        s = pseudo_newtemp(&tc);
        pseudo_emit(blk, op_CONST, s, 31);
//        pseudo_emit(blk, op_CMP | PSSETFLAGS, ALLFLAGS, t, s);
        pseudo_emit(blk, op_CMP, t, s);
        pseudo_emit(blk, op_BGT|NOFLAGS, ((psblock*)blk)->length+5);
			  op2reg = s;
				if (islogic && inst.dp.s)
				  pseudo_emit(blk, op_LSL | PSSETFLAGS, CFLAG, op2reg, rm, t);
				else
          pseudo_emit(blk, op_LSL, op2reg, rm, t);
        pseudo_emit(blk, op_BAL|NOFLAGS, ((psblock*)blk)->length+4);
        pseudo_emit(blk, op_CONST, op2reg, 0);
      }
      break;
      
      case 1:  // logical right
      {
        s = pseudo_newtemp(&tc);
        pseudo_emit(blk, op_CONST, s, 31);
//        pseudo_emit(blk, op_CMP | PSSETFLAGS, ALLFLAGS, t, s);
        pseudo_emit(blk, op_CMP, t, s);
        pseudo_emit(blk, op_BGT|NOFLAGS, ((psblock*)blk)->length+5);
			  op2reg = s;
				if (islogic && inst.dp.s)
				  pseudo_emit(blk, op_LSR | PSSETFLAGS, CFLAG, op2reg, rm, t);
				else
          pseudo_emit(blk, op_LSR, op2reg, rm, t);
        pseudo_emit(blk, op_BAL|NOFLAGS, ((psblock*)blk)->length+4);
        pseudo_emit(blk, op_CONST, op2reg, 0);
      }
      break;
      
      case 2:  // arithmetic right
      {
        s = pseudo_newtemp(&tc);
        pseudo_emit(blk, op_CONST, s, 31);
//        pseudo_emit(blk, op_CMP | PSSETFLAGS, ALLFLAGS, t, s);
        pseudo_emit(blk, op_CMP, t, s);
        pseudo_emit(blk, op_BGT|NOFLAGS, ((psblock*)blk)->length+5);
			  op2reg = s;
				if (islogic && inst.dp.s)
				  pseudo_emit(blk, op_ASR | PSSETFLAGS, CFLAG, op2reg, rm, t);
				else
          pseudo_emit(blk, op_ASR, op2reg, rm, t);
        pseudo_emit(blk, op_BAL|NOFLAGS, ((psblock*)blk)->length+13);
        pseudo_emit(blk, op_CONST, t, TOPBIT);
        pseudo_emit(blk, op_TST/* | PSSETFLAGS, NFLAG|ZFLAG*/, rm, t);
        pseudo_emit(blk, op_BEQ|NOFLAGS, ((psblock*)blk)->length+6);
        pseudo_emit(blk, op_CONST, op2reg, 0xffffffff);
        pseudo_emit(blk, op_BAL|NOFLAGS, ((psblock*)blk)->length+4);
        pseudo_emit(blk, op_CONST, op2reg, 0);
      }
      break;
      
      case 3:  // rotate right
      {
		  	op2reg = pseudo_newtemp(&tc);
				if (islogic && inst.dp.s)
				  pseudo_emit(blk, op_ROR | PSSETFLAGS, reg_CPSRC, op2reg, rm,
                      shiftbyreg);
				else
  				pseudo_emit(blk, op_ROR, op2reg, rm, shiftbyreg);
      }
      break;
    }
	}
	else  // shift by immediate
	{
	  amount = temp>>7;
		if (shifttype!=0 || amount!=0)
		{
      if (!(shifttype==3 && amount==0))
      {
  		  shiftbyreg = pseudo_newtemp(&tc);
  			pseudo_emit(blk, op_CONST, shiftbyreg, amount);
      }
      op2reg = pseudo_newtemp(&tc);
		}
    else
      op2reg = rm;

    switch (shifttype)
    {
      case 0:  // logical left/no shift
      {
        if (amount)
        {
				  if (islogic && inst.dp.s)
				    pseudo_emit(blk, op_LSL | PSSETFLAGS, CFLAG, op2reg, rm,
              shiftbyreg);
				  else
            pseudo_emit(blk, op_LSL, op2reg, rm, shiftbyreg);
        }
      }
      break;

      case 1:  // logical right
      {
        if (amount)  // shift by 1..31
        {
				  if (islogic && inst.dp.s)
				    pseudo_emit(blk, op_LSR | PSSETFLAGS, CFLAG, op2reg, rm, 
              shiftbyreg);
				  else
            pseudo_emit(blk, op_LSR, op2reg, rm, shiftbyreg);
        }
        else  // shift by 32 (represented by 0)
        {
          if (islogic && inst.dp.s)
          {
            pseudo_emit(blk, op_CONST, op2reg, TOPBIT);
//            pseudo_emit(blk, op_TST | PSSETFLAGS, NFLAG|ZFLAG, rm, op2reg);
            pseudo_emit(blk, op_TST, rm, op2reg);
            pseudo_emit(blk, op_BEQ|NOFLAGS, ((psblock*)blk)->length+6);
            pseudo_emit(blk, op_CONST | PSSETFLAGS, CFLAG, reg_CPSRC, 1);
            pseudo_emit(blk, op_BAL|NOFLAGS, ((psblock*)blk)->length+4);
            pseudo_emit(blk, op_CONST | PSSETFLAGS, CFLAG, reg_CPSRC, 0);
            pseudo_emit(blk, op_CONST, op2reg, 0);
          }
          else
          {
            pseudo_emit(blk, op_CONST, op2reg, 0);
          }
        }
      }
      break;

      case 2:  // arithmetic right
      {
        if (amount)
        {
				  if (islogic && inst.dp.s)
				    pseudo_emit(blk, op_ASR | PSSETFLAGS, CFLAG, op2reg, rm, 
              shiftbyreg);
				  else
            pseudo_emit(blk, op_ASR, op2reg, rm, shiftbyreg);
        }
        else
        {
          if (islogic && inst.dp.s)
          {
            pseudo_emit(blk, op_CONST, op2reg, TOPBIT);
//            pseudo_emit(blk, op_TST | PSSETFLAGS, NFLAG|ZFLAG, rm, op2reg);
            pseudo_emit(blk, op_TST, rm, op2reg);
            pseudo_emit(blk, op_BEQ|NOFLAGS, ((psblock*)blk)->length+8);
            pseudo_emit(blk, op_CONST | PSSETFLAGS, CFLAG, reg_CPSRC, 1);
            pseudo_emit(blk, op_CONST, op2reg, 0xffffffff);
            pseudo_emit(blk, op_BAL|NOFLAGS, ((psblock*)blk)->length+6);
            pseudo_emit(blk, op_CONST | PSSETFLAGS, CFLAG, reg_CPSRC, 0);
            pseudo_emit(blk, op_CONST, op2reg, 0);
          }
          else
          {
            pseudo_emit(blk, op_CONST, op2reg, TOPBIT);
//            pseudo_emit(blk, op_TST | PSSETFLAGS, NFLAG|ZFLAG, rm, op2reg);
            pseudo_emit(blk, op_TST, rm, op2reg);
            pseudo_emit(blk, op_BEQ|NOFLAGS, ((psblock*)blk)->length+6);
            pseudo_emit(blk, op_CONST, op2reg, 0xffffffff);
            pseudo_emit(blk, op_BAL|NOFLAGS, ((psblock*)blk)->length+4);
            pseudo_emit(blk, op_CONST, op2reg, 0);
          }
        }
      }
      break;
      
      case 3:  // rotate right / rrx
      {
        if (amount)  // ror
        {
				  if (islogic && inst.dp.s)
				    pseudo_emit(blk, op_ROR | PSSETFLAGS, reg_CPSRC, op2reg, rm,
                        shiftbyreg);
				  else
  				  pseudo_emit(blk, op_ROR, op2reg, rm, shiftbyreg);
        }
        else  // rrx
        {
				  if (islogic && inst.dp.s)
				    pseudo_emit(blk, op_RRX | PSSETFLAGS, CFLAG, op2reg, rm);
				  else
	  			  pseudo_emit(blk, op_RRX, op2reg, rm);
        }
      }
      break;
    }
	}
	/*
	switch (shifttype)
	{
		case 0:  // LSL / no shift
		{
		  if (shiftbyreg != -1)
			{
			  op2reg = pseudo_newtemp(&tc);
				if (islogic && inst.dp.s)
				  pseudo_emit(blk, op_LSL | PSSETFLAGS, CFLAG, op2reg, rm, shiftbyreg);
				else
          pseudo_emit(blk, op_LSL, op2reg, rm, shiftbyreg);
			}
			else
			{
			  op2reg = rm;
			}
		}
		break;
		
		case 1:  // LSR
		{
		  op2reg = pseudo_newtemp(&tc);
			if (islogic && inst.dp.s)
			  pseudo_emit(blk, op_LSR | PSSETFLAGS, CFLAG, op2reg, rm, shiftbyreg);
			else
		  	pseudo_emit(blk, op_LSR, op2reg, rm, shiftbyreg);
		}
		break;
		
		case 2:  // ASR
		{
		  op2reg = pseudo_newtemp(&tc);
			if (islogic && inst.dp.s)
			  pseudo_emit(blk, op_ASR | PSSETFLAGS, CFLAG, op2reg, rm, shiftbyreg);
			else
		  	pseudo_emit(blk, op_ASR, op2reg, rm, shiftbyreg);
		}
		break;
		
		case 3:  // ROR/RRX
		{
		  if (amount==0)  // immediate/amount==0 means RRX
			{
			  op2reg = pseudo_newtemp(&tc);
				if (islogic && inst.dp.s)
				  pseudo_emit(blk, op_RRX | PSSETFLAGS, CFLAG, op2reg, rm);
				else
	  			pseudo_emit(blk, op_RRX, op2reg, rm);
			}
			else
			{
		  	op2reg = pseudo_newtemp(&tc);
				if (islogic && inst.dp.s)
				  pseudo_emit(blk, op_ROR | PSSETFLAGS, reg_CPSRC, op2reg, rm,
                      shiftbyreg);
				else
  				pseudo_emit(blk, op_ROR, op2reg, rm, shiftbyreg);
			}
		}
	}*/
	
	// at this point, we should have op1 in inst.dp.rn and op2 in op2reg
	pseudo_dp_guts(machine, inst, blk, op2reg, &tc);
  return 0;
}

int pseudo_dp_imm(machineinfo* machine, instructionformat inst, void* blk)
{
  sint5 tc = -1;
	uint5 imm = pseudo_newtemp(&tc), temp = inst.dp.operand2;
  uint5 value = temp&255, amount = (temp>>8)*2;
	const uint5 logic = 0xf303;
	int islogic = logic & (1<<inst.dp.opcode);

	pseudo_condition(machine, inst.generic.cond, blk);

  if (inst.dp.rn==reg_R15)
    pseudo_reconstitutepc(machine, inst.dp.s, blk, &tc, 0);

	pseudo_emit(blk, op_CONST, imm, ROR(value, amount));
  
  if (inst.dp.s && islogic && amount)
    pseudo_emit(blk, op_CONST | PSSETFLAGS, CFLAG, reg_CPSRC,
      (value & (1U<<(amount-1))) ? 1 : 0);
	
	pseudo_dp_guts(machine, inst, blk, imm, &tc);
  return 0;
}

void pseudo_dp_guts(machineinfo* machine, instructionformat inst, void* blk,
                    uint5 op2reg, sint5* tc)
{
  uint3 sflagset = inst.dp.s;

  if (inst.dp.rd==reg_R15) inst.dp.s = 0;

	switch (inst.dp.opcode)
	{
	  case dp_AND:
		{
		  if (inst.dp.s)
  		  pseudo_emit(blk, op_AND | PSSETFLAGS, ZFLAG | NFLAG,
				            inst.dp.rd, inst.dp.rn, op2reg);
			else
  		  pseudo_emit(blk, op_AND, inst.dp.rd, inst.dp.rn, op2reg);
		}
		break;
		
		case dp_EOR:
		{
		  if (inst.dp.s)
  		  pseudo_emit(blk, op_EOR | PSSETFLAGS, ZFLAG | NFLAG,
				            inst.dp.rd, inst.dp.rn, op2reg);
			else
  		  pseudo_emit(blk, op_EOR, inst.dp.rd, inst.dp.rn, op2reg);
		}
		break;
		
		case dp_SUB:
		{
			if (inst.dp.s)
			  pseudo_emit(blk, op_SUB | PSSETFLAGS, ALLFLAGS,
				            inst.dp.rd, inst.dp.rn, op2reg);
		  else
		    pseudo_emit(blk, op_SUB, inst.dp.rd, inst.dp.rn, op2reg);
		}
		break;
		
		case dp_RSB:
		{
		  if (inst.dp.s)
  		  pseudo_emit(blk, op_SUB | PSSETFLAGS, ALLFLAGS,
				            inst.dp.rd, op2reg, inst.dp.rn);
			else
  		  pseudo_emit(blk, op_SUB, inst.dp.rd, op2reg, inst.dp.rn);
		}
		break;
		
		case dp_ADD:
		{
		  if (inst.dp.s)
  		  pseudo_emit(blk, op_ADD | PSSETFLAGS, ALLFLAGS,
				            inst.dp.rd, inst.dp.rn, op2reg);
			else
  		  pseudo_emit(blk, op_ADD, inst.dp.rd, inst.dp.rn, op2reg);
		}
		break;
		
		case dp_ADC:
		{
		  if (inst.dp.s)
  		  pseudo_emit(blk, op_ADC | PSSETFLAGS, ALLFLAGS,
				            inst.dp.rd, inst.dp.rn, op2reg, reg_CPSRC);
			else
  		  pseudo_emit(blk, op_ADC, inst.dp.rd, inst.dp.rn, op2reg, reg_CPSRC);
		}
		break;
		
		case dp_SBC:
		{
		  if (inst.dp.s)
  		  pseudo_emit(blk, op_SBC | PSSETFLAGS, ALLFLAGS,
				            inst.dp.rd, inst.dp.rn, op2reg, reg_CPSRC);
			else
  		  pseudo_emit(blk, op_SBC, inst.dp.rd, inst.dp.rn, op2reg, reg_CPSRC);
		}
		break;
		
		case dp_RSC:
		{
		  if (inst.dp.s)
				pseudo_emit(blk, op_SBC | PSSETFLAGS, ALLFLAGS,
			            	inst.dp.rd, op2reg, inst.dp.rn, reg_CPSRC);
			else
				pseudo_emit(blk, op_SBC, inst.dp.rd, op2reg, inst.dp.rn, reg_CPSRC);
		}
		break;
		
		case dp_TST:
		{
		  if (inst.dp.s)
  			pseudo_emit(blk, op_TST | PSSETFLAGS, ZFLAG | NFLAG,
				            inst.dp.rn, op2reg);
			else
        fprintf(stderr, "Warning: TST with clear S flag!\n");
		}
		break;
		
		case dp_TEQ:
		{
		  if (inst.dp.s)
  			pseudo_emit(blk, op_TEQ | PSSETFLAGS, ZFLAG | NFLAG,
				            inst.dp.rn, op2reg);
			else
        fprintf(stderr, "Warning: TEQ with clear S flag!\n");
		}
		break;
		
		case dp_CMP:
		{
		  if (inst.dp.s)
			  pseudo_emit(blk, op_CMP | PSSETFLAGS, ALLFLAGS,
				            inst.dp.rn, op2reg);
			else
        fprintf(stderr, "Warning: CMP with clear S flag!\n");
		}
		break;
		
		case dp_CMN:
		{
		  if (inst.dp.s)
			  pseudo_emit(blk, op_CMN | PSSETFLAGS, ALLFLAGS,
				            inst.dp.rn, op2reg);
			else
			  fprintf(stderr, "Warning: CMN with clear S flag!\n");
		}
		break;
		
		case dp_ORR:
		{
		  if (inst.dp.s)
		  	pseudo_emit(blk, op_OR | PSSETFLAGS, ZFLAG | NFLAG,
				            inst.dp.rd, inst.dp.rn, op2reg);
			else
		  	pseudo_emit(blk, op_OR, inst.dp.rd, inst.dp.rn, op2reg);
		}
		break;
		
		case dp_MOV:
		{
		  if (inst.dp.s)
  		  pseudo_emit(blk, op_MOV | PSSETFLAGS, ZFLAG | NFLAG, inst.dp.rd, 
                    op2reg);
			else
  		  pseudo_emit(blk, op_MOV, inst.dp.rd, op2reg);
		}
		break;
		
		case dp_BIC:
		{
		  uint5 inv = pseudo_newtemp(tc);
		  pseudo_emit(blk, op_MVN, inv, op2reg);
			if (inst.dp.s)
				pseudo_emit(blk, op_AND | PSSETFLAGS, ZFLAG | NFLAG,
				            inst.dp.rd, inst.dp.rn, inv);
			else
				pseudo_emit(blk, op_AND, inst.dp.rd, inst.dp.rn, inv);
		}
		break;
		
		case dp_MVN:
		{
		  if (inst.dp.s)
		  	pseudo_emit(blk, op_MVN | PSSETFLAGS, ZFLAG | NFLAG, inst.dp.rd,
                    op2reg);
			else
		  	pseudo_emit(blk, op_MVN, inst.dp.rd, op2reg);
		}
		break;
	}
	
/*
	if (inst.dp.s)
	{
	  pseudo_emit(blk, op_Z, reg_CPSRZ, inst.dp.rd);
		pseudo_emit(blk, op_N, reg_CPSRN, inst.dp.rd);
	}
*/
	
	if (inst.dp.rd==reg_R15)
	{
    if (sflagset)
	    pseudo_emit(blk, op_SETPC | PSSETFLAGS, ALLFLAGS);
    else
	    pseudo_emit(blk, op_SETPC);
	}
}

int pseudo_bra(machineinfo* machine, instructionformat inst, void* blk)
{
	sint5 offset = (sint5)(inst.bra.offset<<8)>>6, tc=-1;
	psblock* psb = (psblock*) blk;
	registerinfo* reg = machine->reg;

/*	offset += 8;*/
	
	if (inst.bra.l)
	{
    uint5 four = pseudo_newtemp(&tc);
    pseudo_condition(machine, inst.generic.cond, blk);
    pseudo_reconstitutepc(machine, 1, blk, &tc, 0);
    pseudo_emit(blk, op_CONST, four, 4);
		pseudo_emit(blk, op_SUB, reg_R14, reg_R15, four);
		pseudo_jmp(machine, cc_AL, (uint5*)(GET(15)+offset/*-8*/), psb);
/*		pseudo_emit(blk, op_BAL,
		  pseudo_getpsaddr(psb->pseudomap, (uint5*)(GET(15)+offset-8),
		  psb->length));*/
	}
	else
	{
	  if (inst.generic.cond != cc_NV)
		{
/*      pseudo_emit(blk, op_BEQ + inst.generic.cond,
			  pseudo_getpsaddr(psb->pseudomap, (uint5*)(GET(15)+offset-8),
				psb->length));*/
      pseudo_jmp(machine, inst.generic.cond, (uint5*)(GET(15)+offset/*-8*/), 
                 psb);
		}
	}
  return 0;
}

int pseudo_mul(machineinfo* machine, instructionformat inst, void* blk)
{
  sint5 tc=-1;
	uint5 temp = inst.mul.rd;
	pseudo_condition(machine, inst.generic.cond, blk);

  if (inst.mul.a && inst.mul.rd==inst.mul.rn) temp = pseudo_newtemp(&tc);
	
	if (inst.mul.s && !inst.mul.a)
  	pseudo_emit(blk, op_MUL | PSSETFLAGS, ZFLAG | NFLAG,
		            temp, inst.mul.rm, inst.mul.rs);
	else
  	pseudo_emit(blk, op_MUL, temp, inst.mul.rm, inst.mul.rs);

  if (inst.mul.a)
	{
	  if (inst.mul.s)
  	  pseudo_emit(blk, op_ADD | PSSETFLAGS, ZFLAG | NFLAG,
			            inst.mul.rd, temp, inst.mul.rn);
		else
  	  pseudo_emit(blk, op_ADD, inst.mul.rd, temp, inst.mul.rn);
	}

/*  if (inst.mul.s)
	{
	  pseudo_emit(blk, op_Z, reg_CPSRZ, inst.mul.rd);
		pseudo_emit(blk, op_N, reg_CPSRN, inst.mul.rd);
	}*/
  return 0;
}

int pseudo_sdt(machineinfo* machine, instructionformat inst, void* blk)
{
  sint5 tc = -1, offset = -1, addr;
	uint5 setpc = 0;

  pseudo_condition(machine, inst.generic.cond, blk);

 /* pseudo_emit(blk, op_CONST, reg_T5, 0x0);
  pseudo_emit(blk, op_CONST, reg_T4, machine->reg->r[15]);
  pseudo_emit(blk, op_STW, reg_T5, reg_T4);

  pseudo_emit(blk, op_CONST, reg_T5, 0x4);
  pseudo_emit(blk, op_STW, reg_T5, reg_R5);*/
	  
	addr = pseudo_newtemp(&tc);
	
	if (inst.sdt.i)  // register(/shifted register) offset
	{
	  uint5 temp = inst.sdt.offset;
		uint5 rm = temp&15, shifttype = (temp>>5)&3, amount = (temp>>7);

    if (rm==15 || inst.sdt.rn==15)
      pseudo_reconstitutepc(machine, 0, blk, &tc, 0);

		offset = pseudo_newtemp(&tc);
		
		switch (shifttype)
		{
		  case 0:  // LSL / no shift
			{
			  if (amount>0)
				{
				  uint5 amt = pseudo_newtemp(&tc);
					pseudo_emit(blk, op_CONST, amt, amount);
				  pseudo_emit(blk, op_LSL, offset, rm, amt);
				}
				else
				{
				  offset = rm;
				}
			}
			break;
			
			case 1:  // LSR
			{
			  uint5 amt = pseudo_newtemp(&tc);
			  if (amount==0) amount=32;
				pseudo_emit(blk, op_CONST, amt, amount);
				pseudo_emit(blk, op_LSR, offset, rm, amt);
			}
			break;
			
			case 2:  // ASR
			{
			  uint5 amt = pseudo_newtemp(&tc);
				if (amount==0) amount=32;
				pseudo_emit(blk, op_CONST, amt, amount);
				pseudo_emit(blk, op_ASR, offset, rm, amt);
			}
			break;
			
			case 3:  // ROR/RRX
			{
			  if (amount)  // ROR
				{
				  uint5 amt = pseudo_newtemp(&tc);
					pseudo_emit(blk, op_CONST, amt, amount);
				  pseudo_emit(blk, op_ROR, offset, rm, amt);
				}
				else  // RRX
				{
				  pseudo_emit(blk, op_RRX, offset, rm);
				}
			}
		}
	}
	else  // immediate offset
	{
    if (inst.sdt.rn==15)
      pseudo_reconstitutepc(machine, 0, blk, &tc, 0);

	  offset = pseudo_newtemp(&tc);
		pseudo_emit(blk, op_CONST, offset, inst.sdt.offset);
	}

	if (inst.sdt.p)  // pre-indexed
	{
    if (inst.sdt.u)
		  pseudo_emit(blk, op_ADD, addr, inst.sdt.rn, offset);
		else
		  pseudo_emit(blk, op_SUB, addr, inst.sdt.rn, offset);
	}
	else  // not pre-indexed
	{
	  pseudo_emit(blk, op_MOV, addr, inst.sdt.rn);
	}

	if (inst.sdt.l)  // load
	{
	  if (inst.sdt.b)  // byte
		{
		  pseudo_emit(blk, op_LDB, inst.sdt.rd, addr);
		}
		else  // word
		{
		  pseudo_emit(blk, op_LDW, inst.sdt.rd, addr);
			if (inst.sdt.rd==reg_R15) setpc = 1;
		}
	}
	else  // store
	{
    if (inst.sdt.rd==15) pseudo_reconstitutepc(machine, 1, blk, &tc, 4);

	  if (inst.sdt.b)  // byte
		{
		  pseudo_emit(blk, op_STB, addr, inst.sdt.rd);
		}
		else  // word
		{
		  pseudo_emit(blk, op_STW, addr, inst.sdt.rd);
		}
	}

	if (!inst.sdt.p)  // post-indexed addressing
	{
	  if (inst.sdt.u)  // up
		{
		  pseudo_emit(blk, op_ADD, inst.sdt.rn, inst.sdt.rn, offset);
		}
		else  // down
		{
		  pseudo_emit(blk, op_SUB, inst.sdt.rn, inst.sdt.rn, offset);
		}
	}

	if (inst.sdt.w)  // writeback
	{
	  pseudo_emit(blk, op_MOV, inst.sdt.rn, addr);
	}

	if (setpc)
	{
	  pseudo_emit(blk, op_SETPC | PSSETFLAGS, ALLFLAGS);
	}
  return 0;
}

int pseudo_bdt(machineinfo* machine, instructionformat inst, void* blk)
{
  sint5 tc = -1, i;
	uint5 addr = pseudo_newtemp(&tc), base = inst.bdt.rn,
	      incr = pseudo_newtemp(&tc), setpc = 0;
	
	pseudo_condition(machine, inst.generic.cond, blk);
	
	pseudo_emit(blk, op_MOV, addr, base);
	pseudo_emit(blk, op_CONST, incr, 4);

  if (inst.bdt.u)  // upwards
  {
    for (i=0; i<16; i++)
    {
	    if (inst.bdt.reglist & (1<<i))
		  {
	  	  if (inst.bdt.p)  // preincrement
			  {
			  	pseudo_emit(blk, op_ADD, addr, addr, incr);
        }

			  if (inst.bdt.l)  // load
			  {
		  	  pseudo_emit(blk, op_LDW, i, addr);
				  if (i==reg_R15) setpc = 1;
			  }
			  else  // save
			  {
          if (i==reg_R15)
            pseudo_reconstitutepc(machine, 1, blk, &tc, 0);

		  	  pseudo_emit(blk, op_STW, addr, i);
			  }

			  if (!inst.bdt.p)  // postincrement
			  {
			  	pseudo_emit(blk, op_ADD, addr, addr, incr);
			  }
      }
    }
  }
  else  // downwards
  {
    for (i=15; i>=0; i--)
    {
	    if (inst.bdt.reglist & (1<<i))
		  {
	  	  if (inst.bdt.p)  // predecrement
			  {
			  	pseudo_emit(blk, op_SUB, addr, addr, incr);
        }

			  if (inst.bdt.l)  // load
			  {
		  	  pseudo_emit(blk, op_LDW, i, addr);
				  if (i==reg_R15) setpc = 1;
			  }
			  else  // save
			  {
          if (i==reg_R15)
            pseudo_reconstitutepc(machine, 1, blk, &tc, 0);

		  	  pseudo_emit(blk, op_STW, addr, i);
			  }

			  if (!inst.bdt.p)  // postdecrement
			  {
			  	pseudo_emit(blk, op_SUB, addr, addr, incr);
			  }
      }
    }
  }
	
	if (inst.bdt.w)  // writeback
	{
	  pseudo_emit(blk, op_MOV, base, addr);
	}
	
	if (setpc)
	{
    if (inst.bdt.s)
      pseudo_emit(blk, op_SETPC | PSSETFLAGS, ALLFLAGS);
    else
	    pseudo_emit(blk, op_SETPC);
	}
  return 0;
}

int pseudo_swi(machineinfo* machine, instructionformat inst, void* blk)
{
  sint5 tc=-1;
  pseudo_condition(machine, inst.generic.cond, blk);

  pseudo_reconstitutepc(machine, 1, blk, &tc, 0);
	pseudo_emit(blk, op_SWI, inst.swi.number);
  return 0;
}

int pseudo_cdt(machineinfo* machine, instructionformat inst, void* blk)
{
  return 0;
}

int pseudo_cdo(machineinfo* machine, instructionformat inst, void* blk)
{
  return 0;
}

int pseudo_crt(machineinfo* machine, instructionformat inst, void* blk)
{
  return 0;
}

int pseudo_sds(machineinfo* machine, instructionformat inst, void* blk)
{
  return 0;
}

int pseudo_und(machineinfo* machine, instructionformat inst, void* blk)
{
  return 0;
}
