#include <stdio.h>
#include <sys/syscall.h>
#include <sys/stat.h>

#include "defs.h"
#include "machine.h"
#include "memory.h"
#include "registers.h"
#include "processor.h"
#include "execute.h"
#include "fakesys.h"
#include "decode.h"

int exec_condition(machineinfo* machine, instructionformat inst)
{
	registerinfo* reg = machine->reg;
  // conditional execution on any instruction
  switch (inst.generic.cond)
  {
    case cc_EQ:  // Z set
    if (!FLAG(z)) {
      INCPC;
      return 0;
    }
    break;

    case cc_NE:  // Z clear
    if (FLAG(z)) {
      INCPC;
      return 0;
    }
    break;

    case cc_CS:  // C set (unsigned higher or same)
    if (!FLAG(c)) {
      INCPC;
      return 0;
    }
    break;

    case cc_CC:  // C clear (unsigned lower)
    if (FLAG(c)) {
      INCPC;
      return 0;
    }
    break;

    case cc_MI:  // N set (negative)
    if (!FLAG(n)) {
      INCPC;
      return 0;
    }
    break;

    case cc_PL:  // N clear (positive or zero)
    if (FLAG(n)) {
      INCPC;
      return 0;
    }
    break;

    case cc_VS:  // V set (overflow)
    if (!FLAG(v)) {
      INCPC;
      return 0;
    }
    break;

    case cc_VC:  // V clear (overflow)
    if (FLAG(v)) {
      INCPC;
      return 0;
    }
    break;

    case cc_HI:  // C set and Z clear (unsigned higher)
    if (!FLAG(c) || FLAG(z)) {
      INCPC;
      return 0;
    }
    break;

    case cc_LS:  // C clear or Z set (unsigned lower or same)
    if (FLAG(c) && !FLAG(z)) {
      INCPC;
      return 0;
    }
    break;

    case cc_GE:  // N set and V set, or N clear and V clear (greater or equal)
    if (FLAG(n) ^ FLAG(v)) {
      INCPC;
      return 0;
    }
    break;

    case cc_LT:  // N set and V clear, or N clear and V set (less than)
    if (!(FLAG(n) ^ FLAG(v))) {
      INCPC;
      return 0;
    }
    break;

    case cc_GT:  // Z clear, and either N set and V set, or N clear and V clear (greater than)
    if (FLAG(z) || (FLAG(n) ^ FLAG(v))) {
      INCPC;
      return 0;
    }
    break;

    case cc_LE:  // Z set, or N set and V clear, or N clear and V set (less than or equal)
    if (!FLAG(z) && !(FLAG(n) ^ FLAG(v))) {
      INCPC;
      return 0;
    }
    break;

    case cc_AL:  // always
    break;

    case cc_NV:  // never
    INCPC;
    return 0;
  }
  return 1;
}

#ifdef ASM_ALU

#include "asmalu.h"

void exec_dp(machineinfo* machine, instructionformat inst, void* null)
{
  if (!exec_condition(machine, inst)) return;
  else
  {
    registerinfo* reg = machine->reg;
    uint5 temp = inst.dp.operand2;
    uint5 regshift = (temp & 16) ? 1 : 0;
    uint5 op1 = (inst.dp.rn==15 && regshift) ? GET(inst.dp.rn)+4
                                             : GET(inst.dp.rn);
    uint5 rm = ((temp&15)==15 && regshift) ? GET(temp&15)+4 : GET(temp&15);
    uint5 shifttype = (temp>>5)&3;
    uint5 amount, dest = inst.dp.rd;
    uint5 affectrd = (1<<inst.dp.opcode) & 0xf0ff;
    uint5 res;

    if (regshift) amount = reg->r[temp>>8];
      else amount = temp>>7;
    
    // This is the magic line, all the rest slows it down far too much
    // though...
    res = asmalutab[(inst.dp.opcode<<4) | (inst.dp.s<<3) |
          (shifttype<<1) | regshift](op1, rm, amount, &reg->r[15]);

    if (affectrd)
    {
      STOREREG(dp, res);
      reg->r[15] += (dest==15) ? 8 : 4;
    }
    else
    {
      reg->r[15] += 4;
    }
  }
}

void exec_dp_imm(machineinfo* machine, instructionformat inst, void* null)
{
  if (!exec_condition(machine, inst)) return;
  else
  {
    registerinfo* reg = machine->reg;
    uint5 op1 = GET(inst.dp.rn);
    uint5 temp = inst.dp.operand2;
    uint5 amount = (temp>>8)*2;
    uint5 dest = inst.dp.rd;
    const uint5 affectrd = (1<<inst.dp.opcode) & 0xf0ff;
    uint5 res = asmalutab[(inst.dp.opcode<<4) | (inst.dp.s<<3) |
      (3<<1) | 1](op1, temp&0xff, amount, &reg->r[15]);

    if (affectrd)
    {
      STOREREG(dp, res);
      reg->r[15] += (dest==15) ? 8 : 4;
    }
    else
    {
      reg->r[15] += 4;
    }
  }
}

#else
// execute an ALU instruction with op2 as a shifted register
void exec_dp(machineinfo* machine, instructionformat inst, void* null)
{
  registerinfo* reg = machine->reg;
  uint5 op2 = 0, temp = inst.dp.operand2;
  // for register shift & op1==PC, instruction+12 is used
  uint3 regshift = (temp & 16) ? 1 : 0;
  uint5 op1 = (inst.dp.rn==15 && regshift) ? GET(inst.dp.rn)+4
                                           : GET(inst.dp.rn);
  uint5 rd = 0;
  // mask for logic rather than arithmetic instructions
  const uint5 logic = 0xf303;
  // mask for rd-affecting instructions
  const uint5 affectrd = 0xf0ff;
  // I suppose this should be...
  uint5 rm = ((temp&15)==15 && regshift) ? GET(temp&15)+4 : GET(temp&15);
  uint5 shifttype = (temp>>5)&3, amount, tamount;
  // islogic affects the way the carry flag is set
  int islogic = logic & (1<<inst.dp.opcode);

  if (!exec_condition(machine, inst)) return;

  if (regshift)  // shift by register
  {
    amount = RGET(temp>>8) & 0xff; // only the least significant byte
    
    switch (shifttype)  // register shift operations
    {
      case 0:  // logical left/no shift
      {
        op2 = amount>31 ? 0 : rm<<amount;
        if (islogic && amount && inst.dp.s)
        {
          FLAG(c) = amount>31 ? 0 : rm & (1U<<(32-amount)) ? 1 : 0;
        }
      }
      break;
      
      case 1:  // logical right
      {
        op2 = amount>31 ? 0 : rm>>amount;
        if (islogic && amount && inst.dp.s)
        {
          FLAG(c) = amount>32 ? 0 : rm & (1U<<(amount-1)) ? 1 : 0;
        }
      }
      break;
      
      case 2:  // arithmetic right
      {
        op2 = amount>31 ? (rm&TOPBIT ? 0xffffffff : 0)
                        : (uint5)((sint5)rm>>amount);
        if (islogic && amount && inst.dp.s)
        {
          FLAG(c) = amount>32 ? (rm>>31) : rm & (1U<<(amount-1)) ? 1 : 0;
        }
      }
      break;
      
      case 3:  // rotate right (no rrx for register shift)
      {
        amount &= 31;
        op2 = ROR(rm, amount);
        if (islogic && inst.dp.s)
        {
          if (!amount) amount=32;
          FLAG(c) = (rm & (1U<<(amount-1))) ? 1 : 0;
        }
      }
    }
  }
  else   // shift by immediate
  {
    tamount = amount = temp>>7;
    
    switch (shifttype)
    {
      case 0:  // logical left/no shift
      {
        op2 = amount ? rm<<amount : rm;
        if (islogic && amount && inst.dp.s)
        {
          FLAG(c) = rm & (1U<<(32-amount)) ? 1 : 0;
        }
      }
      break;
      
      case 1:  // logical right
      {
        op2 = amount ? rm>>amount : 0;
        if (islogic && inst.dp.s)
        {
          if (!amount) amount=32;
          FLAG(c) = rm & (1U<<(amount-1)) ? 1 : 0;
        }
      }
      break;
      
      case 2:  // arithmetic right
      {
        op2 = amount ? (uint5)((sint5)rm>>amount) :
              (rm&TOPBIT ? 0xffffffff : 0);
        if (islogic && inst.dp.s)
        {
          if (!amount) amount=32;
          FLAG(c) = rm & (1U<<(amount-1)) ? 1 : 0;
        }
      }
      break;
      
      case 3:  // rotate right/rrx
      {
        if (amount==0)  // rrx
        {
          op2 = (rm>>1) | (FLAG(c)<<31);
          if (islogic && inst.dp.s) FLAG(c) = rm & 1;  // also if !logic?
        }
        else
        {
          op2 = ROR(rm, amount);
          if (islogic && inst.dp.s)
          {
            if (!amount) amount=32;
            FLAG(c) = (rm & (1U<<(amount-1))) ? 1 : 0;
          }
        }
      }
    }
  }
	
  switch (inst.dp.opcode)
  {
    case dp_AND:
    {
      rd = op1 & op2;
      STOREREG(dp, rd);
    }
    break;

    case dp_EOR:
    {
      rd = op1 ^ op2;
      STOREREG(dp, rd);
    }
    break;

    case dp_SUB:
    {
      rd = op1 - op2;
      if (inst.dp.s) { SUBFLAGS(op1,op2); }
      STOREREG(dp, rd);
    }
    break;

    case dp_RSB:
    {
      rd = op2 - op1;
      if (inst.dp.s) { SUBFLAGS(op2,op1); }
      STOREREG(dp, rd);
    }
    break;

    case dp_ADD:
    {
      rd = op1 + op2;
      if (inst.dp.s) { ADDFLAGS(op1,op2); }
      STOREREG(dp, rd);
    }
    break;

    case dp_ADC:
    {
      rd = op1 + op2 + FLAG(c);
      if (inst.dp.s) { ADDFLAGS(op1,op2); }
      STOREREG(dp, rd);
    }
    break;

    case dp_SBC:
    {
      rd = op1 - op2 - !FLAG(c);
      if (inst.dp.s) { SUBFLAGS(op1,op2); }
      STOREREG(dp, rd);
    }
    break;

    case dp_RSC:
    {
      rd = op2 - op1 - !FLAG(c);
      if (inst.dp.s) { SUBFLAGS(op2,op1); }
      STOREREG(dp, rd);
    }
    break;

    case dp_TST:
    {
      rd = op1 & op2;
    }
    break;

    case dp_TEQ:
    {
      rd = op1 ^ op2;
    }
    break;

    case dp_CMP:
    {
      rd = op1 - op2;
      SUBFLAGS(op1,op2);  // always affect flags
    }
    break;

    case dp_CMN:
    {
      rd = op1 + op2;
      ADDFLAGS(op1,op2);  // always affect flags
    }
    break;

    case dp_ORR:
    {
      rd = op1 | op2;
      STOREREG(dp, rd);
    }
    break;

    case dp_MOV:
    {
      rd = op2;
      STOREREG(dp, rd);
    }
    break;

    case dp_BIC:
    {
      rd = op1 & ~op2;
      STOREREG(dp, rd);
    }
    break;

    case dp_MVN:
    {
      rd = ~op2;
      STOREREG(dp, rd);
    }
    break;
  }
  // deal with Z and N flags
  if (inst.dp.s) { ZNFLAGS; }

  if (inst.dp.rd==15 && (affectrd & (1<<inst.dp.opcode)))
    reg->r[15]+=8;
  else
    INCPC;
}

void exec_dp_imm(machineinfo* machine, instructionformat inst, void* null)
{
  registerinfo* reg = machine->reg;
  uint5 op1 = GET(inst.dp.rn);
  uint5 op2, temp = inst.dp.operand2;
  uint5 rd = 0, amount = (temp>>8)*2;
  // mask for rd-affecting instructions
  const uint5 affectrd = 0xf0ff;

  if (!exec_condition(machine, inst)) return;

  op2 = ROR(temp&255, amount);
  // why isn't this more clear in the data sheet? grr...
  if (inst.dp.s && amount) FLAG(c) = (temp & (1U<<(amount-1))) ? 1 : 0;

  switch (inst.dp.opcode)
  {
    case dp_AND:
    {
      rd = op1 & op2;
      STOREREG(dp, rd);
    }
    break;

    case dp_EOR:
    {
      rd = op1 ^ op2;
      STOREREG(dp, rd);
    }
    break;

    case dp_SUB:
    {
      rd = op1 - op2;
      if (inst.dp.s) { SUBFLAGS(op1,op2); }
      STOREREG(dp, rd);
    }
    break;

    case dp_RSB:
    {
      rd = op2 - op1;
      if (inst.dp.s) { SUBFLAGS(op2,op1); }
      STOREREG(dp, rd);
    }
    break;

    case dp_ADD:
    {
      rd = op1 + op2;
      if (inst.dp.s) { ADDFLAGS(op1,op2); }
      STOREREG(dp, rd);
    }
    break;

    case dp_ADC:
    {
      rd = op1 + op2 + FLAG(c);
      if (inst.dp.s) { ADDFLAGS(op1,op2); }
      STOREREG(dp, rd);
    }
    break;

    case dp_SBC:
    {
      rd = op1 - op2 - !FLAG(c);
      if (inst.dp.s) { SUBFLAGS(op1,op2); }
      STOREREG(dp, rd);
    }
    break;

    case dp_RSC:
    {
      rd = op2 - op1 - !FLAG(c);
      if (inst.dp.s) { SUBFLAGS(op2,op1); }
      STOREREG(dp, rd);
    }
    break;

    case dp_TST:
    {
      rd = op1 & op2;
    }
    break;

    case dp_TEQ:
    {
      rd = op1 ^ op2;
    }
    break;

    case dp_CMP:
    {
      rd = op1 - op2;
      SUBFLAGS(op1,op2);  // always affect flags
    }
    break;

    case dp_CMN:
    {
      rd = op1 + op2;
      ADDFLAGS(op1,op2);  // always affect flags
    }
    break;

    case dp_ORR:
    {
      rd = op1 | op2;
      STOREREG(dp, rd);
    }
    break;

    case dp_MOV:
    {
      rd = op2;
      STOREREG(dp, rd);
    }
    break;

    case dp_BIC:
    {
      rd = op1 & ~op2;
      STOREREG(dp, rd);
    }
    break;

    case dp_MVN:
    {
      rd = ~op2;
      STOREREG(dp, rd);
    }
    break;
  }
  // deal with Z and N flags
  if (inst.dp.s) { ZNFLAGS; }

  if (inst.dp.rd==15 && (affectrd & (1<<inst.dp.opcode)))
    reg->r[15]+=8;
  else
    INCPC;
}

#endif

void exec_bra(machineinfo* machine, instructionformat inst, void* null)
{
  if (!exec_condition(machine, inst)) return;
  else
  {
    registerinfo* reg = machine->reg;
    sint5 offset = (sint5)(inst.bra.offset<<8)>>6;
    offset+=8;

    if (offset==0) exit(0);


    // PC now holds actual instruction+8 bytes, same as a real ARM
    if (inst.bra.l) RPUT(14, RGET(15)-4);  // prefetch adjustment
    RPUT(15, RGET(15)+offset);
  }
}

void exec_mul(machineinfo* machine, instructionformat inst, void* null)
{
  if (!exec_condition(machine, inst)) return;
  else
  {
    registerinfo* reg = machine->reg;
    uint5 op1 = RGET(inst.mul.rm), op2 = RGET(inst.mul.rs), rd;

    if (inst.mul.a)  // accumulate bit set
    {
      rd = op1*op2+RGET(inst.mul.rn);
    }
    else
    {
      rd = op1*op2;
    }
    RPUT(inst.mul.rd, rd);
    if (inst.mul.s) { ZNFLAGS; }
    INCPC;
  }
}

void exec_sdt(machineinfo* machine, instructionformat inst, void* null)
{
	if (!exec_condition(machine, inst)) return;
  else
  {
	  registerinfo* reg = machine->reg;
	  meminfo* mem = machine->mem;
	  uint5* base = (uint5*)GET(inst.sdt.rn), *addr, *alignaddr;
	  uint5 byteoffset;
	  sint5 offset = 0;

	  if (inst.sdt.i)
	  {
		  uint5 temp = inst.sdt.offset;
		  uint5 rm = GET(temp&15), shifttype = (temp>>5)&3, amount = (temp>>7);

		  switch (shifttype)
		  {
			  case 0:  // LSL / no shift
			  {
				  offset = amount ? rm<<amount : rm;
			  }
			  break;

			  case 1:  // LSR
			  {
				  offset = amount ? rm>>amount : 0;
			  }
			  break;

			  case 2:  // ASR
			  {
				  offset = amount ? (uint5)((sint5)rm>>amount)
                          : ((rm&TOPBIT) ? 0xffffffff : 0);
			  }
			  break;

			  case 3:  // ROR + RRX (only a sick person would use RRX here, right?)
			  {
				  offset = (amount==0) ? (rm>>1) | (FLAG(c)<<31) : ROR(rm, amount);
			  }
			  break;
		  }
	  }
	  else  // 'i' bit zero, is immediate offset
	  {
		  offset = inst.sdt.offset;
	  }

	  if (!inst.sdt.u) offset = -offset;

	  if (inst.sdt.p)  // pre-indexed if 1
		  base = (uint5*)(((uint5)base)+offset);

	  addr = memory_lookup(mem, base);
	  alignaddr = (uint5*)((uint5)addr&~3);
	  byteoffset = (uint5)addr & 3;

	  if (inst.sdt.l)  // load
	  {
		  if (inst.sdt.b)
		  {
			  uint3* caddr = (uint3*) addr;
			  RPUT(inst.sdt.rd, (uint5) *caddr);
		  }
		  else
		  {
			  if (byteoffset) PUT(inst.sdt.rd, ROR(*alignaddr, byteoffset*8));
			  else PUT(inst.sdt.rd, *alignaddr);
		  }
	  }
	  else  // store
	  {
	    // STR PC stores PC+12
	    uint5 src = inst.sdt.rd==15 ? RGET(inst.sdt.rd)+4 : RGET(inst.sdt.rd);
		  if (inst.sdt.b)
		  {
			  uint3* caddr = (uint3*) addr;
			  *caddr = (unsigned char) src;
		  }
		  else
		  {
			  *alignaddr = src;
		  }
      #ifdef VIRTUALFRAMEBUFFER
      memory_postwrite(mem, base);
      #endif
	  }

	  if (!inst.sdt.p)  // post-index addressing
	  {
      // logic previously broke on, eg, ldr r5,[r5],#0
		  RPUT(inst.sdt.rn, RGET(inst.sdt.rn)+offset);
	  }

	  if (inst.sdt.w)  // writeback address into base
	  {
		  RPUT(inst.sdt.rn, (uint5)base);
	  }

    // pipelining correction  
    if (inst.sdt.l && inst.sdt.rd==15)
      reg->r[15]+=8;
    else
      INCPC;
  }
}

// macros relating to block data transfer
/*
#define TESTOVERLAP if ((uint5)prev>>12 != (uint5)physbase>>12) \
                      physbase = memory_lookup(mem, base)
*/
// we're living in a flat address space for now - no overlap testing necessary
#define TESTOVERLAP 
#define INC base++; physbase++;
#define DEC base--; physbase--;
#define TRANSFER if (inst.bdt.l) PUT(i, *physbase); \
                   else *physbase = GET(i);
// inst.bdt.s means 'hat'
// if writing PC, writes *the raw value* (don't know if this is correct)
#define PCTRANSFER if (inst.bdt.l) \
                     if (inst.bdt.s) PCSETADFL(*physbase); \
                       else PCSETADDR(*physbase); \
                   else *physbase = reg->r[15];

// block data transfer always tries to use ready-translated address unless
// straddling a page boundary
void exec_bdt(machineinfo* machine, instructionformat inst, void* null)
{
	if (!exec_condition(machine, inst)) return;
  else
  {
	  registerinfo* reg = machine->reg;
	  meminfo* mem = machine->mem;
    uint5* base = (uint5*)GET(inst.bdt.rn);
    int i;

    if (inst.bdt.u)    // transfer registers upwards in memory
    {
      uint5 *prev, *physbase = (uint5*)((uint5)memory_lookup(mem, base)&~3);
      if (inst.bdt.p)  // preincrement
      {
        for (i=0; i<15; i++)
        {
          if (inst.bdt.reglist & (1<<i))
          {
            prev = physbase;
            INC;
            TESTOVERLAP;
            TRANSFER;
          }
        }
        if (inst.bdt.reglist & (1<<15))
        {
          prev = physbase;
          INC;
          TESTOVERLAP;
          PCTRANSFER;
        }
      }
      else  // postincrement
      {
        for (i=0; i<15; i++)
        {
          if (inst.bdt.reglist & (1<<i))
          {
            prev = physbase;
            TRANSFER;
            INC;
            TESTOVERLAP;
          }
        }
        if (inst.bdt.reglist & (1<<15))
        {
          prev = physbase;
          PCTRANSFER;
          INC;
          TESTOVERLAP;
        }
      }
    }
    else   // transfer registers downwards in memory
    {
      uint5 *prev, *physbase = (uint5*)((uint5)memory_lookup(mem, base)&~3);
      if (inst.bdt.p) // predecrement
      {
        if (inst.bdt.reglist & (1<<15))
        {
          prev = physbase;
          DEC;
          TESTOVERLAP;
          PCTRANSFER;
        }
        for (i=14; i>=0; i--)
        {
          if (inst.bdt.reglist & (1<<i))
          {
            prev = physbase;
            DEC;
            TESTOVERLAP;
            TRANSFER;
          }
        }
      }
      else  // postdecrement
      {
        if (inst.bdt.reglist & (1<<15))
        {
          prev = physbase;
          PCTRANSFER;
          DEC;
          TESTOVERLAP;
        }
        for (i=14; i>=0; i--)
        {
          if (inst.bdt.reglist & (1<<i))
          {
            prev = physbase;
            TRANSFER;
            DEC;
            TESTOVERLAP;
          }
        }
      }
    }
	  if (inst.bdt.w) // writeback
	  {
		  PUT(inst.bdt.rn, (uint5)base);
	  }

  #ifdef VIRTUALFRAMEBUFFER
    if (!inst.bdt.l) memory_postwrite(mem, base);
  #endif

	  // pipeline correction, or just next instruction
    if (inst.bdt.l && (inst.bdt.reglist & (1<<15)))
      reg->r[15]+=8;
    else
      INCPC;
  }
}

/*
#define read(A,B,C) printf("fread(%x,%x,%x,%x)\n", (A),(B),(C),(D))
#define write(A,B,C) printf("fwrite(%x,%x,%x,%x)\n", (A),(B),(C),(D))
*/

void exec_swi(machineinfo* machine, instructionformat inst, void* null)
{
	if (!exec_condition(machine, inst)) return;
  else
  {
	  registerinfo* reg = machine->reg;

  #ifdef FAKESWI
    fake_syscall(machine, inst.swi.number);
  #endif

	  INCPC;
  }
}

void exec_cdt(machineinfo* machine, instructionformat inst, void* null)
{
  // unimplemented
}

void exec_cdo(machineinfo* machine, instructionformat inst, void* null)
{
  // unimplemented
}

void exec_crt(machineinfo* machine, instructionformat inst, void* null)
{
  // unimplemented
}

void exec_sds(machineinfo* machine, instructionformat inst, void* null)
{
  // unimplemented
}

void exec_und(machineinfo* machine, instructionformat inst, void* null)
{
  // unimplemented
}
