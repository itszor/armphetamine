#include <stdio.h>

#include "defs.h"
#include "decode.h"
#include "memory.h"

#define TOPBIT 0x80000000U

// this logic is stolen from ArcEm
#define ADDFLAGS(A,B) \
  flags->flag.overflow = ((((A)&(B)&~rd)&TOPBIT) || ((~(A)&~(B)&rd)&TOPBIT)) ? 1 : 0; \
  flags->flag.carry = ((((A)&(B))&TOPBIT) \
                   || (((A)&~rd)&TOPBIT) \
                   || (((B)&~rd)&TOPBIT)) ? 1 : 0;

#define SUBFLAGS(A,B) \
  flags->flag.overflow = ((((A)&~(B)&~rd)&TOPBIT) || ((~(A)&(B)&rd)&TOPBIT)) ? 1 : 0; \
  flags->flag.carry = ((((A)&~(B))&TOPBIT) \
                   || (((A)&rd)&TOPBIT) \
                   || ((~(B)&rd)&TOPBIT)) ? 1 : 0;

#define ZNFLAGS \
  flags->flag.zero = rd==0; \
  flags->flag.sign = rd>>31;

#define STOREREG reg->r[inst.dp.rd] = rd
#define INCPC reg->r[15]+=4
#define PCADDR (reg->r[15] & ~0xfc000003)
#define PCSETADDR(X) reg->r[15] = (reg->r[15] & 0xfc000003) | ((X) & ~0xfc000003)
#define PCSETADFL(X) reg->r[15] = (reg->r[15] & 0x0c000003) | ((X) & ~0x0c000003)

void decode(uint5 instruction, processorflags* flags, registers* reg, meminfo* mem)
{
  instructionformat inst;
  inst.instruction = instruction;

  // conditional execution on any instruction
  switch (inst.generic.cond)
  {
    case cc_EQ:  // Z set
    if (!flags->flag.zero) {
      INCPC;
      return;
    }
    break;

    case cc_NE:  // Z clear
    if (flags->flag.zero) {
      INCPC;
      return;
    }
    break;

    case cc_CS:  // C set (unsigned higher or same)
    if (!flags->flag.carry) {
      INCPC;
      return;
    }
    break;

    case cc_CC:  // C clear (unsigned lower)
    if (flags->flag.carry) {
      INCPC;
      return;
    }
    break;

    case cc_MI:  // N set (negative)
    if (!flags->flag.sign) {
      INCPC;
      return;
    }
    break;

    case cc_PL:  // N clear (positive or zero)
    if (flags->flag.sign) {
      INCPC;
      return;
    }
    break;

    case cc_VS:  // V set (overflow)
    if (!flags->flag.overflow) {
      INCPC;
      return;
    }
    break;

    case cc_VC:  // V clear (overflow)
    if (flags->flag.overflow) {
      INCPC;
      return;
    }
    break;

    case cc_HI:  // C set and Z clear (unsigned higher)
    if (!flags->flag.carry || flags->flag.zero) {
      INCPC;
      return;
    }
    break;

    case cc_LS:  // C clear or Z set (unsigned lower or same)
    if (flags->flag.carry && !flags->flag.zero) {
      INCPC;
      return;
    }
    break;

    case cc_GE:  // N set and V set, or N clear and V clear (greater or equal)
    if (flags->flag.sign ^ flags->flag.overflow) {
      INCPC;
      return;
    }
    break;

    case cc_LT:  // N set and V clear, or N clear and V set (less than)
    if (!(flags->flag.sign ^ flags->flag.overflow)) {
      INCPC;
      return;
    }
    break;

    case cc_GT:  // Z clear, and either N set and V set, or N clear and V clear (greater than)
    if (flags->flag.zero || (flags->flag.sign ^ flags->flag.overflow)) {
      INCPC;
      return;
    }
    break;

    case cc_LE:  // Z set, or N set and V clear, or N clear and V set (less than or equal)
    if (!flags->flag.zero && !(flags->flag.sign ^ flags->flag.overflow)) {
      INCPC;
      return;
    }
    break;

    case cc_AL:  // always
    break;

    case cc_NV:  // never
    INCPC;
    return;
  }

	switch (inst.generic.class1)
  {
		case 
    uint5 op1 = reg->r[inst.dp.rn];
    uint5 op2, temp = inst.dp.operand2;
    uint5 rd;
    const uint5 logic = 0xf303;  // mask for logic rather than arithmetic instructions

    if (inst.dp.i)  // immediate operand
    {
      op2 = ROR(temp&255, (temp>>8)*2);
    }
    else            // shifted register
    {
      uint5 rm = reg->r[temp&15], shift = (temp>>4);
      uint5 shifttype = (temp>>1)&3, amount;
      // islogic affects the way the carry flag is set
      int islogic = logic & (1<<inst.dp.opcode);

      if (shift & 1)  // shift by register
      {
        amount = reg->r[temp>>4];
      }
      else   // shift by immediate
      {
        amount = temp>>3;
      }
      // do the shift
      switch (shifttype)
      {
        case 0:  // logical left, no shift (LSL #0)
        {
          op2 = amount ? rm<<amount : rm;
          if (islogic && amount && inst.dp.s) flags->flag.carry = (rm & (1<<(32-amount))) ? 1 : 0;
        }
        break;

        case 1:  // logical right
        {
          op2 = amount ? rm>>amount : 0;  // LSR #0 means LSR #32
          if (islogic && inst.dp.s)
            flags->flag.carry = (rm & (1<<(amount ? amount-1 : 31))) ? 1 : 0;
        }
        break;

        case 2:  // arithmetic right
        {
          // this probably can't be trusted
          op2 = amount ? (uint5)((sint5)rm>>amount) : ((rm&TOPBIT) ? 0xffffffff : 0);
          if (islogic && inst.dp.s)
            flags->flag.carry = (rm & (1<<(amount ? amount-1 : 31))) ? 1 : 0;
        }
        break;

        case 3:  // rotate right + RRX
        {
          if (amount==0)
          {
            // RRX
            op2 = (rm>>1) | (flags->flag.carry<<31);
            if (islogic && inst.dp.s) flags->flag.carry = (rm & TOPBIT) ? 1 : 0;
          }
          else
          {
            op2 = ROR(rm, amount);
            if (islogic && inst.dp.s) flags->flag.carry = (rm & (1<<(amount-1))) ? 1 : 0;
          }
        }
        break;
      }
    }

    switch (inst.dp.opcode)
    {
      case dp_AND:
      {
        puts("and");
        rd = op1 & op2;
        STOREREG;
      }
      break;

      case dp_EOR:
      {
        puts("eor");
        rd = op1 ^ op2;
        STOREREG;
      }
      break;

      case dp_SUB:
      {
        puts("sub");
        rd = op1 - op2;
        if (inst.dp.s) { SUBFLAGS(op1,op2) }
        STOREREG;
      }
      break;

      case dp_RSB:
      {
        puts("rsb");
        rd = op2 - op1;
        if (inst.dp.s) { SUBFLAGS(op2,op1) }
        STOREREG;
      }
      break;

      case dp_ADD:
      {
        puts("add");
        rd = op1 + op2;
        if (inst.dp.s) { ADDFLAGS(op1,op2) }
        STOREREG;
      }
      break;

      case dp_ADC:
      {
        puts("adc");
        rd = op1 + op2 + flags->flag.carry;
        if (inst.dp.s) { ADDFLAGS(op1,op2) }
        STOREREG;
      }
      break;

      case dp_SBC:
      {
        puts("sbc");
        rd = op1 - op2 - !flags->flag.carry;
        if (inst.dp.s) { SUBFLAGS(op1,op2) }
        STOREREG;
      }
      break;

      case dp_RSC:
      {
        puts("rsc");
        rd = op2 - op1 - !flags->flag.carry;
        if (inst.dp.s) { SUBFLAGS(op2,op1) }
        STOREREG;
      }
      break;

      case dp_TST:
      {
        puts("tst");
        rd = op1 & op2;
      }
      break;

      case dp_TEQ:
      {
        puts("teq");
        rd = op1 ^ op2;
      }
      break;

      case dp_CMP:
      {
        puts("cmp");
        rd = op1 - op2;
        if (inst.dp.s) { SUBFLAGS(op1,op2) }
      }
      break;

      case dp_CMN:
      {
        puts("cmn");
        rd = op1 + op2;
        if (inst.dp.s) { ADDFLAGS(op1,op2) }
      }
      break;

      case dp_ORR:
      {
        puts("orr");
        rd = op1 | op2;
        STOREREG;
      }
      break;

      case dp_MOV:
      {
        puts("mov");
        rd = op2;
        STOREREG;
      }
      break;

      case dp_BIC:
      {
        puts("bic");
        rd = op1 & ~op2;
        STOREREG;
      }
      break;

      case dp_MVN:
      {
        puts("mvn");
        rd = ~op2;
        STOREREG;
      }
      break;
    }
    // deal with Z and N flags
    if (inst.dp.s) { ZNFLAGS }
    INCPC;
  }
  else if (is_bra(instruction))
  {
    sint5 offset;
    puts("branch");
    offset = (inst.bra.offset & (1<<23)) ? (inst.bra.offset<<2) | 0xfc000000 : inst.bra.offset<<2;
    // PC is expected to hold current instruction+8 bytes, actually holds current instruction, so
    if (inst.bra.l) reg->r[14] = reg->r[15];
    PCSETADDR(offset+PCADDR+8);
  }
  else if (is_mul(instruction))
  {
    uint5 op1 = reg->r[inst.mul.rm], op2 = reg->r[inst.mul.rs], rd;
    if (inst.mul.a)  // accumulate bit set
    {
      puts("multiply with accumulate");
      rd = op1*op2+reg->r[inst.mul.rn];
    }
    else
    {
      puts("multiply");
      rd = op1*op2;
    }
    reg->r[inst.mul.rd] = rd;
    if (inst.mul.s) { ZNFLAGS }
    INCPC;
  }
  else if (is_sdt(instruction))
  {
    puts("single-data transfer");
  }
  else if (is_bdt(instruction))   // block data transfer
  {
    uint5* base = (uint5*)reg->r[inst.bdt.rn];
    int i;
    puts("block-data transfer");

// more lovely lovely macros
#define TESTOVERLAP if ((uint5)prev>>12 != (uint5)physbase>>12) physbase = memory_lookup(mem, base)
#define INC base++; physbase++;
#define DEC base--; physbase--;
#define TRANSFER if (inst.bdt.l) reg->r[i] = *physbase; else *physbase = reg->r[i];

    if (inst.bdt.u)    // transfer registers upwards in memory
    {
      uint5 *prev, *physbase = memory_lookup(mem, base);
      if (inst.bdt.p)  // preincrement
      {
        for (i=0; i<16; i++)
        {
          if (inst.bdt.reglist & (1<<i))
          {
            prev = physbase;
            INC;
            TESTOVERLAP;
            TRANSFER;
          }
        }
      }
      else  // postincrement
      {
        for (i=0; i<16; i++)
        {
          if (inst.bdt.reglist & (1<<i))
          {
            prev = physbase;
            TRANSFER;
            INC;
            TESTOVERLAP;
          }
        }
      }
    }
    else   // transfer registers downwards in memory
    {
      uint5 *prev, *physbase = memory_lookup(mem, base);
      if (inst.bdt.p) // predecrement
      {
        for (i=15; i>=0; i--)
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
        for (i=15; i>=0; i--)
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
    INCPC;
  }
  else if (is_swi(instruction))
  {
    puts("swi");
  }
  else if (is_cdt(instruction))
  {
    puts("co-processor data transfer");
  }
  else if (is_cdo(instruction))
  {
    puts("co-processor data op");
  }
  else if (is_crt(instruction))
  {
    puts("co-processor register transfer");
  }
  else if (is_sds(instruction))
  {
    puts("single data swap");
  }
  else if (is_und(instruction))
  {
    // error - undefined instruction
    puts("undefined instruction");
  }
  else puts("bad instruction pattern");
}
