#include <stdio.h>
#include <stddef.h>
#include <assert.h>

#include "genx86.h"
#include "x86asm.h"
#include "x86dism.h"
#include "rtasm.h"
#include "pheta.h"
#include "palloc.h"
#include "pseudo.h"
#include "phetadism.h"
#include "relocate.h"
#include "cnew.h"
#include "genx86_tab.h"
#include "dynsupport.h"

static const char* allocname[] = {
  "unset", "constb", "const", "rfile", "ireg",
  "stack", "alias", "split"
};

static const char* abname[] = {
  "shl", "shr", "sar", "ror", "rol", "rcr", "rcl", "and", "or", "xor", "add", 
  "adc", "sub", "sbb", "imul", "lea", "mov", "not", "push", "pop", 
  "test", "cmp", "ret", "seto", "setno", "setb", "setae", "sete", "setne", 
  "setbe", "seta", "sets", "setns", "setl", "setge", "setle", "setg", "jo",
  "jno", "jb", "jae", "je", "jne", "jbe", "ja", "js", "jns", "jl", "jge", "jle",
  "jg", "call", "jecxz", "bt", "cmc", "stc", "clc", "nop"
};

/*
palloc_info* genx86_closesplit(palloc_info* a, uint5 line)
{
  if (a->type==pal_SPLIT)
  {
    palloc_splitalloc* alloc = (palloc_splitalloc*) a->info.extra;
    return genx86_closesplit(alloc, line >= alloc->splitline ? &alloc->lower
                                                             : &alloc->upper);
  } 
  else
    return a;
}
*/

static const char* otype[] = { "empty", "partial", "immediate", "register",
  "indirect register", "ind register+8bit disp", "ind register+32bit disp",
  "indirect base+offset", "indirect base+offset+disp8",
  "indirect base+offset+disp32", "address" };

#define COMPOUND2(A,B) ((A)*gotype_NUMTYPES+(B))

#define COMPOUND3(A,B,C) \
  ((A)*gotype_NUMTYPES*gotype_NUMTYPES+(B)*gotype_NUMTYPES+(C))

#define SIZEDTYPE(T,S) ((T)*gowidth_NUMWIDTHS+(S))

#define ERR { fprintf(stderr, "Broke at line %d\n" \
  "Operator=%s op[0]=%s op[1]=%s op[2]=%s\n", __LINE__, abname[inst->operator],\
   inst->op[0] ? otype[inst->op[0]->type] : "(nil)", \
   inst->op[1] ? otype[inst->op[1]->type] : "(nil)", \
   inst->op[2] ? otype[inst->op[2]->type] : "(nil)"); abort(); }

static void genx86_asm_r32_r32_3(nativeblockinfo* nat, genx86_op* inst,
  uint5 opcode, uint5 ops, uint5 reg0, uint5 reg1)
{
  switch SIZEDTYPE(inst->op[2]->type, inst->op[2]->width)
  {
    case SIZEDTYPE(gotype_REGISTER, gowidth_DWORD):
    {
      if (opcode==ab_LEA && genx86_tab[opcode].r32_rm32)
      {
        genx86_tab[opcode].r32_rm32(nat, reg0, rtasm_scind(reg1,
          inst->op[2]->data.reg, scale_1));
      }
      else ERR;
    }
    break;
    
    default:
    ERR;
  }
}

static void genx86_asm_r32_2(nativeblockinfo* nat, genx86_op* inst,
  uint5 opcode, uint5 ops, uint5 reg)
{
  rtasm_mtype arg2_mtype;

  switch SIZEDTYPE(inst->op[1]->type, inst->op[1]->width)
  {
    case SIZEDTYPE(gotype_IMMEDIATE, gowidth_BYTE):
    {
      if (ops==2)
      {
        if (genx86_tab[opcode].rm32_i8)
        {
          genx86_tab[opcode].rm32_i8(nat, rtasm_reg(reg), 
            inst->op[1]->data.imm);
        }
        else if (genx86_tab[opcode].rm32_i32)
        {
          genx86_tab[opcode].rm32_i32(nat, rtasm_reg(reg), 
            inst->op[1]->data.imm);
        }
        else ERR;
      }
      else ERR;
    }
    break;
  
    case SIZEDTYPE(gotype_IMMEDIATE, gowidth_DWORD):
    {
      if (ops==2)
      {
        if (genx86_tab[opcode].r32_i32)
        {
          genx86_tab[opcode].r32_i32(nat, reg, inst->op[1]->data.imm);
        }
        else if (genx86_tab[opcode].rm32_i32)
        {
          genx86_tab[opcode].rm32_i32(nat, rtasm_reg(reg), 
            inst->op[1]->data.imm);
        }
        else ERR;
      }
      else ERR;
    }
    break;
    
    case SIZEDTYPE(gotype_REGISTER, gowidth_DWORD):
    {
      if (ops==2)
      {
        if (genx86_tab[opcode].r32_rm32)
        {
          genx86_tab[opcode].r32_rm32(nat, reg, 
            rtasm_reg(inst->op[1]->data.reg));
        }
        else if (genx86_tab[opcode].rm32_r32)
        {
          genx86_tab[opcode].rm32_r32(nat, rtasm_reg(reg), 
            inst->op[1]->data.reg);
        }
        else ERR;
      }
      else
      {
        genx86_asm_r32_r32_3(nat, inst, opcode, ops, reg,
          inst->op[1]->data.reg);
      }
    }
    break;
    
    case SIZEDTYPE(gotype_INDREG, gowidth_DWORD):
    arg2_mtype = rtasm_ind(inst->op[1]->data.reg);
    goto r32_m32_wotsit;
    
    case SIZEDTYPE(gotype_INDREGPLUSDISP8, gowidth_DWORD):
    arg2_mtype = rtasm_ind8(inst->op[1]->data.regdisp.base,
                            inst->op[1]->data.regdisp.disp);
    goto r32_m32_wotsit;

    case SIZEDTYPE(gotype_INDREGPLUSDISP32, gowidth_DWORD):
    arg2_mtype = rtasm_ind32(inst->op[1]->data.regdisp.base,
                             inst->op[1]->data.regdisp.disp);
    goto r32_m32_wotsit;
    
    case SIZEDTYPE(gotype_INDREGPLUSSCALEDREG, gowidth_DWORD):
    arg2_mtype = rtasm_scind(inst->op[1]->data.regscale.base,
                             inst->op[1]->data.regscale.index,
                             inst->op[1]->data.regscale.scale);
    goto r32_m32_wotsit;

    case SIZEDTYPE(gotype_INDREGPLUSSCALEDREGPLUSDISP8, gowidth_DWORD):
    arg2_mtype = rtasm_scind8(inst->op[1]->data.regscaledisp.base,
                              inst->op[1]->data.regscaledisp.index,
                              inst->op[1]->data.regscaledisp.scale,
                              inst->op[1]->data.regscaledisp.offset);
    goto r32_m32_wotsit;
    
    case SIZEDTYPE(gotype_INDREGPLUSSCALEDREGPLUSDISP32, gowidth_DWORD):
    arg2_mtype = rtasm_scind32(inst->op[1]->data.regscaledisp.base,
                               inst->op[1]->data.regscaledisp.index,
                               inst->op[1]->data.regscaledisp.scale,
                               inst->op[1]->data.regscaledisp.offset);
    goto r32_m32_wotsit;
    
    case SIZEDTYPE(gotype_ADDRESS, gowidth_DWORD):
    arg2_mtype = rtasm_addr(inst->op[1]->data.addr);

    r32_m32_wotsit:
    
    if (ops==2)
    {
      if (genx86_tab[opcode].r32_rm32)
      {
        genx86_tab[opcode].r32_rm32(nat, reg, arg2_mtype);
      }
      else ERR;
    }
    else ERR;
    break;
    
    default:
    ERR;
  }
}

static void genx86_asm_m32_2(nativeblockinfo* nat, genx86_op* inst,
  uint5 opcode, uint5 ops, rtasm_mtype* arg1)
{
  switch SIZEDTYPE(inst->op[1]->type, inst->op[1]->width)
  {
    case SIZEDTYPE(gotype_IMMEDIATE, gowidth_BYTE):
    {
      if (ops==2)
      {
        if (genx86_tab[opcode].rm32_i8)
        {
          genx86_tab[opcode].rm32_i8(nat, *arg1, inst->op[1]->data.imm);
        }
      }
      else ERR;
    }
    break;
  
    case SIZEDTYPE(gotype_IMMEDIATE, gowidth_DWORD):
    {
      if (ops==2)
      {
        if (genx86_tab[opcode].rm32_i32)
        {
          genx86_tab[opcode].rm32_i32(nat, *arg1, inst->op[1]->data.imm);
        }
        else ERR;
      }
      else ERR;
    }
    break;
    
    case SIZEDTYPE(gotype_REGISTER, gowidth_DWORD):
    {
      if (ops==2)
      {
        if (genx86_tab[opcode].rm32_r32)
        {
          genx86_tab[opcode].rm32_r32(nat, *arg1, inst->op[1]->data.reg);
        }
        else ERR;
      }
      else ERR;
    }
    break;
        
    default:
    ERR;
  }
}

static void genx86_asm_r8_2(nativeblockinfo* nat, genx86_op* inst, uint5 opcode,
  uint5 ops, uint5 reg)
{
  rtasm_mtype arg2_mtype;

  switch SIZEDTYPE(inst->op[1]->type, inst->op[1]->width)
  {
    case SIZEDTYPE(gotype_IMMEDIATE, gowidth_BYTE):
    {
      if (ops==2)
      {
        if (genx86_tab[opcode].r8_i8)
        {
          genx86_tab[opcode].r8_i8(nat, reg, inst->op[1]->data.imm);
        }
        else if (genx86_tab[opcode].rm8_i8)
        {
          genx86_tab[opcode].rm8_i8(nat, rtasm_reg(reg), inst->op[1]->data.imm);
        }
        else ERR;
      }
      else ERR;
    }
    break;
    
    case SIZEDTYPE(gotype_REGISTER, gowidth_BYTE):
    {
      if (ops==2)
      {
        if (genx86_tab[opcode].r8_rm8)
        {
          genx86_tab[opcode].r8_rm8(nat, reg, rtasm_reg(inst->op[1]->data.reg));
        }
        else if (genx86_tab[opcode].rm8_r8)
        {
          genx86_tab[opcode].rm8_r8(nat, rtasm_reg(reg), inst->op[1]->data.reg);
        }
        else ERR;
      }
      else ERR;
    }
    break;
    
    case SIZEDTYPE(gotype_INDREG, gowidth_BYTE):
    arg2_mtype = rtasm_ind(inst->op[1]->data.reg);
    goto r8_m8_wotsit;
    
    case SIZEDTYPE(gotype_INDREGPLUSDISP8, gowidth_BYTE):
    arg2_mtype = rtasm_ind8(inst->op[1]->data.regdisp.base,
                            inst->op[1]->data.regdisp.disp);
    goto r8_m8_wotsit;

    case SIZEDTYPE(gotype_INDREGPLUSDISP32, gowidth_BYTE):
    arg2_mtype = rtasm_ind32(inst->op[1]->data.regdisp.base,
                             inst->op[1]->data.regdisp.disp);
    goto r8_m8_wotsit;
    
    case SIZEDTYPE(gotype_INDREGPLUSSCALEDREG, gowidth_BYTE):
    arg2_mtype = rtasm_scind(inst->op[1]->data.regscale.base,
                             inst->op[1]->data.regscale.index,
                             inst->op[1]->data.regscale.scale);
    goto r8_m8_wotsit;

    case SIZEDTYPE(gotype_INDREGPLUSSCALEDREGPLUSDISP8, gowidth_BYTE):
    arg2_mtype = rtasm_scind8(inst->op[1]->data.regscaledisp.base,
                              inst->op[1]->data.regscaledisp.index,
                              inst->op[1]->data.regscaledisp.scale,
                              inst->op[1]->data.regscaledisp.offset);
    goto r8_m8_wotsit;
    
    case SIZEDTYPE(gotype_INDREGPLUSSCALEDREGPLUSDISP32, gowidth_BYTE):
    arg2_mtype = rtasm_scind32(inst->op[1]->data.regscaledisp.base,
                               inst->op[1]->data.regscaledisp.index,
                               inst->op[1]->data.regscaledisp.scale,
                               inst->op[1]->data.regscaledisp.offset);
    goto r8_m8_wotsit;
    
    case SIZEDTYPE(gotype_ADDRESS, gowidth_BYTE):
    arg2_mtype = rtasm_addr(inst->op[1]->data.addr);

    r8_m8_wotsit:
    
    if (ops==2)
    {
      if (genx86_tab[opcode].r8_rm8)
      {
        genx86_tab[opcode].r8_rm8(nat, reg, arg2_mtype);
      }
      else ERR;
    }
    else ERR;
    break;
    
    default:
    ERR;
  }
}

static void genx86_asm_m8_2(nativeblockinfo* nat, genx86_op* inst, uint5 opcode,   uint5 ops, rtasm_mtype* arg1)
{
  switch SIZEDTYPE(inst->op[1]->type, inst->op[1]->width)
  {
    case SIZEDTYPE(gotype_IMMEDIATE, gowidth_BYTE):
    {
      if (ops==2)
      {
        if (genx86_tab[opcode].rm8_i8)
        {
          genx86_tab[opcode].rm8_i8(nat, *arg1, inst->op[1]->data.imm);
        }
        else ERR;
      }
      else ERR;
    }
    break;
    
    case SIZEDTYPE(gotype_REGISTER, gowidth_BYTE):
    {
      if (ops==2)
      {
        if (genx86_tab[opcode].rm8_r8)
        {
          genx86_tab[opcode].rm8_r8(nat, *arg1, inst->op[1]->data.reg);
        }
        else ERR;
      }
      else ERR;
    }
    break;
        
    default:
    ERR;
  }
}

static void genx86_asm_1(nativeblockinfo* nat, genx86_op* inst, uint5 opcode, 
  uint5 ops)
{
  rtasm_mtype arg1_mtype;

  switch (SIZEDTYPE(inst->op[0]->type, inst->op[0]->width))
  {
    case SIZEDTYPE(gotype_IMMEDIATE, gowidth_BYTE):
    {
      if (genx86_tab[opcode].i8)
      {
        genx86_tab[opcode].i8(nat, inst->op[0]->data.imm);
      }
      else if (genx86_tab[opcode].i32)
      {
        genx86_tab[opcode].i32(nat, inst->op[0]->data.imm);
      }
      else ERR;
    }
    break;
    
    case SIZEDTYPE(gotype_IMMEDIATE, gowidth_DWORD):
    {
      if (genx86_tab[opcode].i32)
      {
        genx86_tab[opcode].i32(nat, inst->op[0]->data.imm);
      }
      else ERR;
    }
    break;
    
    case SIZEDTYPE(gotype_REGISTER, gowidth_BYTE):
    {
      if (ops==1)
      {
        if (genx86_tab[opcode].rm8)
        {
          genx86_tab[opcode].rm8(nat, rtasm_reg(inst->op[0]->data.reg));
        }
        else ERR;
      }
      else genx86_asm_r8_2(nat, inst, opcode, ops, inst->op[0]->data.reg);
    }
    break;

    case SIZEDTYPE(gotype_REGISTER, gowidth_DWORD):
    {
      if (ops==1)
      {
        if (genx86_tab[opcode].rm32)
        {
          genx86_tab[opcode].rm32(nat, rtasm_reg(inst->op[0]->data.reg));
        }
        else ERR;
      }
      else genx86_asm_r32_2(nat, inst, opcode, ops, inst->op[0]->data.reg);
    }
    break;

    case SIZEDTYPE(gotype_INDREG, gowidth_BYTE):
    arg1_mtype = rtasm_ind(inst->op[0]->data.reg);
    goto rm8_wotsit;
    
    case SIZEDTYPE(gotype_INDREGPLUSDISP8, gowidth_BYTE):
    arg1_mtype = rtasm_ind8(inst->op[0]->data.regdisp.base,
                            inst->op[0]->data.regdisp.disp);
    goto rm8_wotsit;

    case SIZEDTYPE(gotype_INDREGPLUSDISP32, gowidth_BYTE):
    arg1_mtype = rtasm_ind32(inst->op[0]->data.regdisp.base,
                             inst->op[0]->data.regdisp.disp);
    goto rm8_wotsit;
    
    case SIZEDTYPE(gotype_INDREGPLUSSCALEDREG, gowidth_BYTE):
    arg1_mtype = rtasm_scind(inst->op[0]->data.regscale.base,
                             inst->op[0]->data.regscale.index,
                             inst->op[0]->data.regscale.scale);
    goto rm8_wotsit;

    case SIZEDTYPE(gotype_INDREGPLUSSCALEDREGPLUSDISP8, gowidth_BYTE):
    arg1_mtype = rtasm_scind8(inst->op[0]->data.regscaledisp.base,
                              inst->op[0]->data.regscaledisp.index,
                              inst->op[0]->data.regscaledisp.scale,
                              inst->op[0]->data.regscaledisp.offset);
    goto rm8_wotsit;
    
    case SIZEDTYPE(gotype_INDREGPLUSSCALEDREGPLUSDISP32, gowidth_BYTE):
    arg1_mtype = rtasm_scind32(inst->op[0]->data.regscaledisp.base,
                               inst->op[0]->data.regscaledisp.index,
                               inst->op[0]->data.regscaledisp.scale,
                               inst->op[0]->data.regscaledisp.offset);
    goto rm8_wotsit;
    
    case SIZEDTYPE(gotype_ADDRESS, gowidth_BYTE):
    arg1_mtype = rtasm_addr(inst->op[0]->data.addr);

    rm8_wotsit:

    if (ops==1)
    {
      if (genx86_tab[opcode].rm8)
      {
        genx86_tab[opcode].rm8(nat, arg1_mtype);
      }
      else ERR;
    }
    else genx86_asm_m8_2(nat, inst, opcode, ops, &arg1_mtype);
    break;

    case SIZEDTYPE(gotype_INDREG, gowidth_DWORD):
    arg1_mtype = rtasm_ind(inst->op[0]->data.reg);
    goto rm32_wotsit;
    
    case SIZEDTYPE(gotype_INDREGPLUSDISP8, gowidth_DWORD):
    arg1_mtype = rtasm_ind8(inst->op[0]->data.regdisp.base,
                            inst->op[0]->data.regdisp.disp);
    goto rm32_wotsit;

    case SIZEDTYPE(gotype_INDREGPLUSDISP32, gowidth_DWORD):
    arg1_mtype = rtasm_ind32(inst->op[0]->data.regdisp.base,
                             inst->op[0]->data.regdisp.disp);
    goto rm32_wotsit;
    
    case SIZEDTYPE(gotype_INDREGPLUSSCALEDREG, gowidth_DWORD):
    arg1_mtype = rtasm_scind(inst->op[0]->data.regscale.base,
                             inst->op[0]->data.regscale.index,
                             inst->op[0]->data.regscale.scale);
    goto rm32_wotsit;

    case SIZEDTYPE(gotype_INDREGPLUSSCALEDREGPLUSDISP8, gowidth_DWORD):
    arg1_mtype = rtasm_scind8(inst->op[0]->data.regscaledisp.base,
                              inst->op[0]->data.regscaledisp.index,
                              inst->op[0]->data.regscaledisp.scale,
                              inst->op[0]->data.regscaledisp.offset);
    goto rm32_wotsit;
    
    case SIZEDTYPE(gotype_INDREGPLUSSCALEDREGPLUSDISP32, gowidth_DWORD):
    arg1_mtype = rtasm_scind32(inst->op[0]->data.regscaledisp.base,
                               inst->op[0]->data.regscaledisp.index,
                               inst->op[0]->data.regscaledisp.scale,
                               inst->op[0]->data.regscaledisp.offset);
    goto rm32_wotsit;
    
    case SIZEDTYPE(gotype_ADDRESS, gowidth_DWORD):
    arg1_mtype = rtasm_addr(inst->op[0]->data.addr);

    rm32_wotsit:

    if (ops==1)
    {
      if (genx86_tab[opcode].rm32)
      {
        genx86_tab[opcode].rm32(nat, arg1_mtype);
      }
      else ERR;
    }
    else genx86_asm_m32_2(nat, inst, opcode, ops, &arg1_mtype);
    break;

    default:
    ERR;
  }
}

static void genx86_asm_0(nativeblockinfo* nat, genx86_op* inst, uint5 opcode, 
  uint5 ops)
{
  if (ops==0)
  {
    if (genx86_tab[opcode].narg)
    {
      genx86_tab[opcode].narg(nat);
    }
  }
  else genx86_asm_1(nat, inst, opcode, ops);
}

void genx86_asm(nativeblockinfo* nat, genx86_op* inst)
{
  uint5 ops = 0, opcode = inst->operator;

  if (inst->op[0]) ops++;
  if (inst->op[1]) ops++;
  if (inst->op[2]) ops++;

  genx86_asm_0(nat, inst, opcode, ops);
}

#undef COMPOUND2
#undef COMPOUND3
#undef ERR

void genx86_translatealloc(pheta_chunk* chunk, genx86_operand* dest,
                           palloc_info* src)
{
  if (!dest) return;

  switch (src->type)
  {
    case pal_UNSET:
    {
      dest->type = gotype_EMPTY;
    }
    break;
    
    case pal_IREG:
    {
      dest->type = gotype_REGISTER;
      dest->width = gowidth_DWORD;
      dest->data.reg = src->info.ireg.num;
    }
    break;
    
    case pal_RFILE:
    {
      dest->type = gotype_INDREGPLUSDISP8;
      dest->width = gowidth_DWORD;
      dest->data.regdisp.base = EBP;
      dest->data.regdisp.disp = src->info.value*4;
    }
    break;
    
    case pal_CONST:
    {
      dest->type = gotype_IMMEDIATE;
      dest->width = gowidth_DWORD;
      dest->data.imm = src->info.value;
    }
    break;
    
    case pal_CONSTB:
    {
      dest->type = gotype_IMMEDIATE;
      dest->width = src->info.value<128 ? gowidth_BYTE : gowidth_DWORD;
      dest->data.imm = src->info.value;
    }
    break;
    
    case pal_ALIAS:
    {
      genx86_translatealloc(chunk, dest, &chunk->alloc[src->info.value]);
    }
    break;
    
    default:
    fprintf(stderr, "Don't know how to handle that (src type=%d)\n", src->type);
    exit(1);
    break;
  }
}

genx86_operand* genx86_findoperand(pheta_chunk* chunk, palloc_info* src)
{
  genx86_operand* operand = 0;
  
  if (src->type != pal_UNSET)
  {
    if (!src->slot)
    {
      src->slot = cnew(genx86_operand);
/*      genx86_translatealloc(chunk, src->slot, src);*/
      src->slot->type = gotype_PARTIAL;
      src->slot->data.src = src;
    }
    operand = src->slot;
  }
  
  return operand;
}

void genx86_append(pheta_chunk* chunk, genx86_buffer* buf, uint5 opcode, 
  genx86_operand* dest, genx86_operand* src1, genx86_operand* src2)
{
  genx86_op* inst = cnew(genx86_op);
  clist* newinst;

  inst->op[0] = dest;
  inst->op[1] = src1;
  inst->op[2] = src2;
  inst->operator = opcode;
  
  newinst = clist_append(buf->buffer);
  newinst->data = inst;
  
  if (buf->expecting)
  {
    buf->beenset |= genx86_tab[opcode].flagset;
  }
}

void genx86_insert(pheta_chunk* chunk, genx86_buffer* buf, clist* posn,
  uint5 opcode, genx86_operand* dest, genx86_operand* src1,
  genx86_operand* src2)
{
  clist* newinst = clist_append(posn);
  genx86_op* newop = cnew(genx86_op);
  
  newop->operator = opcode;
  newop->op[0] = dest;
  newop->op[1] = src1;
  newop->op[2] = src2;
  
  newinst->data = newop;
}

uint5 genx86_equivalent(genx86_operand* a, genx86_operand* b)
{
  static const char* patypename[] =
    { "unset", "constb", "const", "rfile", "ireg", "stack",
      "alias", "spilled" };
  if (a->type != b->type) return 0;

  switch (a->type)
  {
    case gotype_PARTIAL:
/*
    fprintf(stderr, "WARNING: Comparing partial allocations for equivalence\n");
    fprintf(stderr, "a->data.src=%s, b->data.src=%s\n",
      patypename[a->data.src->type], patypename[b->data.src->type]);
*/
    if (a->data.src != b->data.src) return 0;
    break;
    
    case gotype_IMMEDIATE:
    if (a->width != b->width || a->data.imm != b->data.imm) return 0;
    break;

    case gotype_REGISTER:
    if (a->width != b->width || a->data.reg != b->data.reg) return 0;
    break;
    
    // probably overly pessimistic, but it probably doesn't matter at
    // this point
    case gotype_INDREG:
    case gotype_INDREGPLUSDISP8:
    case gotype_INDREGPLUSDISP32:
    case gotype_INDREGPLUSSCALEDREG:
    case gotype_INDREGPLUSSCALEDREGPLUSDISP8:
    case gotype_INDREGPLUSSCALEDREGPLUSDISP32:
    case gotype_ADDRESS:
    return 0;
    break;
  }
  
  return 1;
}

// Won't move something into itself, simplifies zero-loads
void genx86_move(pheta_chunk* chunk, genx86_buffer* buf, palloc_info* dest, 
  palloc_info* src)
{
  genx86_operand* destopi, *destop = genx86_findoperand(chunk, dest);
  genx86_operand* srcopi, *srcop = genx86_findoperand(chunk, src);
  
  if (dest->type==pal_ALIAS)
    destopi = genx86_findoperand(chunk,
      &chunk->alloc[palloc_close(chunk, dest->info.value)]);
  else
    destopi = destop;
  
  if (src->type==pal_ALIAS)
    srcopi = genx86_findoperand(chunk,
      &chunk->alloc[palloc_close(chunk, src->info.value)]);
  else
    srcopi = srcop;
  
  if (genx86_equivalent(destopi, srcopi)) return;
  
  genx86_append(chunk, buf, ab_MOV, destop, srcop, 0);
}

/*
void genx86_preserve(nativeblockinfo* nat, pheta_chunk* chunk, list* map)
{
  list* scan;
  palloc_info dest, src, nul;

  nul.type = pal_UNSET;

  for (scan=map; scan; scan=scan->prev)
  {
    pheta_rpair* rpair = (pheta_rpair*) scan->data;
    if (chunk->alloc[rpair->ph].type == pal_IREG)
    {
      dest.type = pal_RFILE;
      if (chunk->alloc[rpair->ph].info.ireg.arm_affiliation != -1)
      {
        dest.info.value = 
          chunk->alloc[rpair->ph].info.ireg.arm_affiliation*4;
        src.type = pal_IREG;
        src.info.ireg.num = chunk->alloc[rpair->ph].info.ireg.num;
        genx86_out(nat, ab_MOV, &dest, &src, &nul, map);
      }
      else fprintf(stderr, "Not preserving register %%%d\n", rpair->ph);
    }
  }
}
*/

genx86_buffer* genx86_newbuffer()
{
  genx86_buffer* buf = cnew(genx86_buffer);
  
  buf->buffer = clist_new();
  buf->fetch = buf->commit = 0;
  buf->expecting = buf->beenset = 0;
  buf->reloc = hash_new(16);
  
  return buf;
}

nativeblockinfo* genx86_translate(pheta_chunk* chunk)
{
  nativeblockinfo* nat = rtasm_new();
  genx86_buffer* buf = genx86_newbuffer();
  uint5 startline = 0;
  
//  genx86_translate_inner(buf, chunk, chunk->root, &startline);
  
//  relocate_fix(&nat->reloc, nat->base);
  
  return nat;
}

// insert code to recover register state after exception
void genx86_recover(genx86_buffer* buf, pheta_chunk* chunk)
{
  uint5 i;
  for (i=0; i<chunk->active->length; i++)
  {
    palloc_liverange* range = ((palloc_liverange*)chunk->active->item[i]->data);
    genx86_delayedfetchcommit* dfc;
    if (chunk->alloc[range->reg].arm_affiliation != -1)
    {
      uint5 creg = palloc_close(chunk, range->reg);
      list_add(&buf->commit);
      /* This is tricky, because a register alias can have a different
       * guest-machine register affiliation than the register it's aliased
       * to. The alias doesn't have a physical host register itself though.
       */
      buf->commit->data = dfc = cnew(genx86_delayedfetchcommit);
      dfc->var = &chunk->alloc[creg];
      dfc->src = chunk->alloc[range->reg].arm_affiliation;
      dfc->loc = buf->buffer->prev;
      fprintf(stderr, "Attempting to recover %d, arm: %d\n", range->reg,
        chunk->alloc[range->reg].arm_affiliation);
    }
  }
}

genx86_operand* genx86_makeconstant(pheta_chunk* chunk, uint5 value)
{
  hashentry* e;
  genx86_operand* op;
  
  if ((e = hash_lookup(chunk->constantpool, value)))
  {
    return e->data;
  }
  
  e = hash_insert(chunk->constantpool, value);
  op = e->data = cnew(genx86_operand);
  
  op->type = gotype_IMMEDIATE;
  op->width = value < 128 ? gowidth_BYTE : gowidth_DWORD;
  op->data.imm = value;
  
  return op;
}

genx86_operand* genx86_makeregister(pheta_chunk* chunk, uint5 reg)
{
  hashentry* e;
  genx86_operand* op;
  
  if ((e = hash_lookup(chunk->registerpool, reg)))
  {
    return e->data;
  }
  
  e = hash_insert(chunk->registerpool, reg);
  op = e->data = cnew(genx86_operand);
  
  op->type = gotype_REGISTER;
  op->width = gowidth_DWORD;
  op->data.reg = reg;
  
  return op;
}

genx86_operand* genx86_makeregister8(pheta_chunk* chunk, uint5 reg)
{
  hashentry* e;
  genx86_operand* op;
  uint5 key = reg+32;
  
  if ((e = hash_lookup(chunk->registerpool, key)))
  {
    return e->data;
  }
  
  e = hash_insert(chunk->registerpool, key);
  op = e->data = cnew(genx86_operand);
  
  op->type = gotype_REGISTER;
  op->width = gowidth_BYTE;
  op->data.reg = reg;
  
  return op;
}

genx86_operand* genx86_makebaseoffset(pheta_chunk* chunk, uint5 offset,
  uint5 width)
{
  hashentry* e;
  genx86_operand* op;
  uint5 key = offset | (width<<16);
  
  if ((e = hash_lookup(chunk->baseoffsetpool, key)))
  {
    return e->data;
  }
  
  e = hash_insert(chunk->baseoffsetpool, key);
  op = e->data = cnew(genx86_operand);
  
  op->type = offset>128 ? gotype_INDREGPLUSDISP32 : gotype_INDREGPLUSDISP8;
  op->width = width;
  op->data.regdisp.base = EBP;
  op->data.regdisp.disp = offset;
  
  return op;
}

void genx86_call_function(genx86_buffer* buf, pheta_chunk* chunk, void* func)
{
  genx86_operand* blank;
  hashentry* entry;
  reloc_record* reloc;
  
  blank = genx86_makeconstant(chunk, 0);
  genx86_append(chunk, buf, ab_CALL, blank, 0, 0);
  
  entry = hash_insert(buf->reloc, (uint5)buf->buffer->prev);
  reloc = entry->data = cnew(reloc_record);
  reloc->value = (uint5)func;
  reloc->offset = 0;
  reloc->size = relsize_WORD;
  reloc->type = reloc_RELATIVE;
}

uint5 genx86_translate_opcode(genx86_buffer* buf, pheta_chunk* chunk,
  pheta_basicblock* blk, pheta_instr* instr, meminfo* mem)
{
  clist* buffer = buf->buffer;
  uint5 i;
  list* map = 0;
  clist* walk;
  sint5 patchfalseblk = -1;
  static const uint5 predset[] =
  {
    ab_SETE, ab_SETNE, ab_SETB, ab_SETAE,
    ab_SETS, ab_SETNS, ab_SETO, ab_SETNO,
    ab_SETA, ab_SETBE, ab_SETGE, ab_SETL,
    ab_SETG, ab_SETLE
  };
  
  switch (instr->opcode)
  {
    case ph_FETCH:
    {
      genx86_delayedfetchcommit* dfc;
      if (instr->data.op.src1 < 15)
      {
        list_add(&buf->fetch);
        buf->fetch->data = dfc = cnew(genx86_delayedfetchcommit);
        dfc->var = &chunk->alloc[palloc_close(chunk, instr->data.op.dest)];
        dfc->src = instr->data.op.src1;
        dfc->loc = buf->buffer->prev;
        fprintf(stderr, "Fetch %d to %d\n",
          palloc_close(chunk, instr->data.op.dest), dfc->src);
      }
      else
      {
        fprintf(stderr, "Special fetch\n");
        switch (instr->data.op.src1)
        {
          case ph_R15_ADDR:
          {
            palloc_info* dest = &chunk->alloc[instr->data.op.dest];
            genx86_operand* src = genx86_makeconstant(chunk,
              chunk->virtualaddress);
            genx86_append(chunk, buf, ab_MOV, genx86_findoperand(chunk, dest),
              src, 0);
          }
          break;

          case ph_R15_FULL:
          {
            palloc_info* dest = &chunk->alloc[instr->data.op.dest];
            genx86_operand* src = genx86_makeconstant(chunk,
              chunk->virtualaddress);
            uint5 areg, breg;
            genx86_operand* aregfull;
            extern char* regname[];
              
            if (palloc_request_8bitsafe_ireg(chunk, &areg, 0))
            {
              if (palloc_request_8bitsafe_ireg(chunk, &breg, 1<<areg))
              {
                genx86_operand* al =
                  genx86_makeregister8(chunk, LO8BITREG(areg));
                genx86_operand* ah =
                  genx86_makeregister8(chunk, HI8BITREG(areg));
                genx86_operand* bl =
                  genx86_makeregister8(chunk, LO8BITREG(breg));
                genx86_operand* bh =
                  genx86_makeregister8(chunk, HI8BITREG(breg));
                genx86_operand* off, *num;
                genx86_operand* destop = genx86_findoperand(chunk, dest);
                
                fprintf(stderr, "Got registers %s and %s for 8-bit "\
                  "operations\n", regname[areg], regname[breg]);
                
                off = genx86_makebaseoffset(chunk,
                  offsetof(registerinfo, vflag), gowidth_BYTE);
                genx86_append(chunk, buf, ab_MOV, al, off, 0);

                off = genx86_makebaseoffset(chunk,
                  offsetof(registerinfo, cflag), gowidth_BYTE);
                genx86_append(chunk, buf, ab_MOV, ah, off, 0);

                off = genx86_makebaseoffset(chunk,
                  offsetof(registerinfo, zflag), gowidth_BYTE);
                genx86_append(chunk, buf, ab_MOV, bl, off, 0);

                off = genx86_makebaseoffset(chunk,
                  offsetof(registerinfo, nflag), gowidth_BYTE);
                genx86_append(chunk, buf, ab_MOV, bh, off, 0);
                
                genx86_append(chunk, buf, ab_SHL, ah,
                  genx86_makeconstant(chunk, 1), 0);
                genx86_append(chunk, buf, ab_SHL, bl,
                  genx86_makeconstant(chunk, 2), 0);
                genx86_append(chunk, buf, ab_SHL, bh,
                  genx86_makeconstant(chunk, 3), 0);
                
                genx86_append(chunk, buf, ab_OR, al, ah, 0);
                genx86_append(chunk, buf, ab_OR, al, bl, 0);
                genx86_append(chunk, buf, ab_OR, al, bh, 0);
                
                aregfull = genx86_makeregister(chunk, areg);
                
                genx86_append(chunk, buf, ab_SHL, aregfull,
                  genx86_makeconstant(chunk, 28), 0);
                
                genx86_append(chunk, buf, ab_MOV, destop, src, 0);

                genx86_append(chunk, buf, ab_OR, destop, aregfull, 0);
                
                palloc_relinquish_ireg(chunk, breg);
                palloc_relinquish_ireg(chunk, areg);
              }
              else
              {
                fprintf(stderr, "Can't free second register!\n");
                exit(1);
              }
            }
            else
            {
              fprintf(stderr, "Can't free a register!\n");
              exit(1);
            }
          }
          break;
          
          default:
          break;
        }
      }

  // no point in doing this?
  /* chunk->alloc[instr->data.op.dest].arm_affiliation = instr->data.op.src1;*/
/*
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      uint5 armsrc = instr->data.op.src1;
      switch (dest->type)
      {
        case pal_IREG:
        {
          palloc_info psrc;
          psrc.type = pal_RFILE;
          psrc.info.value = armsrc*4;
          genx86_append(chunk, buf, ab_MOV, dest, &psrc, &nul);
        }
        break;

        case pal_RFILE:
        {
          dest->info.value = armsrc*4;
        }
        break;

        default:
        break;
      }
*/
    }
    break;

    case ph_COMMIT:
    {
      genx86_delayedfetchcommit* dfc;

      list_add(&buf->commit);
      buf->commit->data = dfc = cnew(genx86_delayedfetchcommit);
      dfc->src = instr->data.op.dest;
      dfc->var = &chunk->alloc[palloc_close(chunk, instr->data.op.src1)];
      dfc->loc = buf->buffer->prev;
      fprintf(mem->trace, "Storing commit %d to %d\n", instr->data.op.src1,
        instr->data.op.dest);
/*
      uint5 armdest = instr->data.op.dest;
      palloc_info* src = &chunk->alloc[instr->data.op.src1];
      switch (src->type)
      {
        case pal_IREG:
        case pal_CONSTB:
        case pal_CONST:
        {
          palloc_info pdest;
          pdest.type = pal_RFILE;
          pdest.info.value = armdest*4;
          genx86_append(chunk, buf, ab_MOV, &pdest, src, &nul);
        }
        break;

        default:
        break;
      }
*/
    }
    break;
    
    case ph_ASSOC:
    {
      chunk->alloc[instr->data.op.src1].arm_affiliation = instr->data.op.dest;
    }
    break;

    case ph_ADDRESS:
    {
      chunk->virtualaddress = instr->data.imm;
      fprintf(mem->trace, "Found address: %x\n", chunk->virtualaddress);
    }
    break;

    case ph_LSL:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
      genx86_move(chunk, buf, dest, src1);
      genx86_append(chunk, buf, ab_SHL, genx86_findoperand(chunk, dest),
        genx86_findoperand(chunk, src2), 0);
    }
    break;

    case ph_LSR:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
      genx86_move(chunk, buf, dest, src1);
      genx86_append(chunk, buf, ab_SHR, genx86_findoperand(chunk, dest),
        genx86_findoperand(chunk, src2), 0);
    }
    break;

    case ph_ASR:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
      genx86_move(chunk, buf, dest, src1);
      genx86_append(chunk, buf, ab_SAR, genx86_findoperand(chunk, dest),
        genx86_findoperand(chunk, src2), 0);
    }
    break;

    case ph_ROR:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
      genx86_move(chunk, buf, dest, src1);
      genx86_append(chunk, buf, ab_ROR, genx86_findoperand(chunk, dest), 
        genx86_findoperand(chunk, src2), 0);
    }
    break;

    case ph_ROL:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
      genx86_move(chunk, buf, dest, src1);
      genx86_append(chunk, buf, ab_ROL, genx86_findoperand(chunk, dest),
        genx86_findoperand(chunk, src2), 0);
    }
    break;

    case ph_AND:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
      genx86_move(chunk, buf, dest, src1);
      genx86_append(chunk, buf, ab_AND, genx86_findoperand(chunk, dest),
        genx86_findoperand(chunk, src2), 0);
    }
    break;

    case ph_OR:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
      genx86_move(chunk, buf, dest, src1);
      genx86_append(chunk, buf, ab_OR, genx86_findoperand(chunk, dest),
        genx86_findoperand(chunk, src2), 0);
    }
    break;

    case ph_EOR:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
      genx86_move(chunk, buf, dest, src1);
      genx86_append(chunk, buf, ab_XOR, genx86_findoperand(chunk, dest),
        genx86_findoperand(chunk, src2), 0);
    }
    break;

    case ph_ADD:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
      if (dest->type==pal_IREG && src1->type==pal_IREG && 
          src2->type==pal_IREG && buf->expecting==0)
      {
        genx86_append(chunk, buf, ab_LEA, genx86_findoperand(chunk, dest),
          genx86_findoperand(chunk, src1), genx86_findoperand(chunk, src2));
      }
      else
      {
        genx86_move(chunk, buf, dest, src1);
        genx86_append(chunk, buf, ab_ADD, genx86_findoperand(chunk, dest),
          genx86_findoperand(chunk, src2), 0);
      }
    }
    break;

    case ph_ADC:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
      genx86_move(chunk, buf, dest, src1);
      genx86_append(chunk, buf, ab_ADC, genx86_findoperand(chunk, dest),
        genx86_findoperand(chunk, src2), 0);
    }
    break;

    case ph_SUB:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
      genx86_move(chunk, buf, dest, src1);
      genx86_append(chunk, buf, ab_SUB, genx86_findoperand(chunk, dest),
        genx86_findoperand(chunk, src2), 0);
    }
    break;

    case ph_SBC:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
      genx86_move(chunk, buf, dest, src1);
      genx86_append(chunk, buf, ab_CMC, 0, 0, 0);
      genx86_append(chunk, buf, ab_SBB, genx86_findoperand(chunk, dest),
        genx86_findoperand(chunk, src2), 0);
    }
    break;

    case ph_MUL:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
      genx86_operand* opdest = genx86_findoperand(chunk, dest);

      if ((src2->type==pal_CONST || src2->type==pal_CONSTB) && 
          dest->type==pal_IREG)
      {
        genx86_append(chunk, buf, ab_IMUL, opdest,
          genx86_findoperand(chunk, src1), genx86_findoperand(chunk, src2));
      }
      else
      {
        genx86_move(chunk, buf, dest, src1);
        genx86_append(chunk, buf, ab_IMUL, opdest,
          genx86_findoperand(chunk, src2), 0);
      }

      if ((buf->expecting & ph_Z) || (buf->expecting & ph_N))
      {
        genx86_append(chunk, buf, ab_TEST, opdest, opdest, 0);
      }
    }
    break;

    case ph_RRX:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src = &chunk->alloc[instr->data.op.src1];
      genx86_operand* one = genx86_makeconstant(chunk, 1);
      genx86_move(chunk, buf, dest, src);
      genx86_append(chunk, buf, ab_RCR, genx86_findoperand(chunk, dest), one, 
        0);
    }
    break;

    case ph_RLX:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src = &chunk->alloc[instr->data.op.src1];
      genx86_operand* one = genx86_makeconstant(chunk, 1);
      genx86_move(chunk, buf, dest, src);
      genx86_append(chunk, buf, ab_RCL, genx86_findoperand(chunk, dest), one, 
        0);
    }
    break;

    case ph_MOV:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src = &chunk->alloc[instr->data.op.src1];
      genx86_move(chunk, buf, dest, src);

      if ((buf->expecting & ph_Z) || (buf->expecting & ph_N))
      {
        genx86_operand* opdest = genx86_findoperand(chunk, dest);
        genx86_append(chunk, buf, ab_TEST, opdest, opdest, 0);
      }
    }
    break;

    case ph_NOT:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src = &chunk->alloc[instr->data.op.src1];
      genx86_operand* opdest = genx86_findoperand(chunk, dest);

      genx86_move(chunk, buf, dest, src);
      genx86_append(chunk, buf, ab_NOT, opdest, 0, 0);

      if ((buf->expecting & ph_Z) || (buf->expecting & ph_N))
      {
        genx86_append(chunk, buf, ab_TEST, opdest, opdest, 0);
      }
    }
    break;

    case ph_TEQ:  // nasty
    {
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
      genx86_operand* os1 = genx86_findoperand(chunk, src1);
      genx86_operand* os2 = genx86_findoperand(chunk, src2);
      genx86_append(chunk, buf, ab_PUSH, os1, 0, 0);
      genx86_append(chunk, buf, ab_XOR, os1, os2, 0);
      genx86_append(chunk, buf, ab_POP, os1, 0, 0);
    }
    break;

    case ph_TST:
    {
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
      genx86_append(chunk, buf, ab_TEST, genx86_findoperand(chunk, src1),
        genx86_findoperand(chunk, src2), 0);
    }
    break;

    case ph_CMP:
    {
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
      genx86_append(chunk, buf, ab_CMP, genx86_findoperand(chunk, src1),
        genx86_findoperand(chunk, src2), 0);
    }
    break;

    case ph_CMN:  // also nasty
    {
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
      genx86_operand* os1 = genx86_findoperand(chunk, src1);
      genx86_operand* os2 = genx86_findoperand(chunk, src2);
      genx86_append(chunk, buf, ab_PUSH, os1, 0, 0);
      genx86_append(chunk, buf, ab_ADD, os1, os2, 0);
      genx86_append(chunk, buf, ab_POP, os1, 0, 0);
    }
    break;

    case ph_FEXPECT:
    case ph_NFEXPECT:
    {
      uint5 expmask = instr->data.flag.need;
      buf->expecting |= expmask;
      buf->beenset = 0;
    }
    break;

    case ph_FCOMMIT:
    {
      uint5 needmask = instr->data.flag.need;
      uint5 pred = instr->data.flag.pred;
      genx86_operand* off;

      if (needmask & ph_C)
      {
        off = genx86_makebaseoffset(chunk, offsetof(registerinfo, cflag),
          gowidth_BYTE);
        if (buf->beenset & ph_IC)
          genx86_append(chunk, buf, ab_SETAE, off, 0, 0);
        else if (buf->beenset & ph_C)
          genx86_append(chunk, buf, ab_SETB, off, 0, 0);
        else
          assert(!"Carry flag has not been set");
      }
      if (needmask & ph_V)
      {
        off = genx86_makebaseoffset(chunk, offsetof(registerinfo, vflag),
          gowidth_BYTE);
        if (buf->beenset & ph_V)
          genx86_append(chunk, buf, ab_SETO, off, 0, 0);
        else
          assert(!"Overflow flag has not been set");
      }
      if (needmask & ph_N)
      {
        off = genx86_makebaseoffset(chunk, offsetof(registerinfo, nflag),
          gowidth_BYTE);
        if (buf->beenset & ph_N)
          genx86_append(chunk, buf, ab_SETS, off, 0, 0);
        else
          assert(!"Negative flag has not been set");
      }
      if (needmask & ph_Z)
      {
        off = genx86_makebaseoffset(chunk, offsetof(registerinfo, zflag),
          gowidth_BYTE);
        if (buf->beenset & ph_Z)
          genx86_append(chunk, buf, ab_SETE, off, 0, 0);
        else
          assert(!"Zero flag has not been set");
      }
      if (pred)
      {
        uint5 j;
        for (j=0; j<14; j++)
        {
          if (pred & (1<<j))
          {
            off = genx86_makebaseoffset(chunk,
              offsetof(registerinfo, predbuf[j]), gowidth_BYTE);
            genx86_append(chunk, buf, predset[j], off, 0, 0);
          }
        }
      }
      buf->expecting = 0;
    }
    break;

    case ph_NFCOMMIT:
    {
      uint5 pred = instr->data.flag.pred;

      if (pred)
      {
        uint5 j;
        for (j=0; j<14; j++)
        {
          if (pred & (1<<j))
          {
            genx86_operand* off = genx86_makebaseoffset(chunk,
              offsetof(registerinfo, npredbuf[j]), gowidth_BYTE);
            genx86_append(chunk, buf, predset[j], off, 0, 0);
          }
        }
      }
    }
    break;

    case ph_FENSURE:
    {
      /* it's possible to optimise this away a bit, but the logic's a bit
       * tricky & I can't work it out right now
       */
      uint5 mask = instr->data.flag.need;
      genx86_operand* zero, *off;
      if (mask)
      {
        if (mask==ph_C || mask==ph_IC)
        {
          off = genx86_makebaseoffset(chunk, offsetof(registerinfo, cflag),
            gowidth_DWORD);
          zero = genx86_makeconstant(chunk, 0);
          genx86_append(chunk, buf, ab_BT, off, zero, 0);
          if (mask==ph_IC)
          {
/*            assert(!"I wasn't expecting you to ensure inverse C!");*/
            fprintf(mem->trace, "WARNING: Ensured inverse C (might break)\n");
            genx86_append(chunk, buf, ab_CMC, 0, 0, 0);
          }
        }
        else
        {
          assert(!"Complicated flag restore pattern here");
        }
      }
    }
    break;

    case ph_NFENSURE:
    {
      assert(!"nfensure was a bad idea");
    }
    break;

    case ph_FWRITE:
    {
      palloc_info* src =
        &chunk->alloc[palloc_close(chunk, instr->data.op.src1)];
      if (src->type==pal_CONST || src->type==pal_CONSTB)
      {
        switch (src->info.value)
        {
          case 0:
          if (instr->data.op.dest & ph_C)
          {
            genx86_append(chunk, buf, ab_CLC, 0, 0, 0);
          }
          else
          {
            fprintf(stderr, "Can't write to that flag\n");
            exit(1);
          }
          break;
          
          case 1:
          if (instr->data.op.dest & ph_C)
          {
            genx86_append(chunk, buf, ab_STC, 0, 0, 0);
          }
          else
          {
            fprintf(stderr, "Can't write to that flag\n");
            exit(1);
          }
          break;
          
          default:
          fprintf(stderr, "Can only write 0 or 1 to a flag\n");
          exit(1);
        }
      }
      else
      {
        fprintf(stderr, "Write register to flag not supported yet\n");
        exit(1);
      }
    }
    break;

    case ph_LDB:
    case ph_LDW:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      genx86_operand* odest = genx86_findoperand(chunk, dest);
      genx86_operand* os1 = genx86_findoperand(chunk, src1);
      genx86_operand* regeax, *regesp;
      genx86_operand* memop;
      genx86_operand* rel, *misc;
      hashentry* entry;
      reloc_record* reloc;
      uint5 preserve_eax = chunk->reguse[EAX] && dest->type==pal_IREG
                           && dest->info.ireg.num!=EAX;
      void* jecxzloc;

      regeax = genx86_makeregister(chunk, EAX);
      regesp = genx86_makeregister(chunk, ESP);

      /*  memory structure base
       *  address
       */
      if (dest->type==pal_IREG && dest->info.ireg.num!=EAX)
      {
        preserve_eax |= palloc_evict_ireg(chunk, EAX, 
                                          (1<<EAX)|(1<<ECX)|(1<<EDX));
      }
      
      if (preserve_eax)
      {
        genx86_append(chunk, buf, ab_PUSH, regeax, 0, 0);
      }
      
      genx86_append(chunk, buf, ab_PUSH, os1, 0, 0);

      memop = genx86_makeconstant(chunk, (uint5)mem);
      genx86_append(chunk, buf, ab_PUSH, memop, 0, 0);

      if (instr->opcode == ph_LDW)
        genx86_call_function(buf, chunk, (void*)memory_readdataword);
      else
        genx86_call_function(buf, chunk, (void*)memory_readbyte);

      misc = genx86_makeconstant(chunk, 8);      
      genx86_append(chunk, buf, ab_ADD, regesp, misc, 0);
      
      misc = cnew(genx86_operand);
      misc->type = gotype_ADDRESS;
      misc->width = gowidth_BYTE;
      misc->data.addr = (uint5)mem + offsetof(meminfo, memoryfault);
      
      rel = genx86_makeconstant(chunk, 1);
      genx86_append(chunk, buf, ab_TEST, misc, rel, 0);
      
      rel = genx86_makeconstant(chunk, 0);
      genx86_append(chunk, buf, ab_JE, rel, 0, 0);

      entry = hash_insert(buf->reloc, (uint5)buf->buffer->prev);
      reloc = entry->data = cnew(reloc_record);
      reloc->value = 0;
      reloc->offset = 0;
      reloc->size = relsize_BYTE;
      reloc->type = reloc_PLACEHOLDER;

      jecxzloc = buf->buffer->prev;

      misc = genx86_makeconstant(chunk, (uint5)chunk->parentmachine);
      genx86_append(chunk, buf, ab_PUSH, misc, 0, 0);

      genx86_call_function(buf, chunk, (void*)processor_dataabort);
      
      misc = genx86_makeconstant(chunk, 4);
      genx86_append(chunk, buf, ab_ADD, regesp, misc, 0);

      genx86_append(chunk, buf, ab_RET, 0, 0, 0);

      entry = hash_insert(buf->reloc, (uint5)buf->buffer->prev);
      reloc = entry->data = cnew(reloc_record);
      reloc->value = (uint5)jecxzloc;
      reloc->offset = 0;
      reloc->size = relsize_BYTE;
      reloc->type = reloc_FORWARD;

      genx86_append(chunk, buf, ab_MOV, odest, regeax, 0);

      if (preserve_eax)
      {
        genx86_append(chunk, buf, ab_POP, regeax, 0, 0);
      }
    }
    break;

    case ph_STW:
    case ph_STB:
    {
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
      genx86_operand* os1 = genx86_findoperand(chunk, src1);
      genx86_operand* os2 = genx86_findoperand(chunk, src2);
      genx86_operand* regeax, *regesp;
      genx86_operand* memop;
      genx86_operand* rel;
      genx86_operand* misc;
      hashentry* entry;
      reloc_record* reloc;
      uint5 preserve_eax = chunk->reguse[EAX];
      void* jecxzloc;

      regeax = genx86_makeregister(chunk, EAX);
      regesp = genx86_makeregister(chunk, ESP);

      if (chunk->reguse[EAX])
      {
        preserve_eax |= palloc_evict_ireg(chunk, EAX, 
                                          (1<<EAX)|(1<<ECX)|(1<<EDX));
      }
      
      if (preserve_eax)
      {
        genx86_append(chunk, buf, ab_PUSH, regeax, 0, 0);
      }
      
      genx86_append(chunk, buf, ab_PUSH, os1, 0, 0);
      genx86_append(chunk, buf, ab_PUSH, os2, 0, 0);

      memop = genx86_makeconstant(chunk, (uint5)mem);
      genx86_append(chunk, buf, ab_PUSH, memop, 0, 0);

      if (instr->opcode == ph_STW)
        genx86_call_function(buf, chunk, (void*)memory_writeword);
      else
        genx86_call_function(buf, chunk, (void*)memory_writebyte);

      misc = genx86_makeconstant(chunk, 12);
      genx86_append(chunk, buf, ab_ADD, regesp, misc, 0);
            
      misc = cnew(genx86_operand);
      misc->type = gotype_ADDRESS;
      misc->width = gowidth_BYTE;
      misc->data.addr = (uint5)mem + offsetof(meminfo, memoryfault);
      
      rel = genx86_makeconstant(chunk, 1);
      genx86_append(chunk, buf, ab_TEST, misc, rel, 0);
      
      rel = genx86_makeconstant(chunk, 0);
      genx86_append(chunk, buf, ab_JE, rel, 0, 0);

      entry = hash_insert(buf->reloc, (uint5)buf->buffer->prev);
      reloc = entry->data = cnew(reloc_record);
      reloc->value = 0;
      reloc->offset = 0;
      reloc->size = relsize_BYTE;
      reloc->type = reloc_PLACEHOLDER;

      jecxzloc = buf->buffer->prev;

      misc = genx86_makeconstant(chunk, (uint5)chunk->parentmachine);
      genx86_append(chunk, buf, ab_PUSH, misc, 0, 0);

      genx86_call_function(buf, chunk, (void*)processor_dataabort);

      misc = genx86_makeconstant(chunk, 4);
      genx86_append(chunk, buf, ab_ADD, regesp, misc, 0);

      genx86_append(chunk, buf, ab_RET, 0, 0, 0);

      entry = hash_insert(buf->reloc, (uint5)buf->buffer->prev);
      reloc = entry->data = cnew(reloc_record);
      reloc->value = (uint5)jecxzloc;
      reloc->offset = 0;
      reloc->size = relsize_BYTE;
      reloc->type = reloc_FORWARD;

      if (preserve_eax)
      {
        genx86_append(chunk, buf, ab_POP, regeax, 0, 0);
      }
    }
    break;

    case ph_SWI:
//      rtasm_nop(nat);
    break;

    case ph_UNDEF:
//      rtasm_nop(nat);
    break;

    case ph_STATE:
    {
/*
      uint5 start = instr->data.imm;
      map = (list*)start;
*/
    }
    break;

    case ph_SYNC:
    {
      genx86_recover(buf, chunk);
    }
    break;

    // !!! this will actually have to do other stuff instead, esp. in
    // 26-bit mode.
    case ph_CAJMP:
    {
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
/*      genx86_operand* src = genx86_findoperand(chunk, src1);
      genx86_operand* c8 = genx86_makeconstant(chunk, 8);
      genx86_move(chunk, buf, dest, src1);
      genx86_append(chunk, buf, ab_ADD, genx86_findoperand(chunk, dest), c8, 0);
      */
      genx86_append(chunk, buf, ab_RET, 0, 0, 0);
    }
    break;

    case ph_CAJMP26F:
    {
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      genx86_operand* src = genx86_findoperand(chunk, src1);
      genx86_operand* c8 = genx86_makeconstant(chunk, 8);
      genx86_move(chunk, buf, dest, src1);
      genx86_append(chunk, buf, ab_ADD, genx86_findoperand(chunk, dest), c8, 0);
      genx86_append(chunk, buf, ab_BT, src, genx86_makeconstant(chunk, 28), 0);
      genx86_append(chunk, buf, ab_SETB, genx86_makebaseoffset(chunk,
        offsetof(registerinfo, vflag), gowidth_BYTE), 0, 0);
      genx86_append(chunk, buf, ab_BT, src, genx86_makeconstant(chunk, 29), 0);
      genx86_append(chunk, buf, ab_SETB, genx86_makebaseoffset(chunk,
        offsetof(registerinfo, cflag), gowidth_BYTE), 0, 0);
      genx86_append(chunk, buf, ab_BT, src, genx86_makeconstant(chunk, 30), 0);
      genx86_append(chunk, buf, ab_SETB, genx86_makebaseoffset(chunk,
        offsetof(registerinfo, zflag), gowidth_BYTE), 0, 0);
      genx86_append(chunk, buf, ab_BT, src, genx86_makeconstant(chunk, 31), 0);
      genx86_append(chunk, buf, ab_SETB, genx86_makebaseoffset(chunk,
        offsetof(registerinfo, nflag), gowidth_BYTE), 0, 0);
      genx86_append(chunk, buf, ab_RET, 0, 0, 0);
    }
    break;

    case ph_UKJMP:
    {
      palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
      genx86_operand* src = genx86_findoperand(chunk, src1);
      genx86_operand* pcop;

      pcop = genx86_makebaseoffset(chunk, 15*4, gowidth_DWORD);
      genx86_append(chunk, buf, ab_MOV, pcop, src, 0);
      genx86_append(chunk, buf, ab_RET, 0, 0, 0);
    }
    break;

    case ph_CONST:
    case ph_CONSTB:
    /* these don't need to generate any actual code */
    break;

    default:
    fprintf(stderr, "Unimplemented ph2 opcode: %s\n", opname[instr->opcode]);
    exit(1);
  }
  
  return 0;
}

void genx86_complete_alloc(pheta_chunk* chunk)
{
  uint5 i;
  
  for (i=0; i<chunk->tempno; i++)
  {
    palloc_info* alloc = &chunk->alloc[i];
    if (alloc->slot && alloc->slot->type==gotype_PARTIAL)
    {
      genx86_translatealloc(chunk, alloc->slot, alloc);
    }
  }
}

void genx86_insert_spill_code_inner(genx86_buffer* buf, pheta_chunk* chunk)
{
  list* scan;
  const uint5 offset[] = {
    0,  4,  8,  12,
    16, 20, 24, 28,
    32, 36, 40, 44,
    48, 52, 56, 60,
    60, 60, offsetof(registerinfo, cpsr), offsetof(registerinfo, cpsr)
  };
  
  for (scan=buf->commit; scan; scan=scan->prev)
  {
    genx86_delayedfetchcommit* dfc = scan->data;
    switch (dfc->var->type)
    {
      case pal_IREG:
      case pal_CONST:
      case pal_CONSTB:
      {
        genx86_operand* mem = genx86_makebaseoffset(chunk, offset[dfc->src], 
          gowidth_DWORD);
        fprintf(stderr, "Commit %d to %d\n", dfc->var->info.ireg.num,
          dfc->src);
        genx86_insert(chunk, buf, dfc->loc->next, ab_MOV, mem, dfc->var->slot,
          0);
      }
      break;
      
      default:
      break;
    }

    switch (dfc->src)
    {
      case ph_R15_ADDR:
      case ph_R15_FULL:
      /* this so shouldn't be here */
/*      genx86_insert(chunk, buf, dfc->loc->next->next, ab_RET, 0, 0, 0);*/
      break;
      
      default:
      break;
    }
  }

  for (scan=buf->fetch; scan; scan=scan->prev)
  {
    genx86_delayedfetchcommit* dfc = scan->data;
    if (dfc->var->type==pal_IREG)
    {
      genx86_operand* mem = genx86_makebaseoffset(chunk, offset[dfc->src], 
        gowidth_DWORD);
      fprintf(stderr, "Fetch %d to %d\n", dfc->src, dfc->var->info.ireg.num);
      genx86_insert(chunk, buf, dfc->loc->next, ab_MOV, dfc->var->slot, mem, 0);
    }
  }
}

void genx86_insert_spill_code(pheta_chunk* chunk)
{
  list* scanblock;
  
  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
  {
    pheta_basicblock* blk = scanblock->data;
    genx86_insert_spill_code_inner(blk->gxbuffer, chunk);
  }
}

// possibly wrong somehow
static void killrelocentry(void* data)
{
  free(data);
}

void genx86_flatten_code_inner(nativeblockinfo* nat, pheta_chunk* chunk,
  pheta_basicblock* blk, pheta_basicblock* last)
{
  clist* scancode;
  uint5 unconditional;
  list* scanreloc;

  fprintf(stderr, "Flatten %p\n", blk);

  blk->natoffset = nat->length;
  
  for (scanreloc=blk->reloc; scanreloc; scanreloc=scanreloc->prev)
  {
    reloc_record* reloc = scanreloc->data;
    reloc->value = nat->length - reloc->offset - 4;
  }
  
  relocate_fix(&blk->reloc, nat->base);
  
  for (scancode=blk->gxbuffer->buffer->next; scancode->data; 
       scancode=scancode->next)
  {
    hashentry* e;
    genx86_op* op = scancode->data;
    
    genx86_asm(nat, op);
    
    if ((e = hash_lookup(blk->gxbuffer->reloc, (uint5)scancode)))
    {
      reloc_record* rel = e->data;
      uint5 offset, value;

      switch (rel->size)
      {
        case relsize_BYTE: offset = nat->length-1; break;
        case relsize_HALFWORD: offset = nat->length-2; break;
        case relsize_WORD: offset = nat->length-4; break;
      }

      switch (rel->type)
      {
        case reloc_RELATIVE:
        {
          relocate_add(&nat->reloc, rel->value, offset, rel->size, rel->type);
        }
        break;
        
        case reloc_FORWARD:
        {
          hashentry* f = hash_lookup(blk->gxbuffer->reloc, rel->value);
          reloc_record* placeholder = f->data;
          relocate_add(&nat->reloc, offset-placeholder->value, 
            placeholder->value, placeholder->size, reloc_ABSOLUTE);
        }
        break;
        
        case reloc_PLACEHOLDER:
        {
          rel->value = offset;
        }
        break;
      }
    }
  }

  unconditional = ((blk->predicate & 0xf)==ph_AL);

  // if we have a true or a false block, and not a single true always block
  if ((blk->trueblk || blk->falseblk) && !unconditional)
  {
    genx86_op* op = cnew(genx86_op);
    genx86_operand* off;
    op->operator = ab_TEST;
    off = genx86_makebaseoffset(chunk, (blk->predicate & ph_NAT) ?
      offsetof(registerinfo, npredbuf[blk->predicate & 0xf]) :
      offsetof(registerinfo, predbuf[blk->predicate & 0xf]), gowidth_BYTE);
    op->op[0] = off;
    op->op[1] = genx86_makeconstant(chunk, 1);
    op->op[2] = 0;
    genx86_asm(nat, op);
    free(op);
  }
  
  blk->marker = 1;
  
  if (blk->trueblk)
  {
    if (blk->falseblk)
    {
      if (blk->falseblk->marker)
      {
        genx86_op* op = cnew(genx86_op);

        op->operator = ab_JE;
        op->op[0] = genx86_makeconstant(chunk, blk->falseblk->natoffset -
          nat->length - 6);
        op->op[1] = op->op[2] = 0;
        genx86_asm(nat, op);
        free(op);
      }
      else
      {
        // put in patchback request, unless false blk will follow immediately
        if (!blk->trueblk->marker)
        {
          genx86_op* op = cnew(genx86_op);
          reloc_record* reloc;

          op->operator = ab_JE;
          op->op[0] = genx86_makeconstant(chunk, 0x100);
          op->op[1] = op->op[2] = 0;
          genx86_asm(nat, op);
          reloc = cnew(reloc_record);
          reloc->type = reloc_ABSOLUTE;
          reloc->value = 0;
          reloc->offset = nat->length-4;
          reloc->size = relsize_WORD;
          list_add(&blk->falseblk->reloc);
          blk->falseblk->reloc->data = reloc;
          free(op);
        }
      }
    }  // if (blk->falseblk)
  
    if (blk->trueblk->marker)
    {
      genx86_op* op;
      // jump to already-generated code
      op = cnew(genx86_op);
      op->operator = unconditional ? ab_JMP : ab_JNE;
      op->op[0] = genx86_makeconstant(chunk, blk->trueblk->natoffset -
        nat->length - (unconditional ? 5 : 6));
      op->op[1] = op->op[2] = 0;
      genx86_asm(nat, op);
      free(op);
    }
    else
    {
      // generate new code
      genx86_flatten_code_inner(nat, chunk, blk->trueblk, blk);
    }
  }
  
  if (blk->falseblk && !blk->falseblk->marker)
  {
    genx86_flatten_code_inner(nat, chunk, blk->falseblk, blk);
  }
}

nativeblockinfo* genx86_flatten_code(pheta_chunk* chunk)
{
  nativeblockinfo* nat = rtasm_new();
  
  palloc_clearmarkers(chunk);
  
  genx86_flatten_code_inner(nat, chunk, chunk->root, 0);

  relocate_fix(&nat->reloc, nat->base);
  
  x86dism_block(nat);
  
  return nat;
}

/*
  list* scanblock;
  nativeblockinfo* nat = rtasm_new();
  pqueue* queue = pqueue_new();
  pqueueitem* qitem;
  pheta_basicblock* last = 0;
  
  palloc_clearmarkers(chunk);
  
  qitem = pqueue_insert(queue, -50);
  qitem->data = chunk->root;
  
  while ((qitem = pqueue_extract(queue)))
  {
    pheta_basicblock* blk = qitem->data;
    // these might benefit from being weighted by profile data
    uint5 truepriority = 25, falsepriority = 25;

    // a block can be added more than once before it is generated,
    // so this ensures we only output one copy... yuck
    if (blk->marker)
    {
      free(qitem);
      continue;
    }

    if (last && ((last->trueblk && last->trueblk != blk) ||
                 (last->falseblk && last->falseblk != blk)))
    {
      uint5 unconditional = ((last->predicate & 0xf) == ph_AL);
      genx86_op* op;
      genx86_operand* off;

      if (!unconditional)
      {
        op = cnew(genx86_op);
        op->operator = ab_TEST;
        off = genx86_makebaseoffset(chunk, (last->predicate & ph_NAT) ?
          offsetof(registerinfo, npredbuf[last->predicate & 0xf]) :
          offsetof(registerinfo, predbuf[last->predicate & 0xf]), gowidth_BYTE);
        op->op[0] = off;
        op->op[1] = genx86_makeconstant(chunk, 1);
        op->op[2] = 0;
        genx86_asm(nat, op);
        free(op);
      }
      
      // generate a branch if we aren't falling through from the
      // previous true block
      if (last->trueblk && last->trueblk != blk)
      {
        reloc_record* reloc;
        op = cnew(genx86_op);
        op->operator = ab_JNE;
        // 0x100 ensures we reserve 32 bits of offset
        op->op[0] = genx86_makeconstant(chunk, 0x100);
        op->op[1] = op->op[2] = 0;
        genx86_asm(nat, op);
        free(op);
        reloc = cnew(reloc_record);
        reloc->type = reloc_ABSOLUTE;
        reloc->value = 0;
        reloc->offset = nat->length-4;
        reloc->size = relsize_WORD;
        list_add(&last->trueblk->reloc);
        last->trueblk->reloc->data = reloc;
      }

      // generate a branch if we aren't falling through from the
      // previous false block
      if (last->falseblk && last->falseblk != blk)
      {
        reloc_record* reloc;
        op = cnew(genx86_op);
        op->operator = ab_JE;
        // again, reserve 32 bits for offset
        op->op[0] = genx86_makeconstant(chunk, 0x100);
        op->op[1] = op->op[2] = 0;
        genx86_asm(nat, op);
        free(op);
        reloc = cnew(reloc_record);
        reloc->type = reloc_ABSOLUTE;
        reloc->value = 0;
        reloc->offset = nat->length-4;
        reloc->size = relsize_WORD;
        list_add(&last->falseblk->reloc);
        last->falseblk->reloc->data = reloc;
      }
    }
    
    for (scanblock=blk->reloc; scanblock; scanblock=scanblock->prev)
    {
      reloc_record* reloc = scanblock->data;
      reloc->value = nat->length - reloc->offset - 4;
    }
    
    relocate_fix(&blk->reloc, nat->base);

    blk->natoffset = nat->length;    
    genx86_flatten_code_inner(nat, blk);
//    hash_nuke(blk->gxbuffer->reloc, &killrelocentry);
    
    blk->marker = 1;
    
    if (blk->trueblk && !blk->falseblk) truepriority += 50;
    
    // link this block to previously generated code
    if ((blk->trueblk && blk->trueblk->marker) ||
        (blk->falseblk && blk->falseblk->marker))
    {
      uint5 unconditional = ((blk->predicate & 0xf) == ph_AL);
      genx86_op* op;
      genx86_operand* off;

      if (!unconditional)
      {
        op = cnew(genx86_op);
        op->operator = ab_TEST;
        off = genx86_makebaseoffset(chunk, (blk->predicate & ph_NAT) ?
          offsetof(registerinfo, npredbuf[blk->predicate & 0xf]) :
          offsetof(registerinfo, predbuf[blk->predicate & 0xf]), gowidth_BYTE);
        op->op[0] = off;
        op->op[1] = genx86_makeconstant(chunk, 1);
        op->op[2] = 0;
        genx86_asm(nat, op);
        free(op);
      }
      
      // if we've got a true block, and one already exists, branch to it
      if (blk->trueblk && blk->trueblk->marker)
      {
        op = cnew(genx86_op);
        op->operator = unconditional ? ab_JMP : ab_JNE;
        op->op[0] = genx86_makeconstant(chunk, blk->trueblk->natoffset - 
          nat->length - (unconditional ? 5 : 6));
        op->op[1] = op->op[2] = 0;
        genx86_asm(nat, op);
        blk->trueblk = 0;
        free(op);
      }

      // if we've got a false block, and one already exists, branch to it
      if (blk->falseblk && blk->falseblk->marker)
      {
        op = cnew(genx86_op);
        op->operator = ab_JE;
        op->op[0] = genx86_makeconstant(chunk, blk->falseblk->natoffset -
          nat->length - 6);
        op->op[1] = op->op[2] = 0;
        genx86_asm(nat, op);
        blk->falseblk = 0;
        free(op);
      }
    }
    
    // add child nodes
    if (blk->trueblk && !blk->trueblk->marker)
    {
      pqueueitem* newitem;
      if (blk->scsubgraph == blk->trueblk->scsubgraph) truepriority += 50;
      newitem = pqueue_insert(queue, -truepriority);
      newitem->data = blk->trueblk;
    }
    
    if (blk->falseblk && !blk->falseblk->marker)
    {
      pqueueitem* newitem;
      if (blk->scsubgraph == blk->falseblk->scsubgraph) falsepriority += 50;
      newitem = pqueue_insert(queue, -falsepriority);
      newitem->data = blk->falseblk;
    }
    
    last = blk;
    
    free(qitem);
  }

  relocate_fix(&nat->reloc, nat->base);
  x86dism_block(nat);
  
  return nat;
}
*/

