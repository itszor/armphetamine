#include <stdio.h>

#include "defs.h"
#include "decode.h"
#include "memory.h"
#include "registers.h"
#include "processor.h"

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

#define PRINTOPS 1
#define FAKESWI 1

void decode(uint5 instruction, processorflags* flags, registers* reg, meminfo* mem)
{
  instructionformat inst;
#ifdef PRINTOPS
	const char* txtcc[]={	"eq","ne","cs","cc","mi","pl","vs","vc",
												"hi","ls","ge","lt","gt","le","","nv"};
#endif
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

  // There is definitely an optimal order for these tests
  // - and this probably isn't it.
  if (is_dp(instruction))
  {
    uint5 op1 = reg->r[inst.dp.rn];
    uint5 op2, temp = inst.dp.operand2;
    uint5 rd;
    const uint5 logic = 0xf303;  // mask for logic rather than arithmetic instructions
#ifdef PRINTOPS
		const char* txtops[] = {"and","eor","sub","rsb","add","adc","sbc","rsc",
														"tst","teq","cmp","cmn","orr","mov","bic","mvn"};
		const int ignoreop1[] = {0,0,0,0,0,0,0,0,1,1,1,1,0,1,0,1};
#endif

    if (inst.dp.i)  // immediate operand
    {
      op2 = ROR(temp&255, (temp>>8)*2);
#ifdef PRINTOPS
			if (ignoreop1[inst.dp.opcode])
				printf("%s%s%s r%d,#%d\n", txtops[inst.dp.opcode], txtcc[inst.generic.cond],
					inst.dp.s ? "s" : "", inst.dp.rd, op2);
			else
				printf("%s%s%s r%d,r%d,#%d\n", txtops[inst.dp.opcode], txtcc[inst.generic.cond],
				inst.dp.s ? "s" : "", inst.dp.rd, inst.dp.rn, op2);
#endif
    }
    else            // shifted register
    {
      uint5 rm = reg->r[temp&15];
      uint5 shifttype = (temp>>5)&3, amount;
      // islogic affects the way the carry flag is set
      int islogic = logic & (1<<inst.dp.opcode);
#ifdef PRINTOPS
			const char* txtshft[] = {"lsl","lsr","asr","ror/rrx"};
#endif

      if (temp & 16)  // shift by register
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
#ifdef PRINTOPS
			printf("%s%s%s r%d,r%d,%s '%d' (=%d)\n", inst.dp.s ? "s" : "",
				txtops[inst.dp.opcode], txtcc[inst.generic.cond], inst.dp.rd, inst.dp.rn,
				txtshft[shifttype], amount, op2);
#endif
    }

    switch (inst.dp.opcode)
    {
      case dp_AND:
      {
        rd = op1 & op2;
        STOREREG;
      }
      break;

      case dp_EOR:
      {
        rd = op1 ^ op2;
        STOREREG;
      }
      break;

      case dp_SUB:
      {
        rd = op1 - op2;
        if (inst.dp.s) { SUBFLAGS(op1,op2) }
        STOREREG;
      }
      break;

      case dp_RSB:
      {
        rd = op2 - op1;
        if (inst.dp.s) { SUBFLAGS(op2,op1) }
        STOREREG;
      }
      break;

      case dp_ADD:
      {
        rd = op1 + op2;
        if (inst.dp.s) { ADDFLAGS(op1,op2) }
        STOREREG;
      }
      break;

      case dp_ADC:
      {
        rd = op1 + op2 + flags->flag.carry;
        if (inst.dp.s) { ADDFLAGS(op1,op2) }
        STOREREG;
      }
      break;

      case dp_SBC:
      {
        rd = op1 - op2 - !flags->flag.carry;
        if (inst.dp.s) { SUBFLAGS(op1,op2) }
        STOREREG;
      }
      break;

      case dp_RSC:
      {
        rd = op2 - op1 - !flags->flag.carry;
        if (inst.dp.s) { SUBFLAGS(op2,op1) }
        STOREREG;
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
        if (inst.dp.s) { SUBFLAGS(op1,op2) }
      }
      break;

      case dp_CMN:
      {
        rd = op1 + op2;
        if (inst.dp.s) { ADDFLAGS(op1,op2) }
      }
      break;

      case dp_ORR:
      {
        rd = op1 | op2;
        STOREREG;
      }
      break;

      case dp_MOV:
      {
        rd = op2;
        STOREREG;
      }
      break;

      case dp_BIC:
      {
        rd = op1 & ~op2;
        STOREREG;
      }
      break;

      case dp_MVN:
      {
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
    offset = (inst.bra.offset & (1<<23)) ? (inst.bra.offset<<2) | 0xfc000000 : inst.bra.offset<<2;
#ifdef PRINTOPS
	    printf("b%s pc%s%d\n", txtcc[inst.generic.cond], offset+8>0 ? "+" : "", offset+8);			
#endif
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
		uint5* base = (uint5*)reg->r[inst.sdt.rn], *addr, *alignaddr;
		uint5 byteoffset;
		sint5 offset;
		
		if (inst.sdt.i)
		{
			uint5 temp = inst.sdt.offset;
			uint5 rm = reg->r[temp&15], shifttype = (temp>>5)&3, amount = (temp>>7);
			
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
					offset = amount ? (uint5)((sint5)rm>>amount) : ((rm&TOPBIT) ? 0xffffffff : 0);
				}
				break;
				
				case 3:  // ROR + RRX (only a sicko would use RRX here?)
				{
					offset = (amount==0) ? (rm>>1) | (flags->flag.carry<<31) : ROR(rm, amount);
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
		{
			base = (uint5*)((uint5)base+offset);
		}
				
		addr = memory_lookup(mem, base);
		alignaddr = (uint5*)((uint5)addr&~3);
		byteoffset = (uint5)addr & 3;
		
		if (inst.sdt.l)  // load
		{
#ifdef PRINTOPS
			printf("ldr%s%s r%d,[r%d,'%d'] (0x%x from 0x%x)]\n", txtcc[inst.generic.cond],
				inst.sdt.b ? "b" : "", inst.sdt.rd, inst.sdt.rn, offset, (uint5)base);
#endif
			if (inst.sdt.b)
			{
				unsigned char* caddr = (char*) addr;
				reg->r[inst.sdt.rd] = (uint5) *caddr;
			}
			else
			{
				if (byteoffset) reg->r[inst.sdt.rd] = ROR(*alignaddr, 32-(byteoffset*8));
				else reg->r[inst.sdt.rd] = *alignaddr;
			}
		}
		else  // save
		{
#ifdef PRINTOPS
			printf("str%s%s r%d,[r%d,'%d'] (='0x%x')\n", txtcc[inst.generic.cond],
				inst.sdt.b ? "b" : "", inst.sdt.rd, inst.sdt.rn, offset, (uint5)base);
#endif
			if (inst.sdt.b)
			{
				unsigned char* caddr = (char*) addr;
				*caddr = (unsigned char) reg->r[inst.sdt.rd];
			}
			else
			{
				if (byteoffset) *alignaddr = ROR(reg->r[inst.sdt.rd], 32-(byteoffset*8));
				else *alignaddr = reg->r[inst.sdt.rd];
			}
		}
		
		if (!inst.sdt.p)  // post-index addressing
		{
		  reg->r[inst.sdt.rn] = (uint5)base = (uint5*)base+offset;
		}
		
		if (inst.sdt.w)  // writeback address into base
		{
			reg->r[inst.sdt.rn] = (uint5)base;
		}
		
		INCPC;
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
      uint5 *prev, *physbase = (uint5*)((uint5)memory_lookup(mem, base)&~3);
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
      uint5 *prev, *physbase = (uint5*)((uint5)memory_lookup(mem, base)&~3);
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
#ifdef PRINTOPS
    printf("swi%s 0x%x\n", txtcc[inst.generic.cond], inst.swi.number);
#endif
#ifdef FAKESWI
		switch (inst.swi.number)
		{
			case 0: puts(reg->r[0]); break;
		}
#endif
		INCPC;
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
