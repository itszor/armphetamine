// backend disassembler functions for instruction decoder

#include <stdio.h>
#include <stdlib.h>

#include "libjtype/defs.h"
#include "core/machine.h"
#include "core/decode.h"
#include "core/execute.h"

static const char* txtcc[]={"eq","ne","cs","cc","mi","pl","vs","vc",
                            "hi","ls","ge","lt","gt","le","","nv"};

static const char* txtops[]={"and","eor","sub","rsb","add","adc","sbc","rsc",
                            "tst","teq","cmp","cmn","orr","mov","bic","mvn"};

static const char ignoreop1[] = {0,0,0,0,0,0,0,0,1,1,1,1,0,1,0,1};
static const char omitsflag[] = {0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0};

static const char* txtshft[] = {"lsl","lsr","asr","ror","rrx"};

// disassemble a data-processing instruction (register+shift op2)
int diss_dp(machineinfo* machine, instructionformat inst, void* null)
{
  uint5 rm = inst.dp.operand2 & 15;
  uint5 temp = inst.dp.operand2, shifttype = (temp>>5)&3;
  uint5 amount = temp&16 ? temp>>8 : temp>>7;
  uint5 unopreg = omitsflag[inst.dp.opcode] ? inst.dp.rn : inst.dp.rd;

  IGNORE(machine);
  IGNORE(null);

  if (omitsflag[inst.dp.opcode] && !inst.dp.s)
  {
    // it's a PSR transfer
    if (inst.mrs.ident==0 && inst.mrs.ident2==15)
    {
      // mrs rather than msr
      fprintf(stderr, "mrs%s r%d,%s", txtcc[inst.generic.cond], inst.mrs.rd,
        inst.mrs.ps ? "spsr" : "cpsr");
    }
    else
    {
      if (((inst.msr.ident>>8)&0x20f)==0x20f)
      {
        // register contents to whole PSR
        fprintf(stderr, "msr%s %s%s%s%s%s,r%d", txtcc[inst.generic.cond], 
          inst.msr.pd ? "spsr_" : "cpsr_",
          (inst.msrf.ident&0x80)?"f":"", (inst.msrf.ident&0x40)?"s":"",
          (inst.msrf.ident&0x20)?"x":"", (inst.msrf.ident&0x10)?"c":"",
          inst.msr.rm);
      }
      else
      {
        fprintf(stderr, "Bad MSR, panic, panic\n");
        abort();
      }
    }
  }
  else
  {
    if (temp & 16) // shift by register
    {
      if (ignoreop1[inst.dp.opcode])
      {
        if (inst.dp.rn==15)
        {
          fprintf(stderr, "%s%s%sp r%d,r%d,%s r%d", txtops[inst.dp.opcode],
            txtcc[inst.generic.cond], inst.dp.s && !omitsflag[inst.dp.opcode]
            ? "s" : "", unopreg, rm, txtshft[shifttype], amount);
        }
        else
        {
          fprintf(stderr, "%s%s%s r%d,r%d,%s r%d", txtops[inst.dp.opcode],
            txtcc[inst.generic.cond], inst.dp.s && !omitsflag[inst.dp.opcode]
            ? "s" : "", unopreg, rm, txtshft[shifttype], amount);
        }
      }
      else
        fprintf(stderr, "%s%s%s r%d,r%d,r%d,%s r%d", txtops[inst.dp.opcode],
          txtcc[inst.generic.cond], inst.dp.s && !omitsflag[inst.dp.opcode]
          ? "s" : "", inst.dp.rd, inst.dp.rn, rm, txtshft[shifttype], amount);
    }
    else  // shift by immediate
    {
      char shiftstr[30];
      if (shifttype==3 && !amount) shifttype++;

      if (shifttype==0 && amount==0)
        shiftstr[0] = 0;
      else
        sprintf(shiftstr, ",%s #%d", txtshft[shifttype], amount);
        
      if (ignoreop1[inst.dp.opcode])
      {
        if (inst.dp.rn==15)
        {
          fprintf(stderr, "%s%s%sp r%d,r%d%s", txtops[inst.dp.opcode],
            txtcc[inst.generic.cond], inst.dp.s && !omitsflag[inst.dp.opcode]
            ? "s" : "", unopreg, rm, shiftstr);
      
        }
        else
        {
          fprintf(stderr, "%s%s%s r%d,r%d%s", txtops[inst.dp.opcode],
            txtcc[inst.generic.cond], inst.dp.s && !omitsflag[inst.dp.opcode]
            ? "s" : "", unopreg, rm, shiftstr);
        }
      }
      else
        fprintf(stderr, "%s%s%s r%d,r%d,r%d%s", txtops[inst.dp.opcode],
          txtcc[inst.generic.cond], inst.dp.s && !omitsflag[inst.dp.opcode]
          ? "s" : "", inst.dp.rd, inst.dp.rn, rm, shiftstr);
    }
  }
  return 0;
}

int diss_dp_imm(machineinfo* machine, instructionformat inst, void* null)
{
  uint5 imm = inst.dp.operand2 & 0xff, rot = 2*(inst.dp.operand2>>8);
  uint5 unopreg = omitsflag[inst.dp.opcode] ? inst.dp.rn : inst.dp.rd;

  IGNORE(machine);
  IGNORE(null);

  if (omitsflag[inst.dp.opcode] && !inst.dp.s
      && ((inst.instruction & 0x0d028f00)==0x01028f00))
  {
    // PSR flags transfer from immediate operand
    fprintf(stderr, "msr%s %s%s%s%s%s,#%d", txtcc[inst.generic.cond], 
      inst.msrf.pd ? "spsr_" : "cpsr_",
      (inst.msrf.ident&0x80)?"f":"", (inst.msrf.ident&0x40)?"s":"",
      (inst.msrf.ident&0x20)?"x":"", (inst.msrf.ident&0x10)?"c":"",
      imm);
  }
  else
  {
    if (ignoreop1[inst.dp.opcode])
     fprintf(stderr, "%s%s%s r%d,#%d", txtops[inst.dp.opcode],
       txtcc[inst.generic.cond], inst.dp.s && !omitsflag[inst.dp.opcode]
       ? "s" : "", unopreg, ROR(imm, rot));
    else
     fprintf(stderr, "%s%s%s r%d,r%d,#%d", txtops[inst.dp.opcode],
       txtcc[inst.generic.cond], inst.dp.s && !omitsflag[inst.dp.opcode]
       ? "s" : "", inst.dp.rd, inst.dp.rn, ROR(imm, rot));
  }
  return 0;
}

int diss_mul(machineinfo* machine, instructionformat inst, void* null)
{
  IGNORE(machine);
  IGNORE(null);

  if (inst.mul.a)
    fprintf(stderr, "mla%s%s r%d,r%d,r%d,r%d", txtcc[inst.generic.cond],
      inst.mul.s ? "s" : "", inst.mul.rd, inst.mul.rm, inst.mul.rs,
      inst.mul.rn);
  else
    fprintf(stderr, "mul%s%s r%d,r%d,r%d", txtcc[inst.generic.cond],
      inst.mul.s ? "s" : "", inst.mul.rd, inst.mul.rm, inst.mul.rs);
  return 0;
}

int diss_mull(machineinfo* machine, instructionformat inst, void* null)
{
  char prefix = (inst.mull.u) ? 's' : 'u';
  
  IGNORE(machine);
  IGNORE(null);
  
  if (inst.mull.a)
    fprintf(stderr, "%cmlal%s%s r%d,r%d,r%d,r%d", prefix, 
      txtcc[inst.generic.cond], inst.mull.s ? "s" : "", inst.mull.rdlo, 
      inst.mull.rdhi, inst.mull.rm, inst.mull.rs);
  else
    fprintf(stderr, "%cmull%s%s r%d,r%d,r%d,r%d", prefix, 
      txtcc[inst.generic.cond], inst.mull.s ? "s" : "", inst.mull.rdlo, 
      inst.mull.rdhi, inst.mull.rm, inst.mull.rs);
  return 0;
}

int diss_sdt(machineinfo* machine, instructionformat inst, void* null)
{
  IGNORE(machine);
  IGNORE(null);

  if (inst.sdt.i)  // offset=register+shift
  {
    uint5 rm = inst.sdt.offset & 15, temp = inst.dp.operand2;
    uint5 shifttype = (temp>>5)&3, amount = temp&16 ? temp>>8 : temp>>7;
    if (inst.sdt.p) // pre-indexed
      fprintf(stderr, "%s%s%s r%d,[r%d,r%d,%s #%d]%s", inst.sdt.l ? "ldr" : "str",
        txtcc[inst.generic.cond], inst.sdt.b ? "b" : "", inst.sdt.rd,
        inst.sdt.rn, rm, txtshft[shifttype], amount,
        inst.sdt.w ? "!" : "");
    else
      fprintf(stderr, "%s%s%s r%d,[r%d],r%d,%s #%d", inst.sdt.l ? "ldr" : "str",
        txtcc[inst.generic.cond], inst.sdt.b ? "b" : "", inst.sdt.rd,
        inst.sdt.rn, rm, txtshft[shifttype], amount);
  }
  else
  {
    if (inst.sdt.p) // pre-indexed
      fprintf(stderr, "%s%s%s r%d,[r%d,#%s%d]%s", inst.sdt.l ? "ldr" : "str",
        txtcc[inst.generic.cond], inst.sdt.b ? "b" : "", inst.sdt.rd,
        inst.sdt.rn, inst.sdt.u ? "" : "-", inst.sdt.offset,
        inst.sdt.w ? "!" : "");
    else
      fprintf(stderr, "%s%s%s r%d,[r%d],#%s%d", inst.sdt.l ? "ldr" : "str",
        txtcc[inst.generic.cond], inst.sdt.b ? "b" : "", inst.sdt.rd,
        inst.sdt.rn, inst.sdt.u ? "" : "-", inst.sdt.offset);
  }
  return 0;
}

int diss_sdth(machineinfo* machine, instructionformat inst, void* null)
{
  IGNORE(machine);
  IGNORE(null);

  if (inst.sdth.imm)
  {
    uint5 offset = (inst.instruction & 0xf) |
                   ((inst.instruction & 0xf00) >> 4);
    if (inst.sdth.p)
      fprintf(stderr, "%s%s%s%s r%d,[r%d,#%d]%s", inst.sdth.l ? "ldr" : "str",
        txtcc[inst.generic.cond], inst.sdth.s ? "s" : "",
        inst.sdth.h ? "h" : "b",
        inst.sdth.rd, inst.sdth.rn, inst.sdth.u ? offset : -offset,
        inst.sdth.w ? "!" : "");
    else
      fprintf(stderr, "%s%s%s%s r%d,[r%d],#%d", inst.sdth.l ? "ldr" : "str",
        txtcc[inst.generic.cond], inst.sdth.s ? "s" : "",
        inst.sdth.h ? "h" : "b",
        inst.sdth.rd, inst.sdth.rn, inst.sdth.u ? offset : -offset);
  }
  else
  {
    if (inst.sdth.p)
      fprintf(stderr, "%s%s%s%s r%d,[r%d,%sr%d]%s", inst.sdth.l ? "ldr" : "str",
        txtcc[inst.generic.cond], inst.sdth.s ? "s" : "",
        inst.sdth.h ? "h" : "b",
        inst.sdth.rd, inst.sdth.rn, inst.sdth.u ? "" : "-", inst.sdth.rm, 
        inst.sdth.w ? "!" : "");
    else
      fprintf(stderr, "%s%s%s%s r%d,[r%d],%sr%d", inst.sdth.l ? "ldr" : "str",
        txtcc[inst.generic.cond], inst.sdth.s ? "s" : "",
        inst.sdth.h ? "h" : "b",
        inst.sdth.rd, inst.sdth.rn, inst.sdth.u ? "" : "-", inst.sdth.rm);
  }
  return 0;
}

int diss_bdt(machineinfo* machine, instructionformat inst, void* null)
{
  int reg=0, start=-1, runlength=0, first=1;

  IGNORE(machine);
  IGNORE(null);

  fprintf(stderr, "%s%s%c%c r%d%s,{", inst.bdt.l ? "ldm" : "stm",
    txtcc[inst.generic.cond], inst.bdt.u ? 'i' : 'd', inst.bdt.p ? 'b' : 'a',
    inst.bdt.rn, inst.bdt.w ? "!" : "");

  // not very pretty. (using 'r16' as an always-zero sentry...)
  for (reg=0; reg<17; reg++)
  {
    if (inst.bdt.reglist&(1<<reg) && start==-1) start=reg, runlength=0;
      else runlength++;
    if (!(inst.bdt.reglist&(1<<reg)) && start!=-1)
    {
      runlength--;
      if (runlength==0) fprintf(stderr, "%sr%d", first ? "" : ",", start);
        else fprintf(stderr, "%sr%d-r%d", first ? "" : ",", start, start+runlength);
      start=-1;
      first=0;
    }
  }
  fprintf(stderr, "}%s", inst.bdt.s ? "^" : "");
  return 0;
}

int diss_bra(machineinfo* machine, instructionformat inst, void* null)
{
  sint5 offset = (sint5)((inst.bra.offset+2)<<8)>>6;
  
  IGNORE(machine);
  
  fprintf(stderr, "b%s%s 0x%x\t\t; pc%s%d", inst.bra.l ? "l" : "", 
    txtcc[inst.generic.cond], (uint5)null+offset,
    offset>=0 ? "+" : "", offset);
  return 0;
}

int diss_swi(machineinfo* machine, instructionformat inst, void* null)
{
  IGNORE(machine);
  IGNORE(null);

  fprintf(stderr, "swi%s 0x%x", txtcc[inst.generic.cond], inst.swi.number);
  return 0;
}

int diss_cdt(machineinfo* machine, instructionformat inst, void* null)
{
  IGNORE(machine);
  IGNORE(inst);
  IGNORE(null);

  fprintf(stderr, "coprocessor data transfer");
  return 0;
}

int diss_cdo(machineinfo* machine, instructionformat inst, void* null)
{
  IGNORE(machine);
  IGNORE(null);

  fprintf(stderr, "cdp%s %d,%d,c%d,c%d,c%d,%d", txtcc[inst.generic.cond],
    inst.cdo.cpn, inst.cdo.cpopc, inst.cdo.crd, inst.cdo.crn, inst.cdo.crm,
    inst.cdo.cp);
  return 0;
}

int diss_crt(machineinfo* machine, instructionformat inst, void* null)
{
  IGNORE(machine);
  IGNORE(null);

  if (inst.crt.l)  // mrc
  {
    fprintf(stderr, "mrc%s cp%d,%d,r%d,cr%d,cr%d,{%d}",
      txtcc[inst.generic.cond], inst.crt.cpn, inst.crt.cpopc,
      inst.crt.rd, inst.crt.crn, inst.crt.crm, inst.crt.cp);
  }
  else  // mcr
  {
    fprintf(stderr, "mcr%s cp%d,%d,r%d,cr%d,cr%d,{%d}",
      txtcc[inst.generic.cond], inst.crt.cpn, inst.crt.cpopc,
      inst.crt.rd, inst.crt.crn, inst.crt.crm, inst.crt.cp);
  }
  return 0;
}

int diss_sds(machineinfo* machine, instructionformat inst, void* null)
{
  IGNORE(machine);
  IGNORE(null);

  fprintf(stderr, "swp%s%s r%d,r%d,[r%d]", txtcc[inst.generic.cond], inst.sds.b ? "b"
    : "", inst.sds.rd, inst.sds.rm, inst.sds.rn);
  return 0;
}

int diss_und(machineinfo* machine, instructionformat inst, void* null)
{
  IGNORE(machine);
  IGNORE(inst);
  IGNORE(null);

  fprintf(stderr, "undefined instruction");
  return 0;
}

void diss_around_pc(machineinfo* machine)
{
  int i;
  instructionformat inst;
  registerinfo* reg = machine->reg;
  meminfo* mem = machine->mem;
  uint5 instaddr = PCADDR-8;

  fprintf(stderr, "Disassembling around %.8x\n", instaddr);

  for (i=-32; i<32; i+=4)
  {
    inst.instruction = memory_readinstword(mem, instaddr+i);
    fprintf(stderr, "+ %.8x : %.8x : ", instaddr+i, inst.instruction);
    dispatch(machine, inst, &diss, (void*)(instaddr+i));
    fprintf(stderr, "\n");
  }

#define LOCALFLAG(X) \
((mem->currentmode<16) ? (((pcinfo*)&reg->r[15])->flag.X) \
                     : (reg->cpsr.flag.X))

  for (i=0; i<15; i++)
  {
    fprintf(stderr, "r%d=%.8x ", i, reg->r[i]);
  }
  fprintf(stderr, "cpsr=%.8x spsr[%d]=%.8x ",
    reg->cpsr.value, reg->spsr_current, 
    reg->spsr[reg->spsr_current].value);
  fprintf(stderr, "%c%c%c%c\n", LOCALFLAG(c)?'C':'c', 
          LOCALFLAG(v)?'V':'v', LOCALFLAG(n)?'N':'n', 
          LOCALFLAG(z)?'Z':'z');
}
