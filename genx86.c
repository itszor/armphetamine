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

static const genx86_variant genx86_tab[] =
{
  /* shl */ { NULL,
              NULL,
              &rtasm_shl_rm32_imm8,
              NULL,
              NULL,
              &rtasm_shl_rm32_cl,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              ph_C, ph_V|ph_N|ph_Z
            },
  /* shr */ { NULL,
              NULL,
              &rtasm_shr_rm32_imm8,
              NULL,
              NULL,
              &rtasm_shr_rm32_cl,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              ph_C, ph_V|ph_N|ph_Z
            },
  /* sar */ { NULL,
              NULL,
              &rtasm_sar_rm32_imm8,
              NULL,
              NULL,
              &rtasm_sar_rm32_cl,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              ph_C, ph_V|ph_N|ph_Z
            },
  /* ror */ { NULL,
              NULL,
              &rtasm_ror_rm32_imm8,
              NULL,
              NULL,
              &rtasm_ror_rm32_cl,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              ph_C, ph_V
            },
  /* rol */ { NULL,
              NULL,
              &rtasm_rol_rm32_imm8,
              NULL,
              NULL,
              &rtasm_rol_rm32_cl,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              ph_C, ph_V
            },
  /* and */ { &rtasm_and_r32_rm32,
              &rtasm_and_rm32_r32,
              &rtasm_and_rm32_imm8,
              &rtasm_and_rm32_imm32,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              ph_N|ph_Z, ph_C|ph_V
            },
  /* or */  { &rtasm_or_r32_rm32,
              &rtasm_or_rm32_r32,
              &rtasm_or_rm32_imm8,
              &rtasm_or_rm32_imm32,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              ph_N|ph_Z, ph_C|ph_V
            },
  /* xor */ { &rtasm_xor_r32_rm32,
              &rtasm_xor_rm32_r32,
              &rtasm_xor_rm32_imm8,
              &rtasm_xor_rm32_imm32,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              ph_N|ph_Z, ph_C|ph_V
            },
  /* add */ { &rtasm_add_r32_rm32,
              &rtasm_add_rm32_r32,
              &rtasm_add_rm32_imm8,
              &rtasm_add_rm32_imm32,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              ph_C|ph_V|ph_N|ph_Z, 0
            },
  /* adc */ { &rtasm_adc_r32_rm32,
              &rtasm_adc_rm32_r32,
              &rtasm_adc_rm32_imm8,
              &rtasm_adc_rm32_imm32,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              ph_C|ph_V|ph_N|ph_Z, 0
            },
  /* sub */ { &rtasm_sub_r32_rm32,
              &rtasm_sub_rm32_r32,
              &rtasm_sub_rm32_imm8,
              &rtasm_sub_rm32_imm32,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              ph_IC|ph_V|ph_N|ph_Z, 0
            },
  /* sbb */ { &rtasm_sbb_r32_rm32,
              &rtasm_sbb_rm32_r32,
              &rtasm_sbb_rm32_imm8,
              &rtasm_sbb_rm32_imm32,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              ph_IC|ph_V|ph_N|ph_Z, 0
            },
  /* imul */{ &rtasm_imul_r32_rm32,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_imul_r32_rm32_imm8,
              &rtasm_imul_r32_rm32_imm32,
              NULL,
              NULL,
              NULL,
              NULL,
              0, ph_C|ph_V|ph_N|ph_Z
            },
  /* lea */ { &rtasm_lea_r32_m32,
              NULL,
              NULL,
              NULL,
              NULL,
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
              NULL,
              &rtasm_not_rm32,
              NULL,
              NULL,
              NULL,
              0, 0
            },
  /* push */{ NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_push_rm32,
              NULL,
              NULL,
              NULL,
              0, 0
            },
  /* pop */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_pop_rm32,
              NULL,
              NULL,
              NULL,
              0, 0
            },
  /* rcr */ { NULL,
              NULL,
              &rtasm_rcr_rm32_imm8,
              NULL,
              NULL,
              &rtasm_rcr_rm32_cl,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              ph_C, ph_V
            },
  /* rcl */ { NULL,
              NULL,
              &rtasm_rcl_rm32_imm8,
              NULL,
              NULL,
              &rtasm_rcl_rm32_cl,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              ph_C, ph_V
            },
  /* test */{ NULL,
              &rtasm_test_rm32_r32,
              NULL,
              &rtasm_test_rm32_imm32,
              &rtasm_test_rm8_imm8,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              ph_N|ph_Z, ph_C|ph_V
            },
  /* cmp */ { &rtasm_cmp_r32_rm32,
              &rtasm_cmp_rm32_r32,
              NULL,
              &rtasm_cmp_rm32_imm32,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              ph_IC|ph_V|ph_N|ph_Z, 0
            },
  /* ret */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_ret,
              0, 0
            },
 /* seto */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_seto_rm8,
              NULL,
              NULL,
              NULL,
              0, 0
            },
/* setno */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_setno_rm8,
              NULL,
              NULL,
              NULL,
              0, 0
            },
 /* setb */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_setb_rm8,
              NULL,
              NULL,
              NULL,
              0, 0
            },
/* setae */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_setae_rm8,
              NULL,
              NULL,
              NULL,
              0, 0
            },
 /* sete */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_sete_rm8,
              NULL,
              NULL,
              NULL,
              0, 0
            },
/* setne */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_setne_rm8,
              NULL,
              NULL,
              NULL,
              0, 0
            },
/* setbe */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_setbe_rm8,
              NULL,
              NULL,
              NULL,
              0, 0
            },
 /* seta */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_seta_rm8,
              NULL,
              NULL,
              NULL,
              0, 0
            },
 /* sets */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_sets_rm8,
              NULL,
              NULL,
              NULL,
              0, 0
            },
/* setns */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_setns_rm8,
              NULL,
              NULL,
              NULL,
              0, 0
            },
 /* setl */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_setl_rm8,
              NULL,
              NULL,
              NULL,
              0, 0
            },
/* setge */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_setge_rm8,
              NULL,
              NULL,
              NULL,
              0, 0
            },
/* setle */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_setle_rm8,
              NULL,
              NULL,
              NULL,
              0, 0
            },
 /* setg */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_setg_rm8,
              NULL,
              NULL,
              NULL,
              0, 0
            },
   /* jo */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_jo_rel8,
              &rtasm_jo_rel32,
              NULL,
              0, 0
            },
  /* jno */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_jno_rel8,
              &rtasm_jno_rel32,
              NULL,
              0, 0
            },
   /* jb */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_jb_rel8,
              &rtasm_jb_rel32,
              NULL,
              0, 0
            },
  /* jae */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_jae_rel8,
              &rtasm_jae_rel32,
              NULL,
              0, 0
            },
   /* je */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_je_rel8,
              &rtasm_je_rel32,
              NULL,
              0, 0
            },
  /* jne */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_jne_rel8,
              &rtasm_jne_rel32,
              NULL,
              0, 0
            },
  /* jbe */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_jbe_rel8,
              &rtasm_jbe_rel32,
              NULL,
              0, 0
            },
   /* ja */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_ja_rel8,
              &rtasm_ja_rel32,
              NULL,
              0, 0
            },
   /* js */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_js_rel8,
              &rtasm_js_rel32,
              NULL,
              0, 0
            },
  /* jns */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_jns_rel8,
              &rtasm_jns_rel32,
              NULL,
              0, 0
            },
   /* jl */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_jl_rel8,
              &rtasm_jl_rel32,
              NULL,
              0, 0
            },
  /* jge */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_jge_rel8,
              &rtasm_jge_rel32,
              NULL,
              0, 0
            },
  /* jle */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_jle_rel8,
              &rtasm_jle_rel32,
              NULL,
              0, 0
            },
   /* jg */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_jg_rel8,
              &rtasm_jg_rel32,
              NULL,
              0, 0
            },
  /* jmp */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_jmp_rm32,
              &rtasm_jmp_rel8,
              &rtasm_jmp_rel32,
              NULL,
              0, 0
            },
 /* call */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_call_rel32,
              NULL,
              0, ph_C|ph_V|ph_N|ph_Z
            },
/* jecxz */ { NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              &rtasm_jecxz_rel8,
              NULL,
              NULL,
              0, 0
            },
   /* bt */ { NULL,
              &rtasm_bt_rm32_r32,
              &rtasm_bt_rm32_imm8,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL,
              ph_C, 0
            }
};

void genx86_test(void)
{
  nativeblockinfo* nat = rtasm_new();
  
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
  
  rtasm_delete(nat);
}

static const char* allocname[] = {
  "unset", "constb", "const", "rfile", "ireg",
  "stack", "alias", "split"
};

static const char* abname[] = {
  "shl", "shr", "sar", "ror", "rol", "and", "or", "xor", "add", "adc",
  "sub", "sbb", "imul", "lea", "mov", "not", "push", "pop", "rcr", "rcl",
  "test", "cmp", "ret", "seto", "setno", "setb", "setae", "sete", "setne", 
  "setbe", "seta", "sets", "setns", "setl", "setge", "setle", "setg", "jo",
  "jno", "jb", "jae", "je", "jne", "jbe", "ja", "js", "jns", "jl", "jge", "jle",
  "jg", "call", "jecxz", "bt"
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

void genx86_out2(nativeblockinfo* nat, genx86_op inst)
{

}

#define COMPOUND(D,S1,S2) (((D)*pal_NUMTYPES*pal_NUMTYPES) + \
                           ((S1)*pal_NUMTYPES) + (S2))
#define ERR { fprintf(stderr, "Specialisation error at %d. " \
  "Opcode=%s, pattern=%s:%s:%s.\n", \
  __LINE__, abname[opcode], allocname[dest->type], allocname[src1->type], \
  allocname[src2->type]); abort(); }

void genx86_out(nativeblockinfo* nat, uint5 opcode, palloc_info* dest,
                palloc_info* src1, palloc_info* src2, list* map)
{
/*  dest = genx86_closesplit(dest, line);
  src1 = genx86_closesplit(src1, line);
  src2 = genx86_closesplit(src2, line);*/
  
  switch (COMPOUND(dest->type, src1->type, src2->type))
  {
    case COMPOUND(pal_UNSET, pal_UNSET, pal_UNSET):
    {
      if (genx86_tab[opcode].narg)
      {
        genx86_tab[opcode].narg(nat);
      }
      else ERR;
    }
    break;
 
    case COMPOUND(pal_RFILE, pal_UNSET, pal_UNSET):
    {
      if (genx86_tab[opcode].rm32)
      {
        genx86_tab[opcode].rm32(nat, rtasm_ind8(EBP, dest->info.value));
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
        genx86_tab[opcode].rm32(nat, rtasm_ind8(ESP, dest->info.value));
      }
      else ERR;
    }
    break;
    
    case COMPOUND(pal_CONST, pal_UNSET, pal_UNSET):
    {
      if (genx86_tab[opcode].i32)
      {
        genx86_tab[opcode].i32(nat, dest->info.value);
      }
      else ERR;
    }
    break;

    case COMPOUND(pal_CONSTB, pal_UNSET, pal_UNSET):
    {
      if (genx86_tab[opcode].i8)
      {
        genx86_tab[opcode].i8(nat, dest->info.value);
      }
      else ERR;
    }
    break;

    case COMPOUND(pal_RFILE, pal_CONSTB, pal_UNSET):
    {
      uint5 value = src1->info.value;
      if (value<128 && genx86_tab[opcode].rm8_i8)
      {
        // warning: this might break stuff for other opcodes...
        assert(opcode==ab_TEST);
        genx86_tab[opcode].rm8_i8(nat, rtasm_ind8(EBP, dest->info.value),
          value);
      }
      else if (value<128 && genx86_tab[opcode].rm32_i8)
      {
        genx86_tab[opcode].rm32_i8(nat, rtasm_ind8(EBP, dest->info.value), 
          value);
      }
      else if (genx86_tab[opcode].rm32_i32)
      {
        genx86_tab[opcode].rm32_i32(nat, rtasm_ind8(EBP, dest->info.value),
          value);
      }
      else ERR;
    }
    break;
    
    case COMPOUND(pal_RFILE, pal_CONST, pal_UNSET):
    {
      if (genx86_tab[opcode].rm32_i32)
      {
        genx86_tab[opcode].rm32_i32(nat, rtasm_ind8(EBP, dest->info.value),
          src1->info.value);
      }
      else ERR;
    }
    break;
    
    case COMPOUND(pal_RFILE, pal_RFILE, pal_UNSET):
    {
      // this case is nasty. We can't ever generate an x86 instruction with two
      // memory operands, so let's generate some really dumb code to spill and
      // reload a register instead.
      palloc_info temp;
      temp.type = pal_IREG;
      temp.info.ireg.num = EAX;
      rtasm_push_r32(nat, EAX);
      if (opcode!=ab_MOV)
        rtasm_mov_r32_rm32(nat, EAX, rtasm_ind8(EBP, dest->info.value));
      genx86_out(nat, opcode, &temp, src1, src2, map);
      rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value), EAX);
      rtasm_pop_r32(nat, EAX);
    }
    break;
    
    case COMPOUND(pal_RFILE, pal_IREG, pal_UNSET):
    {
      if (genx86_tab[opcode].rm32_r32)
      {
        genx86_tab[opcode].rm32_r32(nat, rtasm_ind8(EBP, dest->info.value),
          src1->info.ireg.num);
      }
      else if (genx86_tab[opcode].rm32_c)
      {
        if (src1->info.ireg.num==ECX)
        {
          genx86_tab[opcode].rm32_c(nat, rtasm_ind8(EBP, dest->info.value));
        }
        else
        {
          rtasm_push_r32(nat, ECX);
          rtasm_mov_r32_rm32(nat, ECX, rtasm_reg(src1->info.ireg.num));
          genx86_tab[opcode].rm32_c(nat, rtasm_ind8(EBP, dest->info.value));
          rtasm_pop_r32(nat, ECX);
        }
      }
      else if (genx86_tab[opcode].r32_rm32)
      {
        // ew!
        // *** xchg reg,[mem] is a BAD INSTRUCTION according to pentium
        // optimisation doc
        rtasm_xchg_r32_rm32(nat, src1->info.ireg.num, rtasm_ind8(EBP,
          dest->info.value));
        genx86_tab[opcode].r32_rm32(nat, src1->info.ireg.num, rtasm_ind8(EBP,
          dest->info.value));
        rtasm_xchg_r32_rm32(nat, src1->info.ireg.num, rtasm_ind8(EBP,
          dest->info.value));
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
          src1->info.value));
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
          src1->info.value));
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
          src1->info.value), src2->info.value);
      }
      else ERR;
    }
    break;
    
    case COMPOUND(pal_IREG, pal_RFILE, pal_CONST):
    {
      if (genx86_tab[opcode].r32_rm32_i8)
      {
        genx86_tab[opcode].r32_rm32_i32(nat, dest->info.ireg.num, 
          rtasm_ind8(EBP, src1->info.value), src2->info.value);
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
      if (genx86_tab[opcode].r32_rm32_i32)
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
          src1->info.value), src2->info.value);
      }
      else ERR;
    }
    break;
    
    case COMPOUND(pal_IREG, pal_STACK, pal_CONST):
    {
      if (genx86_tab[opcode].r32_rm32_i8)
      {
        genx86_tab[opcode].r32_rm32_i32(nat, dest->info.ireg.num, 
          rtasm_ind8(ESP, src1->info.value), src2->info.value);
      }
      else ERR;
    }
    break;
    
    case COMPOUND(pal_CONSTB, pal_CONSTB, pal_UNSET):
    case COMPOUND(pal_CONST, pal_CONST, pal_UNSET):
    {
      // This is OK iff we allocate a constant to a register, then
      // alias the destination of a mov instruction to the source
      if (dest->info.value != src1->info.value)
      {
        ERR;
      }
    }
    break;
    
    default:
    ERR;
  }

  if (nat->expecting)
  {
    nat->beenset |= genx86_tab[opcode].flagset;
  }
}

#undef COMPOUND
#undef ERR

// Won't move something into itself, simplifies zero-loads
void genx86_move(nativeblockinfo* nat, palloc_info* dest, palloc_info* src,
                 list* map)
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
      genx86_out(nat, ab_XOR, dest, dest, &nul, map);
      return;
    }
  }
  genx86_out(nat, ab_MOV, dest, src, &nul, map);
}

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

nativeblockinfo* genx86_translate(pheta_chunk* chunk)
{
  nativeblockinfo* nat = rtasm_new();
  uint5 startline = 0;
  
  genx86_translate_inner(nat, chunk, chunk->root, &startline);
  
  relocate_fix(&nat->reloc, nat->base);
  
  return nat;
}

uint5 genx86_translate_inner(nativeblockinfo* nat,
  pheta_chunk* chunk, pheta_basicblock* blk, uint5* startline)
{
  uint5 i, startpos = nat->length;
  sint5 truebranch = -1, falsebranch = -1;
  palloc_info nul, one, off;
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

  nul.type = pal_UNSET;
  
  for (walk=blk->base->next, i=0; walk->data; walk=walk->next, i++)
  {
    pheta_instr* instr = walk->data;
    uint5 opcode = instr->opcode;
    
    switch (opcode)
    {
      case ph_FETCH:
      {
        palloc_info* dest = &chunk->alloc[instr->data.op.dest];
        uint5 armsrc = instr->data.op.src1;
        switch (dest->type)
        {
          case pal_IREG:
          {
            palloc_info psrc;
            psrc.type = pal_RFILE;
            psrc.info.value = armsrc*4;
            genx86_out(nat, ab_MOV, dest, &psrc, &nul, map);
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
      }
      break;
      
      case ph_COMMIT:
      {
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
            genx86_out(nat, ab_MOV, &pdest, src, &nul, map);
          }
          break;
          
          default:
          break;
        }
      }
      break;
      
      case ph_LSL:
      {
        palloc_info* dest = &chunk->alloc[instr->data.op.dest];
        palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
        palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
        genx86_move(nat, dest, src1, map);
        genx86_out(nat, ab_SHL, dest, src2, &nul, map);
      }
      break;
      
      case ph_LSR:
      {
        palloc_info* dest = &chunk->alloc[instr->data.op.dest];
        palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
        palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
        genx86_move(nat, dest, src1, map);
        genx86_out(nat, ab_SHR, dest, src2, &nul, map);
      }
      break;
      
      case ph_ASR:
      {
        palloc_info* dest = &chunk->alloc[instr->data.op.dest];
        palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
        palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
        genx86_move(nat, dest, src1, map);
        genx86_out(nat, ab_SAR, dest, src2, &nul, map);
      }
      break;
      
      case ph_ROR:
      {
        palloc_info* dest = &chunk->alloc[instr->data.op.dest];
        palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
        palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
        genx86_move(nat, dest, src1, map);
        genx86_out(nat, ab_ROR, dest, src2, &nul, map);
      }
      break;
      
      case ph_ROL:
      {
        palloc_info* dest = &chunk->alloc[instr->data.op.dest];
        palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
        palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
        genx86_move(nat, dest, src1, map);
        genx86_out(nat, ab_ROL, dest, src2, &nul, map);
      }
      break;

      case ph_AND:
      {
        palloc_info* dest = &chunk->alloc[instr->data.op.dest];
        palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
        palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
        genx86_move(nat, dest, src1, map);
        genx86_out(nat, ab_AND, dest, src2, &nul, map);
      }
      break;

      case ph_OR:
      {
        palloc_info* dest = &chunk->alloc[instr->data.op.dest];
        palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
        palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
        genx86_move(nat, dest, src1, map);
        genx86_out(nat, ab_OR, dest, src2, &nul, map);
      }
      break;

      case ph_EOR:
      {
        palloc_info* dest = &chunk->alloc[instr->data.op.dest];
        palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
        palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
        genx86_move(nat, dest, src1, map);
        genx86_out(nat, ab_XOR, dest, src2, &nul, map);
      }
      break;
      
      case ph_ADD:
      {
        palloc_info* dest = &chunk->alloc[instr->data.op.dest];
        palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
        palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
        if (dest->type==pal_IREG && src1->type==pal_IREG && 
            src2->type==pal_IREG && nat->expecting==0)
        {
          genx86_out(nat, ab_LEA, dest, src1, src2, map);
        }
        else
        {
          genx86_move(nat, dest, src1, map);
          genx86_out(nat, ab_ADD, dest, src2, &nul, map);
        }
      }
      break;
      
      case ph_ADC:
      {
        palloc_info* dest = &chunk->alloc[instr->data.op.dest];
        palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
        palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
        genx86_move(nat, dest, src1, map);
        genx86_out(nat, ab_ADC, dest, src2, &nul, map);
      }
      break;
      
      case ph_SUB:
      {
        palloc_info* dest = &chunk->alloc[instr->data.op.dest];
        palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
        palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
        genx86_move(nat, dest, src1, map);
        genx86_out(nat, ab_SUB, dest, src2, &nul, map);
      }
      break;

      case ph_SBC:
      {
        palloc_info* dest = &chunk->alloc[instr->data.op.dest];
        palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
        palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
        genx86_move(nat, dest, src1, map);
        genx86_out(nat, ab_SBB, dest, src2, &nul, map);
      }
      break;
      
      case ph_MUL:
      {
        palloc_info* dest = &chunk->alloc[instr->data.op.dest];
        palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
        palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
        
        if ((src2->type==pal_CONST || src2->type==pal_CONSTB) && 
            dest->type==pal_IREG)
        {
          genx86_out(nat, ab_IMUL, dest, src1, src2, map);
        }
        else
        {
          genx86_move(nat, dest, src1, map);
          genx86_out(nat, ab_IMUL, dest, src2, &nul, map);
        }
      }
      break;
              
      case ph_RRX:
      {
        palloc_info* dest = &chunk->alloc[instr->data.op.dest];
        palloc_info* src = &chunk->alloc[instr->data.op.src1];
        palloc_info one;
        one.type = pal_CONSTB;
        one.info.value = 1;
        genx86_move(nat, dest, src, map);
        genx86_out(nat, ab_RCR, dest, &one, &nul, map);
      }
      break;
      
      case ph_RLX:
      {
        palloc_info* dest = &chunk->alloc[instr->data.op.dest];
        palloc_info* src = &chunk->alloc[instr->data.op.src1];
        palloc_info one;
        one.type = pal_CONSTB;
        one.info.value = 1;
        genx86_move(nat, dest, src, map);
        genx86_out(nat, ab_RCL, dest, &one, &nul, map);
      }
      break;
      
      case ph_MOV:
      {
        palloc_info* dest = &chunk->alloc[instr->data.op.dest];
        palloc_info* src = &chunk->alloc[instr->data.op.src1];
        genx86_move(nat, dest, src, map);
      }
      break;
      
      case ph_NOT:
      {
        palloc_info* dest = &chunk->alloc[instr->data.op.dest];
        palloc_info* src = &chunk->alloc[instr->data.op.src1];
        genx86_move(nat, dest, src, map);
        genx86_out(nat, ab_NOT, dest, &nul, &nul, map);
      }
      break;
      
      case ph_TEQ:  // nasty
      {
        palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
        palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
        genx86_out(nat, ab_PUSH, src1, &nul, &nul, map);
        genx86_out(nat, ab_XOR, src1, src2, &nul, map);
        genx86_out(nat, ab_POP, src1, &nul, &nul, map);
      }
      break;
      
      case ph_TST:
      {
        palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
        palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
        genx86_out(nat, ab_TEST, src1, src2, &nul, map);
      }
      break;
      
      case ph_CMP:
      {
        palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
        palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
        genx86_out(nat, ab_CMP, src1, src2, &nul, map);
      }
      break;
      
      case ph_CMN:  // also nasty
      {
        palloc_info* src1 = &chunk->alloc[instr->data.op.src1];
        palloc_info* src2 = &chunk->alloc[instr->data.op.src2];
        genx86_out(nat, ab_PUSH, src1, &nul, &nul, map);
        genx86_out(nat, ab_ADD, src1, src2, &nul, map);
        genx86_out(nat, ab_POP, src1, &nul, &nul, map);
      }
      break;
      
      case ph_FEXPECT:
      case ph_NFEXPECT:
      {
        uint5 expmask = instr->data.flag.need;
        nat->expecting |= expmask;
        nat->beenset = 0;
      }
      break;
      
      case ph_FCOMMIT:
      {
        uint5 needmask = instr->data.flag.need;
        uint5 pred = instr->data.flag.pred;
        palloc_info off;
        off.type = pal_RFILE;
        if (needmask & ph_C)
        {
          off.info.value = offsetof(registerinfo, cflag);
          if (nat->beenset & ph_IC)
            genx86_out(nat, ab_SETAE, &off, &nul, &nul, map);
          else if (nat->beenset & ph_C)
            genx86_out(nat, ab_SETB, &off, &nul, &nul, map);
          else
            assert(!"Carry flag has not been set");
        }
        if (needmask & ph_V)
        {
          off.info.value = offsetof(registerinfo, vflag);
          genx86_out(nat, ab_SETO, &off, &nul, &nul, map);
        }
        if (needmask & ph_N)
        {
          off.info.value = offsetof(registerinfo, nflag);
          genx86_out(nat, ab_SETS, &off, &nul, &nul, map);
        }
        if (needmask & ph_Z)
        {
          off.info.value = offsetof(registerinfo, zflag);
          genx86_out(nat, ab_SETE, &off, &nul, &nul, map);
        }
        if (pred)
        {
          uint5 j;
          for (j=0; j<14; j++)
          {
            if (pred & (1<<j))
            {
              off.info.value = offsetof(registerinfo, predbuf[j]);
              genx86_out(nat, predset[j], &off, &nul, &nul, map);
            }
          }
        }
        nat->expecting = 0;
      }
      break;
      
      case ph_NFCOMMIT:
      {
        uint5 pred = instr->data.flag.pred;
        
        off.type = pal_RFILE;
        if (pred)
        {
          uint5 j;
          for (j=0; j<14; j++)
          {
            if (pred & (1<<j))
            {
              off.info.value = offsetof(registerinfo, npredbuf[j]);
              genx86_out(nat, predset[j], &off, &nul, &nul, map);
            }
          }
        }
      }
      break;
      
      case ph_FENSURE:
      {
        uint5 mask = instr->data.flag.need;
        palloc_info off;
        if (mask)
        {
          if (mask==ph_C || mask==ph_IC)
          {
            palloc_info zero;
            off.type = pal_RFILE;
            off.info.value = offsetof(registerinfo, cflag);
            zero.type = pal_CONSTB;
            zero.info.value = 0;
            genx86_out(nat, ab_BT, &off, &zero, &nul, map);
            if (mask==ph_IC)
            {
              assert(!"I wasn't expecting you to ensure inverse C!");
//              genx86_out(nat, ab_CMC, &off, &nul, &nul, map);
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
      
      case ph_LDW:
      {
        palloc_info rel;
        rel.type = pal_CONST;
        rel.info.value = 0;
        /*  memory structure base
         *  address
         */
        genx86_out(nat, ab_CALL, &rel, &nul, &nul, map);
        rel.type = pal_CONSTB;
        rel.info.value = 6;
        genx86_out(nat, ab_JECXZ, &rel, &nul, &nul, map);
        genx86_preserve(nat, chunk, map);
        rel.type = pal_CONST;
        rel.info.value = 0;
        genx86_out(nat, ab_CALL, &rel, &nul, &nul, map);
        genx86_out(nat, ab_RET, &nul, &nul, &nul, map);
      }
      break;
      
      case ph_LDB:
      {
        palloc_info rel;
        rel.type = pal_CONST;
        rel.info.value = 0;
        /*  memory structure base
         *  address
         */
        genx86_out(nat, ab_CALL, &rel, &nul, &nul, map);
        rel.type = pal_CONSTB;
        rel.info.value = 6;
        genx86_out(nat, ab_JECXZ, &rel, &nul, &nul, map);
        genx86_preserve(nat, chunk, map);
        rel.type = pal_CONST;
        rel.info.value = 0;
        genx86_out(nat, ab_CALL, &rel, &nul, &nul, map);
        genx86_out(nat, ab_RET, &nul, &nul, &nul, map);
      }
      break;
      
      case ph_STW:
      case ph_STB:
      {
        palloc_info rel;
        rel.type = pal_CONST;
        rel.info.value = 0;
        /*  memory structure base
         *  address
         */
        genx86_out(nat, ab_CALL, &rel, &nul, &nul, map);
        rel.type = pal_CONSTB;
        rel.info.value = 6;
        genx86_out(nat, ab_JECXZ, &rel, &nul, &nul, map);
        genx86_preserve(nat, chunk, map);
        rel.type = pal_CONST;
        rel.info.value = 0;
        genx86_out(nat, ab_CALL, &rel, &nul, &nul, map);
        genx86_out(nat, ab_RET, &nul, &nul, &nul, map);
      }
      break;
      
      case ph_SWI:
      rtasm_nop(nat);
      break;
      
      case ph_UNDEF:
      rtasm_nop(nat);
      break;

      case ph_STATE:
      {
        uint5 start = instr->data.imm;
        map = (list*)start;
      }
      break;
      
      case ph_SYNC:
      // sync needs to store all allocated registers back to memory, but that
      // information isn't available here... damn.
      break;
      
      // !!! this will actually have to do other stuff instead, esp. in
      // 26-bit mode.
      case ph_CAJMP:
      {
        palloc_info dest, c8;
        dest.type = pal_RFILE;
        dest.info.value = 15*4;
        c8.type = pal_CONSTB;
        c8.info.value = 8;
        genx86_out(nat, ab_ADD, &dest, &c8, &nul, map);
        genx86_out(nat, ab_RET, &nul, &nul, &nul, map);
      }
      break;
      // !!! things missing
    }
  }  // for (...)

  phetadism_block(blk, *startline);
  x86dism_partblock(nat, startpos, nat->length);

  blk->marker = 1;
  blk->natoffset = startpos;

  (*startline) += blk->length;

  // true & false blocks, neither generated yet
  if (blk->trueblk && !blk->trueblk->marker &&
      blk->falseblk && !blk->falseblk->marker)
  {
    one.type = pal_CONSTB;
    one.info.value = 1;
    off.type = pal_RFILE;
    off.info.value = offsetof(registerinfo, predbuf[blk->predicate]);
    genx86_out(nat, ab_TEST, &off, &one, &nul, map);
    off.type = pal_CONST;
    off.info.value = -(nat->length+6);
    genx86_out(nat, ab_JE, &off, &nul, &nul, map);
    patchfalseblk = nat->length-4;
  }
  
  // generated true block, non-generated false block
  if (blk->trueblk && blk->trueblk->marker &&
      blk->falseblk && !blk->falseblk->marker)
  {
    one.type = pal_CONSTB;
    one.info.value = 1;
    off.type = pal_RFILE;
    off.info.value = offsetof(registerinfo, predbuf[blk->predicate]);
    genx86_out(nat, ab_TEST, &off, &one, &nul, map);
    off.type = pal_CONST;
    off.info.value = blk->trueblk->natoffset-nat->length-6;
    genx86_out(nat, ab_JNE, &off, &nul, &nul, map);
  }
  
  // generated true & false blocks
  if (blk->trueblk && blk->trueblk->marker &&
      blk->falseblk && blk->falseblk->marker)
  {
    one.type = pal_CONSTB;
    one.info.value = 1;
    off.type = pal_RFILE;
    off.info.value = offsetof(registerinfo, predbuf[blk->predicate]);
    genx86_out(nat, ab_TEST, &off, &one, &nul, map);
    off.type = pal_CONST;
    off.info.value = blk->trueblk->natoffset-nat->length-6;
    genx86_out(nat, ab_JNE, &off, &nul, &nul, map);
    off.info.value = blk->falseblk->natoffset-nat->length-6;
    genx86_out(nat, ab_JMP, &off, &nul, &nul, map);
  }
  
  if (blk->trueblk && blk->trueblk->marker &&
      !blk->falseblk)
  {
    off.type = pal_CONST;
    off.info.value = blk->trueblk->natoffset-nat->length-6;
    genx86_out(nat, ab_JMP, &off, &nul, &nul, map);
  }

  if (blk->trueblk && !blk->trueblk->marker)
    truebranch = genx86_translate_inner(nat, chunk, blk->trueblk, startline);

  if (blk->falseblk && !blk->falseblk->marker)
    falsebranch = genx86_translate_inner(nat, chunk, blk->falseblk, startline);

  if (patchfalseblk != -1)
  {
    if (falsebranch==-1) falsebranch = blk->falseblk->natoffset;
    fprintf(stderr, "Adding relocation offset %x value %x\n", patchfalseblk,
      falsebranch);
    relocate_add(&nat->reloc, falsebranch, patchfalseblk, relsize_WORD, 
      reloc_RELATIVE);
  }

  return startpos;
}
