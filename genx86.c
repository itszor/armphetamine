#include <stdio.h>

#include "genx86.h"
#include "x86asm.h"
#include "x86dism.h"
#include "rtasm.h"
#include "pheta.h"
#include "palloc.h"
#include "pseudo.h"

static const genx86_variant genx86_tab[] =
{
  /* shl */ { NULL,
              NULL,
              &rtasm_shl_rm32_imm8,
              NULL,
              &rtasm_shl_rm32_cl,
              NULL,
              NULL,
              NULL,
              CFLAG, VFLAG|NFLAG|ZFLAG
            },
  /* shr */ { NULL,
              NULL,
              &rtasm_shr_rm32_imm8,
              NULL,
              &rtasm_shr_rm32_cl,
              NULL,
              NULL,
              NULL,
              CFLAG, VFLAG|NFLAG|ZFLAG
            },
  /* sar */ { NULL,
              NULL,
              &rtasm_sar_rm32_imm8,
              NULL,
              &rtasm_sar_rm32_cl,
              NULL,
              NULL,
              NULL,
              CFLAG, VFLAG|NFLAG|ZFLAG
            },
  /* ror */ { NULL,
              NULL,
              &rtasm_ror_rm32_imm8,
              NULL,
              &rtasm_ror_rm32_cl,
              NULL,
              NULL,
              NULL,
              CFLAG, VFLAG
            },
  /* rol */ { NULL,
              NULL,
              &rtasm_rol_rm32_imm8,
              NULL,
              &rtasm_rol_rm32_cl,
              NULL,
              NULL,
              NULL,
              CFLAG, VFLAG
            },
  /* and */ { &rtasm_and_r32_rm32,
              &rtasm_and_rm32_r32,
              &rtasm_and_rm32_imm8,
              &rtasm_and_rm32_imm32,
              NULL,
              NULL,
              NULL,
              NULL,
              NFLAG|ZFLAG, CFLAG|VFLAG
            },
  /* or */  { &rtasm_or_r32_rm32,
              &rtasm_or_rm32_r32,
              &rtasm_or_rm32_imm8,
              &rtasm_or_rm32_imm32,
              NULL,
              NULL,
              NULL,
              NULL,
              NFLAG|ZFLAG, CFLAG|VFLAG
            },
  /* xor */ { &rtasm_xor_r32_rm32,
              &rtasm_xor_rm32_r32,
              &rtasm_xor_rm32_imm8,
              &rtasm_xor_rm32_imm32,
              NULL,
              NULL,
              NULL,
              NULL,
              NFLAG|ZFLAG, CFLAG|VFLAG
            },
  /* add */ { &rtasm_add_r32_rm32,
              &rtasm_add_rm32_r32,
              &rtasm_add_rm32_imm8,
              &rtasm_add_rm32_imm32,
              NULL,
              NULL,
              NULL,
              NULL,
              CFLAG|VFLAG|NFLAG|ZFLAG, 0
            },
  /* adc */ { &rtasm_adc_r32_rm32,
              &rtasm_adc_rm32_r32,
              &rtasm_adc_rm32_imm8,
              &rtasm_adc_rm32_imm32,
              NULL,
              NULL,
              NULL,
              NULL,
              CFLAG|VFLAG|NFLAG|ZFLAG, 0
            },
  /* sub */ { &rtasm_sub_r32_rm32,
              &rtasm_sub_rm32_r32,
              &rtasm_sub_rm32_imm8,
              &rtasm_sub_rm32_imm32,
              NULL,
              NULL,
              NULL,
              NULL,
              ICFLAG|VFLAG|NFLAG|ZFLAG, 0
            },
  /* sbb */ { &rtasm_sbb_r32_rm32,
              &rtasm_sbb_rm32_r32,
              &rtasm_sbb_rm32_imm8,
              &rtasm_sbb_rm32_imm32,
              NULL,
              NULL,
              NULL,
              NULL,
              ICFLAG|VFLAG|NFLAG|ZFLAG, 0
            },
  /* imul */{ &rtasm_imul_r32_rm32,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_imul_r32_rm32_imm8,
              &rtasm_imul_r32_rm32_imm32,
              NULL,
              0, CFLAG|VFLAG|NFLAG|ZFLAG
            },
  /* lea */ { &rtasm_lea_r32_m32,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              0, 0
            },
  /* mov */ { &rtasm_mov_r32_rm32,
              &rtasm_mov_rm32_r32,
              NULL,
              &rtasm_mov_rm32_imm32,
              NULL,
              NULL,
              NULL,
              NULL,
              0, 0
            },
  /* not */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_not_rm32,
              0, 0
            },
  /* push */{ NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_push_rm32,
              0, 0
            },
  /* pop */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_pop_rm32,
              0, 0
            },
  /* rcr */ { NULL,
              NULL,
              &rtasm_rcr_rm32_imm8,
              NULL,
              &rtasm_rcr_rm32_cl,
              NULL,
              NULL,
              NULL,
              CFLAG, VFLAG
            },
  /* rcl */ { NULL,
              NULL,
              &rtasm_rcl_rm32_imm8,
              NULL,
              &rtasm_rcl_rm32_cl,
              NULL,
              NULL,
              NULL,
              CFLAG, VFLAG
            },
  /* test */{ NULL,
              &rtasm_test_rm32_r32,
              NULL,
              &rtasm_test_rm32_imm32,
              NULL,
              NULL,
              NULL,
              NULL,
              NFLAG|ZFLAG, CFLAG|VFLAG
            },
  /* cmp */ { &rtasm_cmp_r32_rm32,
              &rtasm_cmp_rm32_r32,
              NULL,
              &rtasm_cmp_rm32_imm32,
              NULL,
              NULL,
              NULL,
              NULL,
              ICFLAG|VFLAG|NFLAG|ZFLAG, 0
            }
};

void genx86_test(void)
{
  nativeblockinfo* nat = x86asm_newnative();
  
  rtasm_opsize16(nat);
  rtasm_add_ax_imm16(nat, 54);
  rtasm_opsize16(nat);
  rtasm_jne_rel16(nat, 0x1234);
  rtasm_je_rel32(nat, 0x12345678);
  rtasm_imul_r32_rm32_imm32(nat, ECX, rtasm_ind(EBX), 0x300);
  rtasm_and_rm32_imm32(nat, rtasm_ind8(EAX,4), 0x123);
  rtasm_arpl_rm16_r16(nat, rtasm_reg(EAX), EBX);
  rtasm_bound_r32_m32x2(nat, EDX, rtasm_scind8(EDX, ECX, scale_2, 4));
  rtasm_bswap_r32(nat, ECX);
  rtasm_call_rel32(nat, (uint5)genx86_test);
  rtasm_call_rm32(nat, rtasm_ind8(EBX, 8));
  rtasm_call_ptr16_32(nat, 13, 0x1234);
  rtasm_call_m16_32(nat, rtasm_ind8(EBX, 8));
  rtasm_cmp_rm32_imm32(nat, rtasm_ind8(EBP, 8), 1000);
  rtasm_cmpxchg_rm32_r32(nat, rtasm_reg(EAX), EBX);
  rtasm_int_imm8(nat, 0x21);
  rtasm_lar_r32_rm32(nat, ECX, rtasm_ind(ECX));
  rtasm_movzx_r32_rm8(nat, EDX, rtasm_ind8(EAX, 4));
  rtasm_popf(nat);
  rtasm_push_ss(nat);
  rtasm_rdtsc(nat);
  rtasm_fadd_st0_sti(nat, 3);
  rtasm_fsin(nat);
  
  x86dism_block(nat);
  
  x86asm_deletenative(nat);
}

static const char* allocname[] = {
  "unset", "constb", "const", "rfile", "ireg",
  "stack", "alias", "split"
};

static const char* abname[] = {
  "shl", "shr", "sar", "ror", "rol", "and", "or", "xor", "add", "adc",
  "sub", "sbb", "imul", "lea", "mov", "not", "push", "pop", "rcr", "rcl",
  "test", "cmp"
};

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

#define COMPOUND(D,S1,S2) (((D)*pal_NUMTYPES*pal_NUMTYPES) + \
                           ((S1)*pal_NUMTYPES) + (S2))
#define ERR { fprintf(stderr, "Specialisation error at %d. " \
  "Opcode=%s, pattern=%s:%s:%s.\n", \
  __LINE__, abname[opcode], allocname[dest->type], allocname[src1->type], \
  allocname[src2->type]); abort(); }

void genx86_out(nativeblockinfo* nat, uint5 opcode, palloc_info* dest,
                palloc_info* src1, palloc_info* src2, uint5 line)
{
  dest = genx86_closesplit(dest, line);
  src1 = genx86_closesplit(src1, line);
  src2 = genx86_closesplit(src2, line);
  
  switch (COMPOUND(dest->type, src1->type, src2->type))
  {
    case COMPOUND(pal_RFILE, pal_UNSET, pal_UNSET):
    {
      if (genx86_tab[opcode].rm32)
      {
        genx86_tab[opcode].rm32(nat, rtasm_ind8(EBP, dest->info.value*4));
      }
      else ERR;
    }
    break;
    
    case COMPOUND(pal_IREG, pal_UNSET, pal_UNSET):
    {
      if (genx86_tab[opcode].rm32)
      {
        genx86_tab[opcode].rm32(nat, rtasm_reg(dest->info.ireg.num));
      }
      else ERR;
    }
    break;
    
    case COMPOUND(pal_STACK, pal_UNSET, pal_UNSET):
    {
      if (genx86_tab[opcode].rm32)
      {
        genx86_tab[opcode].rm32(nat, rtasm_ind8(ESP, dest->info.value*4));
      }
      else ERR;
    }
    break;

    case COMPOUND(pal_RFILE, pal_CONSTB, pal_UNSET):
    {
      uint5 value = src1->info.value;
      if (value<128 && genx86_tab[opcode].rm32_i8)
      {
        genx86_tab[opcode].rm32_i8(nat, rtasm_ind8(EBP, dest->info.value*4), 
          value);
      }
      else if (genx86_tab[opcode].rm32_i32)
      {
        genx86_tab[opcode].rm32_i32(nat, rtasm_ind8(EBP, dest->info.value*4),
          value);
      }
      else ERR;
    }
    break;
    
    case COMPOUND(pal_RFILE, pal_CONST, pal_UNSET):
    {
      if (genx86_tab[opcode].rm32_i32)
      {
        genx86_tab[opcode].rm32_i32(nat, rtasm_ind8(EBP, dest->info.value*4),
          src1->info.value);
      }
      else ERR;
    }
    break;
    
    case COMPOUND(pal_RFILE, pal_RFILE, pal_UNSET):
    ERR;
    break;
    
    case COMPOUND(pal_RFILE, pal_IREG, pal_UNSET):
    {
      if (genx86_tab[opcode].rm32_r32)
      {
        genx86_tab[opcode].rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
          src1->info.ireg.num);
      }
      else if (genx86_tab[opcode].rm32_c)
      {
        if (src1->info.ireg.num==ECX)
        {
          genx86_tab[opcode].rm32_c(nat, rtasm_ind8(EBP, dest->info.value*4));
        }
        else
        {
          rtasm_push_r32(nat, ECX);
          rtasm_mov_r32_rm32(nat, ECX, rtasm_reg(src1->info.ireg.num));
          genx86_tab[opcode].rm32_c(nat, rtasm_ind8(EBP, dest->info.value*4));
          rtasm_pop_r32(nat, ECX);
        }
      }
      else if (genx86_tab[opcode].r32_rm32)
      {
        // ew!
        rtasm_xchg_r32_rm32(nat, src1->info.ireg.num, rtasm_ind8(EBP,
          dest->info.value*4));
        genx86_tab[opcode].r32_rm32(nat, src1->info.ireg.num, rtasm_ind8(EBP,
          dest->info.value*4));
        rtasm_xchg_r32_rm32(nat, src1->info.ireg.num, rtasm_ind8(EBP,
          dest->info.value*4));
      }
      else ERR;
    }
    break;
    
    case COMPOUND(pal_RFILE, pal_STACK, pal_UNSET):
    ERR;
    break;
    
    case COMPOUND(pal_IREG, pal_CONSTB, pal_UNSET):
    {
      uint5 value = src1->info.value;
      if (value<128 && genx86_tab[opcode].rm32_i8)
      {
        genx86_tab[opcode].rm32_i8(nat, rtasm_reg(dest->info.ireg.num), 
          value);
      }
      else if (genx86_tab[opcode].rm32_i32)
      {
        genx86_tab[opcode].rm32_i32(nat, rtasm_reg(dest->info.ireg.num), 
          value);
      }
      else ERR;
    }
    break;
    
    case COMPOUND(pal_IREG, pal_CONST, pal_UNSET):
    {
      uint5 value = src1->info.value;
      if (value<128 && genx86_tab[opcode].rm32_i8)
      {
        genx86_tab[opcode].rm32_i8(nat, rtasm_reg(dest->info.ireg.num),
          value);
      }
      else if (genx86_tab[opcode].rm32_i32)
      {
        genx86_tab[opcode].rm32_i32(nat, rtasm_reg(dest->info.ireg.num),
          value);
      }
      else ERR;
    }
    break;
    
    case COMPOUND(pal_IREG, pal_RFILE, pal_UNSET):
    {
      if (genx86_tab[opcode].r32_rm32)
      {
        genx86_tab[opcode].r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
          src1->info.value*4));
      }
      else ERR;
    }
    break;
    
    case COMPOUND(pal_IREG, pal_IREG, pal_UNSET):
    {
      if (genx86_tab[opcode].r32_rm32)
      {
        genx86_tab[opcode].r32_rm32(nat, dest->info.ireg.num, 
          rtasm_reg(src1->info.ireg.num));
      }
      else if (genx86_tab[opcode].rm32_r32)
      {
        genx86_tab[opcode].rm32_r32(nat, rtasm_reg(dest->info.ireg.num),
          src1->info.ireg.num);
      }
      else if (genx86_tab[opcode].rm32_c)
      {
        if (src1->info.ireg.num==ECX)
        {
          genx86_tab[opcode].rm32_c(nat, rtasm_reg(dest->info.ireg.num));
        }
        else
        {
          if (dest->info.ireg.num==ECX)
          {
            rtasm_xchg_r32_rm32(nat, ECX, rtasm_reg(src1->info.ireg.num));
            genx86_tab[opcode].rm32_c(nat, rtasm_reg(src1->info.ireg.num));
            rtasm_xchg_r32_rm32(nat, ECX, rtasm_reg(src1->info.ireg.num));
          }
          else
          {
            rtasm_push_r32(nat, ECX);
            rtasm_mov_r32_rm32(nat, ECX, rtasm_reg(src1->info.ireg.num));
            genx86_tab[opcode].rm32_c(nat, rtasm_reg(dest->info.ireg.num));
            rtasm_pop_r32(nat, ECX);
          }
        }
      }
      else ERR;
    }
    break;

    case COMPOUND(pal_IREG, pal_STACK, pal_UNSET):
    {
      if (genx86_tab[opcode].r32_rm32)
      {
        genx86_tab[opcode].r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(ESP,
          src1->info.value*4));
      }
      else ERR;
    }
    break;
    
    case COMPOUND(pal_IREG, pal_IREG, pal_IREG):
    {
      switch (opcode)
      {
        case ab_LEA:  // we can only do LEA like this!
        {
          genx86_tab[ab_LEA].r32_rm32(nat, dest->info.ireg.num,
            rtasm_scind(src1->info.ireg.num, src2->info.ireg.num, scale_1));
        }
        break;
        
        default:
        ERR;
      }
    }
    break;
    
    case COMPOUND(pal_IREG, pal_RFILE, pal_CONSTB):
    {
      if (genx86_tab[opcode].r32_rm32_i8)
      {
        genx86_tab[opcode].r32_rm32_i8(nat, dest->info.ireg.num, rtasm_ind8(EBP,
          src1->info.value*4), src2->info.value);
      }
      else ERR;
    }
    break;
    
    case COMPOUND(pal_IREG, pal_RFILE, pal_CONST):
    {
      if (genx86_tab[opcode].r32_rm32_i8)
      {
        genx86_tab[opcode].r32_rm32_i32(nat, dest->info.ireg.num, 
          rtasm_ind8(EBP, src1->info.value*4), src2->info.value);
      }
      else ERR;
    }
    break;

    case COMPOUND(pal_IREG, pal_IREG, pal_CONSTB):
    {
      if (genx86_tab[opcode].r32_rm32_i8)
      {
        genx86_tab[opcode].r32_rm32_i8(nat, dest->info.ireg.num, 
          rtasm_reg(src1->info.ireg.num), src2->info.value);
      }
      else ERR;
    }
    break;

    case COMPOUND(pal_IREG, pal_IREG, pal_CONST):
    {
      if (genx86_tab[opcode].r32_rm32_i8)
      {
        genx86_tab[opcode].r32_rm32_i32(nat, dest->info.ireg.num, 
          rtasm_reg(src1->info.ireg.num), src2->info.value);
      }
      else ERR;
    }
    break;

    case COMPOUND(pal_IREG, pal_STACK, pal_CONSTB):
    {
      if (genx86_tab[opcode].r32_rm32_i8)
      {
        genx86_tab[opcode].r32_rm32_i8(nat, dest->info.ireg.num, rtasm_ind8(ESP,
          src1->info.value*4), src2->info.value);
      }
      else ERR;
    }
    break;
    
    case COMPOUND(pal_IREG, pal_STACK, pal_CONST):
    {
      if (genx86_tab[opcode].r32_rm32_i8)
      {
        genx86_tab[opcode].r32_rm32_i32(nat, dest->info.ireg.num, 
          rtasm_ind8(ESP, src1->info.value*4), src2->info.value);
      }
      else ERR;
    }
    break;
    
    case COMPOUND(pal_CONSTB, pal_CONSTB, pal_UNSET):
    case COMPOUND(pal_CONST, pal_CONST, pal_UNSET):
    {
      if (dest->info.value != src1->info.value)
      {
        ERR;
      }
    }
    break;
    
    default:
    ERR;
  }
}

#undef COMPOUND
#undef ERR

// Won't move something into itself, simplifies zero-loads
void genx86_move(nativeblockinfo* nat, palloc_info* dest, palloc_info* src,
                 uint5 line)
{
  palloc_info nul;
  nul.type = pal_UNSET;
  if (dest->type==src->type)
  {
    switch (dest->type)
    {
      case pal_IREG:
      if (dest->info.ireg.num==src->info.ireg.num) return;
      break;
      
      case pal_RFILE:
      case pal_STACK:
      if (dest->info.value==src->info.value) return;
      break;
      
      default:
      break;
    }
  }
  if ((src->type==pal_CONST || src->type==pal_CONSTB) && dest->type==pal_IREG)
  {
    if (src->info.value==0)
    {
      genx86_out(nat, ab_XOR, dest, dest, &nul, line);
      return;
    }
  }
  genx86_out(nat, ab_MOV, dest, src, &nul, line);
}

nativeblockinfo* genx86_translate(pheta_chunk* chunk, pheta_basicblock* blk,
                                  uint5 startline)
{
  uint5 i;
  nativeblockinfo* nat = x86asm_newnative();
  nativeblockinfo* truebranch = 0, *falsebranch = 0;
  palloc_info nul;
  
  nul.type = pal_UNSET;
  
  for (i=0; i<blk->length; i++)
  {
    uint5 opcode = blk->base[i];
    uint5 line = startline+i;
    
    switch (opcode)
    {
      case ph_FETCH:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        uint5 armsrc = blk->base[i+2];
        switch (dest->type)
        {
          case pal_IREG:
          {
            palloc_info psrc;
            psrc.type = pal_RFILE;
            psrc.info.value = armsrc;
            genx86_out(nat, ab_MOV, dest, &psrc, &nul, line);
          }
          break;
          
          case pal_RFILE:
          {
            dest->info.value = armsrc;
          }
          break;
        }
      }
      break;
      
      case ph_COMMIT:
      {
        uint5 armdest = blk->base[i+1];
        palloc_info* src = &chunk->alloc[blk->base[i+2]];
        if (src->type == pal_IREG)
        {
          palloc_info pdest;
          pdest.type = pal_RFILE;
          pdest.info.value = armdest;
          genx86_out(nat, ab_MOV, &pdest, src, &nul, line);
        }
      }
      break;
      
      case ph_LSL:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src1 = &chunk->alloc[blk->base[i+2]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+3]];
        genx86_move(nat, dest, src1, line);
        genx86_out(nat, ab_SHL, dest, src2, &nul, line);
      }
      break;
      
      case ph_LSR:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src1 = &chunk->alloc[blk->base[i+2]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+3]];
        genx86_move(nat, dest, src1, line);
        genx86_out(nat, ab_SHR, dest, src2, &nul, line);
      }
      break;
      
      case ph_ASR:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src1 = &chunk->alloc[blk->base[i+2]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+3]];
        genx86_move(nat, dest, src1, line);
        genx86_out(nat, ab_SAR, dest, src2, &nul, line);
      }
      break;
      
      case ph_ROR:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src1 = &chunk->alloc[blk->base[i+2]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+3]];
        genx86_move(nat, dest, src1, line);
        genx86_out(nat, ab_ROR, dest, src2, &nul, line);
      }
      break;
      
      case ph_ROL:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src1 = &chunk->alloc[blk->base[i+2]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+3]];
        genx86_move(nat, dest, src1, line);
        genx86_out(nat, ab_ROL, dest, src2, &nul, line);
      }
      break;

      case ph_AND:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src1 = &chunk->alloc[blk->base[i+2]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+3]];
        genx86_move(nat, dest, src1, line);
        genx86_out(nat, ab_AND, dest, src2, &nul, line);
      }
      break;

      case ph_OR:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src1 = &chunk->alloc[blk->base[i+2]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+3]];
        genx86_move(nat, dest, src1, line);
        genx86_out(nat, ab_OR, dest, src2, &nul, line);
      }
      break;

      case ph_EOR:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src1 = &chunk->alloc[blk->base[i+2]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+3]];
        genx86_move(nat, dest, src1, line);
        genx86_out(nat, ab_XOR, dest, src2, &nul, line);
      }
      break;
      
      case ph_ADD:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src1 = &chunk->alloc[blk->base[i+2]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+3]];
        if (dest->type==pal_IREG && src1->type==pal_IREG && 
            src2->type==pal_IREG)
        {
          genx86_out(nat, ab_LEA, dest, src1, src2, line);
        }
        else
        {
          genx86_move(nat, dest, src1, line);
          genx86_out(nat, ab_ADD, dest, src2, &nul, line);
        }
      }
      break;
      
      case ph_ADC:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src1 = &chunk->alloc[blk->base[i+2]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+3]];
        genx86_move(nat, dest, src1, line);
        genx86_out(nat, ab_ADC, dest, src2, &nul, line);
      }
      break;
      
      case ph_SUB:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src1 = &chunk->alloc[blk->base[i+2]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+3]];
        genx86_move(nat, dest, src1, line);
        genx86_out(nat, ab_SUB, dest, src2, &nul, line);
      }
      break;

      case ph_SBC:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src1 = &chunk->alloc[blk->base[i+2]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+3]];
        genx86_move(nat, dest, src1, line);
        genx86_out(nat, ab_SBB, dest, src2, &nul, line);
      }
      break;
      
      case ph_MUL:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src1 = &chunk->alloc[blk->base[i+2]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+3]];
        
        if ((src2->type==pal_CONST || src2->type==pal_CONSTB) && 
            dest->type==pal_IREG)
        {
          genx86_out(nat, ab_IMUL, dest, src1, src2, line);
        }
        else
        {
          genx86_move(nat, dest, src1, line);
          genx86_out(nat, ab_IMUL, dest, src2, &nul, line);
        }
      }
      break;
              
      case ph_RRX:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src = &chunk->alloc[blk->base[i+2]];
        palloc_info one;
        one.type = pal_CONSTB;
        one.info.value = 1;
        genx86_move(nat, dest, src, line);
        genx86_out(nat, ab_RCR, dest, &one, &nul, line);
      }
      break;
      
      case ph_RLX:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src = &chunk->alloc[blk->base[i+2]];
        palloc_info one;
        one.type = pal_CONSTB;
        one.info.value = 1;
        genx86_move(nat, dest, src, line);
        genx86_out(nat, ab_RCL, dest, &one, &nul, line);
      }
      break;
      
      case ph_MOV:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src = &chunk->alloc[blk->base[i+2]];
        genx86_move(nat, dest, src, line);
      }
      break;
      
      case ph_NOT:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src = &chunk->alloc[blk->base[i+2]];
        genx86_move(nat, dest, src, line);
        genx86_out(nat, ab_NOT, dest, &nul, &nul, line);
      }
      break;
      
      case ph_TEQ:  // nasty
      {
        palloc_info* src1 = &chunk->alloc[blk->base[i+1]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+2]];
        genx86_out(nat, ab_PUSH, src1, &nul, &nul, line);
        genx86_out(nat, ab_XOR, src1, src2, &nul, line);
        genx86_out(nat, ab_POP, src1, &nul, &nul, line);
      }
      break;
      
      case ph_TST:
      {
        palloc_info* src1 = &chunk->alloc[blk->base[i+1]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+2]];
        genx86_out(nat, ab_TEST, src1, src2, &nul, line);
      }
      break;
      
      case ph_CMP:
      {
        palloc_info* src1 = &chunk->alloc[blk->base[i+1]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+2]];
        genx86_out(nat, ab_CMP, src1, src2, &nul, line);
      }
      break;
      
      case ph_CMN:  // also nasty
      {
        palloc_info* src1 = &chunk->alloc[blk->base[i+1]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+2]];
        genx86_out(nat, ab_PUSH, src1, &nul, &nul, line);
        genx86_out(nat, ab_ADD, src1, src2, &nul, line);
        genx86_out(nat, ab_POP, src1, &nul, &nul, line);
      }
      break;
      
      case ph_LDW:
      case ph_LDB:
      rtasm_nop(nat);
      break;
      
      case ph_STW:
      case ph_STB:
      rtasm_nop(nat);
      break;
      
      case ph_SWI:
      rtasm_nop(nat);
      break;
      
      case ph_UNDEF:
      rtasm_nop(nat);
      break;
      
      case ph_SYNC:
      break;
      
      case ph_END:
      break;
    }
    
    i += pheta_instlength[opcode];
  }  // for (...)

  phetadism_block(blk);
  x86dism_block(nat);

  blk->marker = 1;

  if (blk->trueblk && !blk->trueblk->marker)
    truebranch = genx86_translate(chunk, blk->trueblk, startline+blk->length);
  
  if (blk->falseblk && !blk->falseblk->marker)
    falsebranch = genx86_translate(chunk, blk->falseblk, startline+blk->length);

  /* join parent block to true/false blocks... */

  return nat;
}
