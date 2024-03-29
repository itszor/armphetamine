#include <stdio.h>
//#include <sys/syscall.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "libjtype/defs.h"
#include "core/machine.h"
#include "core/memory.h"
#include "core/registers.h"
#include "core/processor.h"
#include "core/execute.h"

#ifdef ROHLE
#include "fakesys.h"
#endif

#include "core/decode.h"
#include "shell/debug/debug.h"

#ifdef EMULART
#include "mach/lart/intctrl.h"
#endif

/* nzcv eq ne cs cc mi pl vs vc hi ls ge lt gt le al nv
 * 1111 1  0  1  0  1  0  1  0  0  1  1  0  1  0  1  0
 * 1110 1  0  1  0  1  0  0  1  0  1  0  1  0  1  1  0
 * 1101 1  0  0  1  1  0  1  0  0  1  1  0  1  0  1  0
 * 1100 1  0  0  1  1  0  0  1  0  1  0  1  0  1  1  0
 * 1011 0  1  1  0  1  0  1  0  1  0  1  0  0  1  1  0
 * 1010 0  1  1  0  1  0  0  1  1  0  0  1  0  1  1  0
 * 1001 0  1  0  1  1  0  1  0  0  1  1  0  0  1  1  0
 * 1000 0  1  0  1  1  0  0  1  0  1  0  1  0  1  1  0
 * 0111 1  0  1  0  0  1  1  0  0  1  0  1  0  1  1  0
 * 0110 1  0  1  0  0  1  0  1  0  1  1  0  0  1  1  0
 * 0101 1  0  0  1  0  1  1  0  0  1  0  1  0  1  1  0
 * 0100 1  0  0  1  0  1  0  1  0  1  1  0  0  1  1  0
 * 0011 0  1  1  0  0  1  1  0  1  0  0  1  0  1  1  0
 * 0010 0  1  1  0  0  1  0  1  1  0  1  0  1  0  1  0
 * 0001 0  1  0  1  0  1  1  0  0  1  0  1  0  1  1  0
 * 0000 0  1  0  1  0  1  0  1  0  1  1  0  1  0  1  0
 */

/*
    char* ccname[] = {"eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
                    "hi", "ls", "ge", "lt", "gt", "le", "al", "nv"};

    fprintf(stderr, "%s, %s%s%s%s, %s, %x\n", ccname[inst.generic.cond],
    FLAG(n) ? "N" : "n",
    FLAG(z) ? "Z" : "z",
    FLAG(c) ? "C" : "c",
    FLAG(v) ? "V" : "v",
    (cctable[inst.generic.cond] & (1<<(reg->cpsr.value>>28)))
      ? "true"
      : "false",
    reg->cpsr.value>>28);*/

int EXECUTEFN(exec_condition)(machineinfo* machine, instructionformat inst)
{
  static const uint5 cctable[16] =
  {
    0xf0f0, 0x0f0f, 0xcccc, 0x3333,
    0xff00, 0x00ff, 0xaaaa, 0x5555,
    0x0c0c, 0xf3f3, 0xaa55, 0x55aa,
    0xa005, 0x5ffa, 0xffff, 0x0000
  };
  registerinfo* reg = machine->reg;
  // conditional execution on any instruction
  #if 1
  #  ifdef ARM26BIT
  if (cctable[inst.generic.cond] & (1<<(reg->r[15]>>28))) return 1;
  #  else
  if (cctable[inst.generic.cond] & (1<<(reg->cpsr.value>>28))) return 1;
  #  endif

  INCPC;
  return 0;

  #else
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
  #endif
}

#if defined(ASM_ALU) && defined(ARM26BIT)

#warning "ASM_ALU support is out-of-date"

#include "asmalu.h"

int EXECUTEFN(exec_dp)(machineinfo* machine, instructionformat inst,
  void* null)
{
  if (!EXECUTEFN(exec_condition)(machine, inst)) return 0;
  else
  {
    registerinfo* reg = machine->reg;
    uint5 temp = inst.dp.operand2;
    uint5 regshift = (temp & 16) ? 1 : 0;
    uint5 op1 = (inst.dp.rn==15 && regshift) ? GET(inst.dp.rn)+4
                                             : GET(inst.dp.rn);
    // I suspect this should be RGET
    uint5 rm = ((temp&15)==15 && regshift) ? RGET(temp&15)+4 : RGET(temp&15);
    uint5 shifttype = (temp>>5)&3;
    uint5 amount;
/*    dest = inst.dp.rd;
    uint5 affectrd = (1<<inst.dp.opcode) & 0xf0ff;
    uint5 res;*/

    if (regshift) amount = reg->r[temp>>8];
      else amount = temp>>7;
    
    // This is the magic line, all the rest slows it down far too much
    // though...
    asmalutab[((inst.dp.rd==15)?256:0) | (inst.dp.opcode<<4) | 
              (inst.dp.s<<3) | (shifttype<<1) | regshift]
      (op1, rm, amount, &reg->r[15], &reg->r[inst.dp.rd]);

 /*   if (affectrd)
    {
      STOREREG(inst.dp.rd, res);
      reg->r[15] += (dest==15) ? 8 : 4;
    }
    else
    {*/
      reg->r[15] += 4;
 /* }*/
  }
  return 0;
}

int EXECUTEFN(exec_dp_imm)(machineinfo* machine, instructionformat inst,
  void* null)
{
  if (!EXECUTEFN(exec_condition)(machine, inst)) return 0;
  else
  {
    registerinfo* reg = machine->reg;
    uint5 op1 = GET(inst.dp.rn);
    uint5 temp = inst.dp.operand2;
    uint5 amount = (temp>>8)*2;
/*    uint5 dest = inst.dp.rd;
   const uint5 affectrd = (1<<inst.dp.opcode) & 0xf0ff;*/

    asmalutab[((inst.dp.rd==15)?256:0) | (inst.dp.opcode<<4) | 
              (inst.dp.s<<3) | (3<<1) | 1]
      (op1, temp&0xff, amount, &reg->r[15], &reg->r[inst.dp.rd]);

  /*  if (affectrd)
    {
      STOREREG(inst.dp.rd, res);
      reg->r[15] += (dest==15) ? 8 : 4;
    }
    else
    {*/
      reg->r[15] += 4;
  /*  }*/
  }
  return 0;
}

#else

// execute an ALU instruction with op2 as a shifted register
int EXECUTEFN(exec_dp)(machineinfo* machine, instructionformat inst,
  void* null)
{
  registerinfo* reg = machine->reg;
  meminfo* mem = machine->mem;
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
  uint5 rm = ((temp&15)==15 && regshift) ? RGET(temp&15)+INSTSIZE
                                         : RGET(temp&15);
  uint5 shifttype = (temp>>5)&3, amount;
  // islogic affects the way the carry flag is set
  int islogic = logic & (1<<inst.dp.opcode);
#ifdef ARM26BIT
  uint5 set_flags = inst.dp.s;
#else
  uint5 set_flags = inst.dp.s && inst.dp.rd!=15;
#endif

  IGNORE(null);
  #ifdef ARM26BIT
  // ewww
  IGNORE(mem);
  #endif

  if (!EXECUTEFN(exec_condition)(machine, inst)) return 0;

  if ((inst.dp.rn==15 || (temp&15)==15) && regshift)
  {
    fprintf(stderr, "Found allegedly unpredictable instruction:\n");
    dispatch(machine, inst, &diss, (void*)reg->r[15]);
    fprintf(stderr, "\n");
    abort();
  }

  if (regshift)  // shift by register
  {
    amount = RGET(temp>>8) & 0xff; // only the least significant byte
    
    switch (shifttype)  // register shift operations
    {
      case 0:  // logical left/no shift
      {
        op2 = amount>31 ? 0 : rm<<amount;
        if (islogic && amount && set_flags)
        {
          FLAG(c) = amount>31 ? 0 : rm & (1U<<(32-amount)) ? 1 : 0;
        }
      }
      break;
      
      case 1:  // logical right
      {
        op2 = amount>31 ? 0 : rm>>amount;
        if (islogic && amount && set_flags)
        {
          FLAG(c) = amount>32 ? 0 : rm & (1U<<(amount-1)) ? 1 : 0;
        }
      }
      break;
      
      case 2:  // arithmetic right
      {
        op2 = amount>31 ? (rm&TOPBIT ? 0xffffffff : 0)
                        : (uint5)((sint5)rm>>amount);
        if (islogic && amount && set_flags)
        {
          FLAG(c) = amount>32 ? (rm>>31) : rm & (1U<<(amount-1)) ? 1 : 0;
        }
      }
      break;
      
      case 3:  // rotate right (no rrx for register shift)
      {
        amount &= 31;
        op2 = ROR(rm, amount);
        if (islogic && set_flags)
        {
          if (!amount) amount=32;
          FLAG(c) = (rm & (1U<<(amount-1))) ? 1 : 0;
        }
      }
    }
  }
  else   // shift by immediate
  {
    amount = temp>>7;
    
    switch (shifttype)
    {
      case 0:  // logical left/no shift
      {
        op2 = amount ? rm<<amount : rm;
        if (islogic && amount && set_flags)
        {
          FLAG(c) = rm & (1U<<(32-amount)) ? 1 : 0;
        }
      }
      break;
      
      case 1:  // logical right
      {
        op2 = amount ? rm>>amount : 0;
        if (islogic && set_flags)
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
        if (islogic && set_flags)
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
          if (islogic && set_flags) FLAG(c) = rm & 1;  // also if !logic?
        }
        else
        {
          op2 = ROR(rm, amount);
          if (islogic && set_flags)
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
    }
    break;

    case dp_EOR:
    {
      rd = op1 ^ op2;
    }
    break;

    case dp_SUB:
    {
      rd = op1 - op2;
      if (set_flags) { SUBFLAGS(op1,op2); }
    }
    break;

    case dp_RSB:
    {
      rd = op2 - op1;
      if (set_flags) { SUBFLAGS(op2,op1); }
    }
    break;

    case dp_ADD:
    {
      rd = op1 + op2;
      if (set_flags) { ADDFLAGS(op1,op2); }
    }
    break;

    case dp_ADC:
    {
      rd = op1 + op2 + FLAG(c);
      if (set_flags) { ADDFLAGS(op1,op2); }
    }
    break;

    case dp_SBC:
    {
      rd = op1 - op2 - !FLAG(c);
      if (set_flags) { SUBFLAGS(op1,op2); }
    }
    break;

    case dp_RSC:
    {
      rd = op2 - op1 - !FLAG(c);
      if (set_flags) { SUBFLAGS(op2,op1); }
    }
    break;

    case dp_TST:
    {
      if (set_flags)
      {
        rd = op1 & op2;
        if (inst.dp.rd==15)  // tstp? is there even a tstp?
        {
          #ifdef ARM26BIT
          reg->r[15] &= ~reg->pcmask;
          reg->r[15] |= rd & reg->pcmask;
          #else
          if (mem->currentmode!=pm_USR32)
            reg->cpsr = reg->spsr[reg->spsr_current];
          fprintf(stderr, "Ill instruction?\n");
          abort();
          #endif
        }
      }
      else
      {
        return EXECUTEFN(exec_psrt)(machine, inst, op2);
      }
    }
    break;

    case dp_TEQ:
    {
      if (set_flags)
      {
        rd = op1 ^ op2;
        if (inst.dp.rd==15)  // teqp
        {
          #ifdef ARM26BIT
          reg->r[15] &= ~reg->pcmask;
          reg->r[15] |= rd & reg->pcmask;
          #else
          if (mem->currentmode!=pm_USR32)
            reg->cpsr = reg->spsr[reg->spsr_current];
          fprintf(stderr, "Ill instruction?\n");
          abort();
          #endif
        }
      }
      else
      {
        return EXECUTEFN(exec_psrt)(machine, inst, op2);
      }
    }
    break;

    case dp_CMP:
    {
      if (set_flags)
      {
        rd = op1 - op2;
        SUBFLAGS(op1,op2);  // always affect flags
      }
      else
      {
        return EXECUTEFN(exec_psrt)(machine, inst, op2);
      }
    }
    break;

    case dp_CMN:
    {
      if (set_flags)
      {
        rd = op1 + op2;
        ADDFLAGS(op1,op2);  // always affect flags
      }
      else
      {
        return EXECUTEFN(exec_psrt)(machine, inst, op2);
      }
    }
    break;

    case dp_ORR:
    {
      rd = op1 | op2;
    }
    break;

    case dp_MOV:
    {
      rd = op2;
    }
    break;

    case dp_BIC:
    {
      rd = op1 & ~op2;
    }
    break;

    case dp_MVN:
    {
      rd = ~op2;
    }
    break;
  }
  // deal with Z and N flags
  if (set_flags) { ZNFLAGS; }

  if (affectrd & (1<<inst.dp.opcode))
  {
    uint5 pcset = 0;
    if (inst.dp.rd==15)
    {
      rd += INSTSIZE*2;
      pcset = 1;
      if (inst.dp.s && reg->spsr_current != 0)
      {
        psrinfo oldspsr = reg->spsr[reg->spsr_current];
        processor_mode(machine, reg->cpsr.flag.mode, oldspsr.flag.mode);
        reg->cpsr = oldspsr;
      }
    }
    
    RPUT(inst.dp.rd, rd);
    
    if (pcset) return 1;
  }

  INCPC;
  return 0;
}

int EXECUTEFN(exec_dp_imm)(machineinfo* machine, instructionformat inst,
  void* null)
{
  registerinfo* reg = machine->reg;
  meminfo* mem = machine->mem;
  uint5 op1 = GET(inst.dp.rn);
  uint5 op2, temp = inst.dp.operand2;
  uint5 rd = 0, amount = (temp>>8)*2;
  // mask for rd-affecting instructions
  const uint5 affectrd = 0xf0ff;
#ifdef ARM26BIT
  uint5 set_flags = inst.dp.s;
#else
  uint5 set_flags = inst.dp.s && inst.dp.rd!=15;
#endif
  

  IGNORE(null);
#ifdef ARM26BIT
  IGNORE(mem);
#endif

  if (!EXECUTEFN(exec_condition)(machine, inst)) return 0;

  op2 = ROR(temp&255, amount);
  // why isn't this more clear in the data sheet? grr...
  if (set_flags && amount) FLAG(c) = (temp & (1U<<(amount-1))) ? 1 : 0;

  switch (inst.dp.opcode)
  {
    case dp_AND:
    {
      rd = op1 & op2;
    }
    break;

    case dp_EOR:
    {
      rd = op1 ^ op2;
    }
    break;

    case dp_SUB:
    {
      rd = op1 - op2;
      if (set_flags) { SUBFLAGS(op1,op2); }
    }
    break;

    case dp_RSB:
    {
      rd = op2 - op1;
      if (set_flags) { SUBFLAGS(op2,op1); }
    }
    break;

    case dp_ADD:
    {
      rd = op1 + op2;
      if (set_flags) { ADDFLAGS(op1,op2); }
    }
    break;

    case dp_ADC:
    {
      rd = op1 + op2 + FLAG(c);
      if (set_flags) { ADDFLAGS(op1,op2); }
    }
    break;

    case dp_SBC:
    {
      rd = op1 - op2 - !FLAG(c);
      if (set_flags) { SUBFLAGS(op1,op2); }
    }
    break;

    case dp_RSC:
    {
      rd = op2 - op1 - !FLAG(c);
      if (set_flags) { SUBFLAGS(op2,op1); }
    }
    break;

    case dp_TST:
    {
      if (inst.dp.s)
      {
        rd = op1 & op2;
        if (inst.dp.rd==15)
        {
          #ifdef ARM26BIT
          reg->r[15] &= ~reg->pcmask;
          reg->r[15] |= rd & reg->pcmask;
          #else
          if (mem->currentmode!=pm_USR32)
            reg->cpsr = reg->spsr[reg->spsr_current];
          fprintf(stderr, "Ill instruction?\n");
          abort();
          #endif
        }
      }
      else
      {
        return EXECUTEFN(exec_psrt)(machine, inst, op2);
      }
    }
    break;

    case dp_TEQ:
    {
      if (inst.dp.s)
      {
        rd = op1 ^ op2;
        if (inst.dp.rd==15)
        {
          #ifdef ARM26BIT
          reg->r[15] &= ~reg->pcmask;
          reg->r[15] |= rd & reg->pcmask;
          #else
          if (mem->currentmode!=pm_USR32)
            reg->cpsr = reg->spsr[reg->spsr_current];
          fprintf(stderr, "Ill instruction?\n");
          abort();
          #endif
        }
      }
      else
      {
        return EXECUTEFN(exec_psrt)(machine, inst, op2);
      }
    }
    break;

    case dp_CMP:
    {
      if (set_flags)
      {
        rd = op1 - op2;
        SUBFLAGS(op1,op2);
      }
      else
      {
        return EXECUTEFN(exec_psrt)(machine, inst, op2);
      }
    }
    break;

    case dp_CMN:
    {
      if (set_flags)
      {
        rd = op1 + op2;
        ADDFLAGS(op1,op2);
      }
      else
      {
        return EXECUTEFN(exec_psrt)(machine, inst, op2);
      }
    }
    break;

    case dp_ORR:
    {
      rd = op1 | op2;
    }
    break;

    case dp_MOV:
    {
      rd = op2;
    }
    break;

    case dp_BIC:
    {
      rd = op1 & ~op2;
    }
    break;

    case dp_MVN:
    {
      rd = ~op2;
    }
    break;
  }
  // deal with Z and N flags
  if (set_flags) { ZNFLAGS; }

  if (affectrd & (1<<inst.dp.opcode))
  {
    uint5 pcset = 0;
    if (inst.dp.rd==15)
    {
      rd += INSTSIZE*2;
      pcset = 1;
      if (inst.dp.s && reg->spsr_current != 0)
      {
        psrinfo oldspsr = reg->spsr[reg->spsr_current];
        processor_mode(machine, reg->cpsr.flag.mode, oldspsr.flag.mode);
        reg->cpsr = oldspsr;
      }
    }
    
    RPUT(inst.dp.rd, rd);
    
    if (pcset) return 1;
  }

  INCPC;
  return 0;
}

#endif

int EXECUTEFN(exec_psrt)(machineinfo* machine, instructionformat inst,
  uint5 val)
{
  registerinfo* reg = machine->reg;
/*  meminfo* mem = machine->mem;*/

  /* MRS */
  if (inst.mrs.ident==0 && inst.mrs.ident2==15 && inst.mrs.ident3==2)
  {
    PUT(inst.mrs.rd, inst.mrs.ps ? reg->spsr[reg->spsr_current].value
                                 : reg->cpsr.value);
  }
  /* MSR (field) */
  else if ((inst.msrf.ident&0x20f)==0x20f
           && inst.msrf.ident2==2 && inst.msrf.ident3==0)
  {
    uint5 field = ((inst.msrf.ident&0x80)?0xff000000:0)
                | ((inst.msrf.ident&0x40)?0x00ff0000:0)
                | ((inst.msrf.ident&0x20)?0x0000ff00:0)
                | ((inst.msrf.ident&0x10)?0x000000ff:0);
    if (inst.msrf.pd)
    {
      reg->spsr[reg->spsr_current].value &= ~field;
      reg->spsr[reg->spsr_current].value |= val & field;
/*      fprintf(stderr, "Set spsr[%d] to %.8x, field=%.8x\n", reg->spsr_current,
        reg->spsr[reg->spsr_current].value, field);*/
    }
    else
    {
/*      uint5 interrupt = 0;
      uint5 before = reg->cpsr.flag.interrupt & 0x2;*/

      /* control fields updated? */
      if (field & 0xff)
      {
        int oldmode = reg->cpsr.flag.mode;

/*        if (!(reg->cpsr.flag.interrupt & 0x2))
        {
          fprintf(stderr, "-- IRQ Enabled\n");
         // interrupt = 1;
        }
        else
        {
          fprintf(stderr, "-- IRQ Disabled\n");
        }*/

        reg->cpsr.value &= ~field;
        reg->cpsr.value |= val & field;

        if (reg->cpsr.flag.mode != oldmode)
        {
          int newmode = reg->cpsr.flag.mode;
          /*reg->cpsr.flag.mode = oldmode;*/
      /*    reg->spsr[oldmode&15] = oldcpsr;*/
          processor_mode(machine, oldmode, newmode);
          abort();
        }
      /*  INCPC;

        if (interrupt)
        {
          intctrl_fire(machine);
          return 1;
        }
        else return 0;*/
      }
    }
  }
  else
  {
    fprintf(stderr, "WARNING: psrt misdecoded\n");
    fprintf(stderr, "ident=%x ident2=%x ident3=%x\n",
      inst.msrf.ident, inst.msrf.ident2, inst.msrf.ident3);
    fprintf(stderr, "%x\n", inst.instruction);
    dispatch(machine, inst, &diss, (void*)reg->r[15]);
    fprintf(stderr, "\n");
    abort();
  }
  
  INCPC;
  return 0;
}

int EXECUTEFN(exec_bra)(machineinfo* machine, instructionformat inst,
  void* null)
{
  IGNORE(null);

  if (!EXECUTEFN(exec_condition)(machine, inst)) return 0;
  else
  {
    registerinfo* reg = machine->reg;
    sint5 offset = (sint5)(inst.bra.offset<<8)>>6;
    offset+=INSTSIZE*2;

#ifdef EXIT_ON_ZERO_BRANCH
    if (offset==0) exit(0);
#endif

    // PC now holds actual instruction+8 bytes, same as a real ARM
    if (inst.bra.l) RPUT(14, RGET(15)-INSTSIZE);  // prefetch adjustment
    RPUT(15, RGET(15)+offset);
  }
  return 1;
}

int EXECUTEFN(exec_mul)(machineinfo* machine, instructionformat inst,
  void* null)
{
  IGNORE(null);

  if (!EXECUTEFN(exec_condition)(machine, inst)) return 0;
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
  return 0;
}

/* It's probably possible to code this better, not sure about the
 * rules for promoting result wrt 32*32->64 operation, so it promotes
 * both source operands to 64 bits
 */
int EXECUTEFN(exec_mull)(machineinfo* machine, instructionformat inst,
  void* null)
{
  IGNORE(null);

  if (!EXECUTEFN(exec_condition)(machine, inst)) return 0;
  else
  {
    registerinfo* reg = machine->reg;
    uint5 op1 = RGET(inst.mull.rm), op2 = RGET(inst.mull.rs);
    uint6 result;
    
    if (inst.mull.u)  // signed
    {
      if (inst.mull.a)  // accumulate
      {
        result = ((uint6)RGET(inst.mull.rdlo)
               + ((uint6)RGET(inst.mull.rdhi)<<32));
        result += (sint6)op1 * (sint6)op2;
      }
      else
      {
        result = (sint6)op1 * (sint6)op2;
      }
    }
    else  // unsigned
    {
      if (inst.mull.a)  // accumulate
      {
        result = ((uint6)RGET(inst.mull.rdlo)
               + ((uint6)RGET(inst.mull.rdhi)<<32));
        result += (uint6)op1 * (uint6)op2;
      }
      else
      {
        result = (uint6)op1*(uint6)op2;
      }
    }
    if (inst.mul.s)
    {
      FLAG(n) = result>>63;
      FLAG(z) = (result==0) ? 1 : 0;
    }
    RPUT(inst.mull.rdlo, result & 0xffffffff);
    RPUT(inst.mull.rdhi, (result>>32));
    INCPC;
  }
  return 0;
}

int EXECUTEFN(exec_sdt)(machineinfo* machine, instructionformat inst,
  void* null)
{
  IGNORE(null);
  
  if (!EXECUTEFN(exec_condition)(machine, inst)) return 0;
  else
  {
    registerinfo* reg = machine->reg;
    meminfo* mem = machine->mem;
    uint5 base = GET(inst.sdt.rn), addr, alignaddr;
    uint5 byteoffset;
    sint5 offset = 0;
    int t_variant = !inst.sdt.p && inst.sdt.w;
    int memmode_temp;

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
      base += offset;

    // addr = memory_lookup(mem, base);
    addr = base;
    alignaddr = addr & ~3;
    byteoffset = addr & 3;

    /* the T variant works as if we are in non-priveleged mode for
       this transfer, ie that's what the memory system is told.
    */
    if (t_variant)
    {
/*      fprintf(stderr, "Warning: str T variant not tested much\n");*/
      memmode_temp = mem->currentmode;
      /* temporarily force memory system into USER mode */
      mem->currentmode = pm_USR32;
    }

    if (inst.sdt.l)  // load
    {
      if (inst.sdt.b)
      {
        // uint3* caddr = (uint3*) addr;
        // RPUT(inst.sdt.rd, (uint5) *caddr);
        RPUT(inst.sdt.rd, memory_readbyte(mem, addr));
      }
      else
      {
        // if (byteoffset) PUT(inst.sdt.rd, ROR(*alignaddr, byteoffset*8));
        //   else PUT(inst.sdt.rd, *alignaddr);
        if (byteoffset)
        {
          PUT(inst.sdt.rd, ROR(memory_readdataword(mem, alignaddr),
                               byteoffset*8));
        }
        else
        {
          PUT(inst.sdt.rd, memory_readdataword(mem, alignaddr));
 /*         fprintf(stderr, "Writing %x from address %x into r%d\n", reg->r[inst.sdt.rd],
            alignaddr, inst.sdt.rd);*/
        }
      }
    }
    else  // store
    {
      // STR PC stores PC+12
      #ifdef STOREPCPLUS12
      uint5 src = inst.sdt.rd==15 ? RGET(inst.sdt.rd)+INSTSIZE
                                  : RGET(inst.sdt.rd);
      #else
      uint5 src = RGET(inst.sdt.rd);
      #endif
            
      if (inst.sdt.b)
      {
        // uint3* caddr = (uint3*) addr;
        // *caddr = (unsigned char) src;
        memory_writebyte(mem, addr, src);
      }
      else
      {
        // *alignaddr = src;
        memory_writeword(mem, alignaddr, src);
      }

      #ifdef VIRTUALFRAMEBUFFER
      memory_postwrite(mem, base);
      #endif
    }

    if (t_variant)
    {
      mem->currentmode = memmode_temp;
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

    if (mem->memoryfault)
    {
      processor_dataabort(machine);
      mem->memoryfault = 0;
      return 1;
    }

    // pipelining correction  
    if (inst.sdt.l && inst.sdt.rd==15)
    {
      reg->r[15]+=INSTSIZE*2;
      return 1;
    }
    else
    {
      INCPC;
      return 0;
    }
  }
  return 0;
}

int EXECUTEFN(exec_sdth)(machineinfo* machine, instructionformat inst,
  void* null)
{
  if (!EXECUTEFN(exec_condition)(machine, inst)) return 0;
  else
  {
    registerinfo* reg = machine->reg;
    meminfo* mem = machine->mem;
    uint5 base = GET(inst.sdth.rn);
    uint5 offset;

    IGNORE(null);
        
    if (inst.sdth.imm)  // it used to be such a pretty baby
    {
      offset = (inst.instruction & 0xf) |
               ((inst.instruction & 0xf00) >> 4);
    }
    else
    {
      offset = GET(inst.sdth.rm);
    }
    
    if (!inst.sdth.u) offset = -offset;  // down not up;

    if (inst.sdth.p)  // pre-indexing
    {
      base += offset;
    }
    
    if (inst.sdth.l)  // load
    {
      if (inst.sdth.s)  // signed
      {
        if (inst.sdth.h)  // halfword (not byte)
        {
          RPUT(inst.sdth.rd, memory_readshalf(mem, base));
        }
        else  // byte
        {
          RPUT(inst.sdth.rd, memory_readsbyte(mem, base));
        }
      }
      else  // unsigned
      {
        if (inst.sdth.h)
        {
          RPUT(inst.sdth.rd, memory_readhalf(mem, base));
        }
        else  // byte
        {
          // I don't think this is possible
          RPUT(inst.sdth.rd, memory_readbyte(mem, base));
          fprintf(stderr, "WARNING: Found braindamaged load-byte encoding\n");
          fprintf(stderr, "Should have decoded as multiply, swap\n");
          abort();
        }
      }
    }
    else  // store
    {
      uint5 src = inst.sdt.rd==15 ? RGET(inst.sdt.rd)+INSTSIZE
                                  : RGET(inst.sdt.rd);
      if (inst.sdth.h)  // halfword
      {
        memory_writehalf(mem, base, src);
      }
      else  // byte
      {
        fprintf(stderr, "WARNING: Found braindamaged store-byte encoding\n");
        memory_writebyte(mem, base, src);
      }
    }
    
    if (!inst.sdth.p)  // post-indexing
    {
      base += offset;
    }

    if (inst.sdth.w || !inst.sdth.p)  // writeback address into base
    {
      RPUT(inst.sdth.rn, base);
    }
    
    if (mem->memoryfault)
    {
      processor_dataabort(machine);
      mem->memoryfault = 0;
      return 1;
    }
    INCPC;
  }
  return 0;
}


// macros relating to block data transfer
// we're living in a flat address space for now - no overlap testing necessary
#define TESTOVERLAP 
#define INC do { base+=4; } while (0)
#define DEC do { base-=4; } while (0)
#define TRANSFER do { \
                   if (inst.bdt.l) \
                     RPUT(i, memory_readdataword(mem, base)); \
                   else \
                     memory_writeword(mem, base, RGET(i)); \
                 } while (0)
// inst.bdt.s means 'hat'
// if writing PC, writes *the raw value* (don't know if this is correct)
#define PCTRANSFER do { \
                     if (inst.bdt.l) { \
                       if (inst.bdt.s) \
                         PCSETADFL(memory_readdataword(mem, base)); \
                       else \
                         PCSETADDR(memory_readdataword(mem, base)); \
                     } else { \
                       memory_writeword(mem, base, pcstore); \
                     } \
                   } while (0)

// block data transfer always tries to use ready-translated address unless
// straddling a page boundary
int EXECUTEFN(exec_bdt)(machineinfo* machine, instructionformat inst,
  void* null)
{
  IGNORE(null);

  if (!EXECUTEFN(exec_condition)(machine, inst)) return 0;
  else
  {
    registerinfo* reg = machine->reg;
    meminfo* mem = machine->mem;
    uint5 base = RGET(inst.bdt.rn);
    sint5 i;
    uint5 originalmode = reg->cpsr.flag.mode;
    uint5 oldpc = reg->r[15];
    /* the value to store for pc */
    #ifdef STOREPCPLUS12
    uint5 pcstore = reg->r[15]+INSTSIZE;
    #else
    uint5 pcstore = reg->r[15];
    #endif

#ifdef ARM26BIT
    if (inst.bdt.s && !(inst.bdt.l && (inst.bdt.reglist & (1<<15))))
    {
/*      fprintf(stderr, "Hatted %s in 26-bit mode, forcing user "
                      "bank transfer\n", inst.bdt.l ? "LDM" : "STM");*/
      processor_reg_savecurrent(machine, originalmode);
      processor_reg_restorenew(machine, pm_USR26);
    }
#else
    if (inst.bdt.s && !(inst.bdt.l && (inst.bdt.reglist & (1<<15))))
    {
   /*   fprintf(stderr, "Hatted %s in 32-bit mode, forcing user "
                      "bank transfer\n", inst.bdt.l ? "LDM" : "STM");
      fprintf(stderr, "PC %sincluded\n",
        (inst.bdt.reglist & (1<<15)) ? "" : "not ");*/
      processor_reg_savecurrent(machine, originalmode);
      processor_reg_restorenew(machine, pm_USR32);
    }
#endif

    if (inst.bdt.u)    // transfer registers upwards in memory
    {
      // uint5 *prev, *physbase = (uint5*)((uint5)memory_lookup(mem, base)&~3);
      if (inst.bdt.p)  // preincrement
      {
        for (i=0; i<15; i++)
        {
          if (inst.bdt.reglist & (1<<i))
          {
            // prev = physbase;
            INC;
            TRANSFER;
          }
        }
        if (inst.bdt.reglist & (1<<15))
        {
          // prev = physbase;
          INC;
          PCTRANSFER;
        }
      }
      else  // postincrement
      {
        for (i=0; i<15; i++)
        {
          if (inst.bdt.reglist & (1<<i))
          {
            // prev = physbase;
            TRANSFER;
            INC;
          }
        }
        if (inst.bdt.reglist & (1<<15))
        {
          // prev = physbase;
          PCTRANSFER;
          INC;
        }
      }
    }
    else   // transfer registers downwards in memory
    {
      // uint5 *prev, *physbase = (uint5*)((uint5)memory_lookup(mem, base)&~3);
      if (inst.bdt.p) // predecrement
      {
        if (inst.bdt.reglist & (1<<15))
        {
          // prev = physbase;
          DEC;
          PCTRANSFER;
        }
        for (i=14; i>=0; i--)
        {
          if (inst.bdt.reglist & (1<<i))
          {
            // prev = physbase;
            DEC;
            TRANSFER;
          }
        }
      }
      else  // postdecrement
      {
        if (inst.bdt.reglist & (1<<15))
        {
          // prev = physbase;
          PCTRANSFER;
          DEC;
        }
        for (i=14; i>=0; i--)
        {
          if (inst.bdt.reglist & (1<<i))
          {
            // prev = physbase;
            TRANSFER;
            DEC;
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

    if (mem->memoryfault)
    {
      processor_dataabort(machine);
      mem->memoryfault = 0;
      return 1;
    }

//#ifndef ARM26BIT
    if (inst.bdt.s && !(inst.bdt.l && (inst.bdt.reglist & (1<<15))))
    {
      // maybe this can corrupt registers, it's too complicated for me to
      // work out right now?
      processor_reg_savecurrent(machine, pm_USR32);
      processor_reg_restorenew(machine, originalmode);
    }
//#endif

    if (inst.bdt.s && inst.bdt.l && (inst.bdt.reglist & (1<<15)))
    {
      psrinfo oldspsr = reg->spsr[reg->spsr_current];
     /* fprintf(stderr, "Potential trouble spot (LDM)\n");
      fprintf(stderr, "Address = %.8x\n", oldpc-8);*/
/*      machine->trace = 1;*/
    /*  reg->cpsr.flag.interrupt &= 1;*/
     /* fprintf(stderr, "Causing mode switch to: %d\n", 
        reg->spsr[reg->spsr_current].flag.mode);*/

      processor_mode(machine, reg->cpsr.flag.mode, oldspsr.flag.mode);
      reg->cpsr = oldspsr;
    //  reg->cpsr = reg->spsr[oldmode&15];
/*      machine->trace = 1;*/
      /* mumble */
   /*   reg->r[15]+=INSTSIZE*2;
      return 1;*/
     /* fprintf(stderr, "Unimplemented mode change in LDM!\n");
      abort();*/
    }

    // pipeline correction, or just next instruction
    if (inst.bdt.l && (inst.bdt.reglist & (1<<15)))
    {
      reg->r[15]+=INSTSIZE*2;
      return 1;
    }
    else
    {
      INCPC;
      return 0;
    }
  }
}

/*
#define read(A,B,C) printf("fread(%x,%x,%x,%x)\n", (A),(B),(C),(D))
#define write(A,B,C) printf("fwrite(%x,%x,%x,%x)\n", (A),(B),(C),(D))
*/

int EXECUTEFN(exec_swi)(machineinfo* machine, instructionformat inst,
  void* null)
{
  static int fire = 0;
  IGNORE(null);

  if (!EXECUTEFN(exec_condition)(machine, inst)) return 0;
  else
  {
/*    registerinfo* reg = machine->reg;
    uint5 mumble;
    tlbentry null;*/

    ++fire;
    fprintf(stderr, "SWI call: %d\n", fire);

 /*   if (fire==8)
    {
      machine->trace = 1;
    }*/

  #ifdef FAKESWI
    fake_syscall(machine, inst.swi.number);
  #else
    processor_swi(machine);
  #endif
/*    mumble = memory_virtualtophysical(machine->mem, 0xec000030, &null, 4);
    fprintf(stderr, "virtual to physical mapping for %.8x is %.8x\n",
      0xec000030, mumble);*/
    
/*    if (--fire == 0)
    {
      machine->trace = 1;
      machine->detracecounter=500;
    }*/
  }
  return 1;
}

int EXECUTEFN(exec_cdt)(machineinfo* machine, instructionformat inst,
  void* null)
{
  IGNORE(null);

  if (!EXECUTEFN(exec_condition)(machine, inst)) return 0;
  fprintf(stderr, "Error: unimplemented instruction (cdt)\n");
  abort();
  processor_und(machine);
  return 1;
}

int EXECUTEFN(exec_cdo)(machineinfo* machine, instructionformat inst,
  void* null)
{
  IGNORE(null);

  if (!EXECUTEFN(exec_condition)(machine, inst)) return 0;
//  fprintf(stderr, "Error: unimplemented instruction (cdo)\n");
  processor_und(machine);
  return 1;
}

int EXECUTEFN(exec_crt)(machineinfo* machine, instructionformat inst,
  void* null)
{
  IGNORE(null);

/*  machine->trace = 1;
  machine->detracecounter = 10;*/

  if (!EXECUTEFN(exec_condition)(machine, inst)) return 0;
  else
  {
    registerinfo* reg = machine->reg;
    meminfo* mem = machine->mem;
/*    uint5 pcaddr = PCADDR-8;

    fprintf(stderr, "%.8x : %.8x : ", pcaddr, inst.instruction);
    dispatch(machine, inst, &diss, (void*)pcaddr);
    fprintf(stderr, "\n");*/

    if ((mem->currentmode & 15) == pm_USR26)
    {
/*      processor_und(machine);
      return 1;*/
      fprintf(stderr, "Warning: attempt to use coprocessor reg transfer in "
        "usr mode\n");
/*      INCPC;
      return 0;*/
    }
    
    /*fprintf(stderr, "Error: unimplemented instruction (crt)\n");
    exit(1);*/
    switch (inst.crt.cpn)
    {
      case 15:  // MMU control
      {
        if (inst.crt.l)  // coprocessor -> ARM (MRC)
        {
          switch (inst.crt.cpopc)
          {
            case 0:  // dunno what this means exactly
            {
              switch (inst.crt.crn)  // coproc register number
              {
                // identify self as an SA110?
                #ifdef EMULART
                // SA1100 for Lart emulation
                // Stepping 15 arbitrarily ;-)
                case 0: PCFLAGPUT(inst.crt.rd, 0x4401A11f); break;
                #else
                case 0: PCFLAGPUT(inst.crt.rd, 0x4401a100); break;
                #endif
                // is the following line right?
                case 1: PCFLAGPUT(inst.crt.rd, mem->mmucontrol);
                fprintf(stderr, "> Copy mmucontrol to r%d\n", inst.crt.rd);
                break;
                
                case 5:
                PCFLAGPUT(inst.crt.rd, mem->faultstatus);
                abort();
                break;
                
                case 6:
                PCFLAGPUT(inst.crt.rd, mem->faultaddress);
                abort();
                break;
                
                case 0x8: case 0x9: case 0xa: case 0xb:
                case 0xc: case 0xf:
                processor_und(machine);
                return 1;
                
                case 0xd:
                fprintf(stderr, "> Read process ID\n");
                PCFLAGPUT(inst.crt.rd, reg->process_id);
                abort();
                break;
                
                case 0xe:
                fprintf(stderr, "> Read breakpoint\n");
                break;
              }
            }
            break;
            
            default:
            fprintf(stderr, "Unknown MRC OPc type\n");
          }
        }
        else  // ARM -> coprocessor (MCR)
        {
          uint5 srcreg = inst.crt.rd==15 ? RGET(inst.crt.rd)+4
                                         : RGET(inst.crt.rd);
          switch (inst.crt.cpopc)
          {
            case 0:
            {
              switch (inst.crt.crn)  // coproc register number
              {
                case 1:
                {
                  uint5 oldstate = mem->mmucontrol;
                  uint5 newstate = srcreg;
                  fprintf(stderr, "> Set MMU control: %x\n",
                    newstate);

                  if (newstate & (1<<13))
                  {
                    fprintf(stderr, "OMG high vectors\n");
                    reg->vectorbase = 0xffff0000;
                  }
                  else
                  {
                    reg->vectorbase = 0x0;
                  }
                  
                  if ((oldstate & 1) != (newstate & 1))
                  {
                    uint5 instaddr;
                    instructionformat inst;
                    int i;
                    INCPC;
                    // we need to execute the next like two instructions
                    // because the virtual memory system has some latency
                    // when switching on and off. These little things just
                    // kill you honestly.
                    fprintf(stderr, "Virtual memory state changed!\n");
                    for (i=0; i<3; i++)
                    {
                      instaddr = PCADDR-INSTSIZE*2;
                      inst.instruction = memory_readinstword(mem, instaddr);
                      fprintf(stderr, "%.8x : %.8x : ", instaddr, 
                              inst.instruction);
                      dispatch(machine, inst, &diss, (void*)instaddr);
                      fprintf(stderr, "\n");
                      dispatch(machine, inst, machine->exectab, 0);
                    }
/*                    machine->trace = 1;*/
                    fprintf(stderr, "[virtual memory %s]\n",
                      (newstate&1) ? "on" : "off");
              /*      machine->trace = 1;
                    machine->detracecounter = 4;*/
                    mem->mmucontrol = newstate;
                    // otherwise the following data/instructions will get
                    // a cached value!
                    memory_invalidatetlb(&mem->insttlb);
                    memory_invalidatetlb(&mem->datatlb);
                    return 1;
                  }
                  mem->mmucontrol = newstate;
                }
                break;
                
                case 2:
                {
                  fprintf(stderr, "> Set translation base: %x\n", srcreg);
                  mem->translationbase = srcreg;
                }
                break;
                
                case 3:
                {
                  fprintf(stderr, "> Set domain access control: %x\n",
                    srcreg);
                  mem->domainaccesscontrol = srcreg;
                  memory_invalidatetlb(&mem->insttlb);
                  memory_invalidatetlb(&mem->datatlb);
  /*                machine->trace = 1;
                  machine->detracecounter = 0;*/
                }
                break;
                
              /*  case 5: // flush TLB
                {
                  fprintf(stderr, "> Flush TLB\n");
                  memory_invalidatetlb(&mem->insttlb);
                  memory_invalidatetlb(&mem->datatlb);
                }
                break;*/
                
                case 5: // fault status
                {
                  abort();
                }
                break;
                
/*                case 6: // purge TLB
                {
                  fprintf(stderr, "> Purge TLB\n");
                  memory_invalidatetlb(&mem->insttlb);
                  memory_invalidatetlb(&mem->datatlb);
                }
                break;*/
                
                case 6:  // fault address
                {
                  abort();
                }
                break;
                
                case 7: // cache control
                {
/*                  fprintf(stderr, "> Flush IDC\n");*/
                  memory_invalidatetlb(&mem->insttlb);
                  memory_invalidatetlb(&mem->datatlb);
                }
                break;
                
                case 0x8:
                {
                  fprintf(stderr, "> TLB operation (SA)\n");
/*                  machine->trace = 1;*/
                  memory_invalidatetlb(&mem->insttlb);
                  memory_invalidatetlb(&mem->datatlb);
                }
                break;
                
                case 0x9:
                {
                  int subop = (inst.crt.cp << 4) | (inst.crt.crm);
                  fprintf(stderr, "> Read buffer op %x (SA)\n", subop);
                  switch (subop)
                  {
                    case 0x00:  /* flush all */
                    fprintf(stderr, "Flush all RB\n");
                    break;
                    
                    case 0x10:  /* flush buffer 0 */
                    fprintf(stderr, "Flush RB 0\n");
                    break;
                    
                    case 0x11:  /* flush buffer 1 */
                    fprintf(stderr, "Flush RB 1\n");
                    break;
                    
                    case 0x12:  /* flush buffer 2 */
                    fprintf(stderr, "Flush RB 2\n");
                    break;
                    
                    case 0x13:  /* flush buffer 3 */
                    fprintf(stderr, "Flush RB 3\n");
                    break;
                    
                    case 0x20:  /* load buffer 0 with one word */
                    fprintf(stderr, "Load RB0 with one word\n");
                    break;
                    
                    case 0x24:  /* load buffer 0 with four words */
                    fprintf(stderr, "Load RB0 with four words\n");
                    break;
                    
                    case 0x28:  /* load buffer 0 with eight words */
                    fprintf(stderr, "Load RB0 with eight words\n");
                    break;
                    
                    case 0x21:  /* load buffer 1 with one word */
                    fprintf(stderr, "Load RB1 with one word\n");
                    break;
                    
                    case 0x25:  /* load buffer 1 with four words */
                    fprintf(stderr, "Load RB1 with four words\n");
                    break;
                    
                    case 0x29:  /* load buffer 1 with eight words */
                    fprintf(stderr, "Load RB1 with eight words\n");
                    break;

                    case 0x22:  /* load buffer 2 with one word */
                    fprintf(stderr, "Load RB2 with one word\n");
                    break;
                    
                    case 0x26:  /* load buffer 2 with four words */
                    fprintf(stderr, "Load RB2 with four words\n");
                    break;
                    
                    case 0x2a:  /* load buffer 2 with eight words */
                    fprintf(stderr, "Load RB2 with eight words\n");
                    break;

                    case 0x23:  /* load buffer 3 with one word */
                    fprintf(stderr, "Load RB3 with one word\n");
                    break;
                    
                    case 0x27:  /* load buffer 3 with four words */
                    fprintf(stderr, "Load RB3 with four words\n");
                    break;
                    
                    case 0x2b:  /* load buffer 3 with eight words */
                    fprintf(stderr, "Load RB3 with eight words\n");
                    break;
                    
                    case 0x40:  /* disable user-mode mcr access */
                    fprintf(stderr, "Disable user-mode mcr access\n");
                    break;
                    
                    case 0x50:  /* enable user-mode mcr access */
                    fprintf(stderr, "Enable user-mode mcr access\n");
                    break;
                    
                    default:
                    fprintf(stderr, "Unknown read buffer op\n");
                    abort();
                  }
                }
                break;
                
                case 0xa: case 0xb:
                case 0xc:
                fprintf(stderr, "> Throwing undefined instruction for MCR\n");
/*                machine->trace = 1;
                machine->detracecounter = 16;
                reg->r[15] -= 8;*/
                processor_und(machine);
                return 1;
                break;
                
                case 0xd: 
                {
                  fprintf(stderr, "> Write process ID\n");
                  reg->process_id = srcreg;
                  abort();
                }
                break;
                
                case 0xe:
                {
                  fprintf(stderr, "> Write breakpoint\n");
                }
                break;
                
                case 0xf:
                {
                  fprintf(stderr, "> Test, Clock, Idle ctrl (SA)\n");
           /*       machine->trace = 1;
                  machine->detracecounter = 4;*/
                }
                break;
              }
            }
            break;
            
            default:
            {
              fprintf(stderr, "Unknown MCR OPc type\n");
              abort();
            }
          }
        }
      }
      break;
      
      default:
      {
        char* cmd = strdup("disassemble 0xc0008000 0xc0008080\n");
        char* cmd2 = strdup("virtual 0xc0008000\n");
        fprintf(stderr, "Access to unknown coprocessor attempted (%d)\n",
          inst.crt.cpn);
        fprintf(stderr, "PC=%.8x\n", reg->r[15]);
        #ifdef EMULART
        debug_command(machine, cmd);
        debug_command(machine, cmd2);
        #endif
        free(cmd);
        free(cmd2);
        abort();
      }
      break;
    }
    INCPC;
  }
  return 0;
}

int EXECUTEFN(exec_sds)(machineinfo* machine, instructionformat inst,
  void* null)
{
  IGNORE(null);

  if (!EXECUTEFN(exec_condition)(machine, inst)) return 0;
  // unimplemented
  fprintf(stderr, "Error: unimplemented instruction (sds)\n");
  exit(1);
  return 0;
}

int EXECUTEFN(exec_und)(machineinfo* machine, instructionformat inst,
  void* null)
{
  IGNORE(null);

  if (!EXECUTEFN(exec_condition)(machine, inst)) return 0;
  // unimplemented
  fprintf(stderr, "Warning: undefined instruction\n");
  processor_und(machine);
  return 1;
}

#ifdef ARMTHUMB

int EXECUTEFN(exec_thumbl)(machineinfo* machine, instructionformat inst,
  void* null)
{
  registerinfo* reg = machine->reg;

  IGNORE(null);

  if (inst.instruction & 0x0100)
  {
    // part 1, latch low bits
    reg->r[14] = PCADDR + ((inst.instruction & 0x07ff)<<12);
    return 0;
  }
  else
  {
    uint5 oldpc = reg->r[15];
    sint5 newaddr = reg->r[14] + ((inst.instruction & 0x07ff)<<1);
    newaddr = (newaddr<<8)>>8;
    reg->r[15] += newaddr;
    reg->r[14] = oldpc-2;
    return 1;
  }
}

#endif
