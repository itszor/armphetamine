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

static const char* otype[] = { "empty", "immediate", "register",
  "indirect register", "ind register+8bit disp", "ind register+32bit disp",
  "indirect base+offset", "indirect base+offset+disp8",
  "indirect base+offset+disp32", "address" };

#define COMPOUND2(A,B) ((A)*gotype_NUMTYPES+(B))

#define COMPOUND3(A,B,C) \
  ((A)*gotype_NUMTYPES*gotype_NUMTYPES+(B)*gotype_NUMTYPES+(C))

#define ERR { fprintf(stderr, "Broke at line %d\n" \
  "Operator=%s op[0]=%s op[1]=%s op[2]=%s\n", __LINE__, abname[inst->operator],\
   otype[inst->op[0]->type], otype[inst->op[1]->type], \
   otype[inst->op[2]->type]); abort(); }

void genx86_asm(nativeblockinfo* nat, genx86_op* inst)
{
  uint5 ops = 0, opcode = inst->operator;

  if (inst->op[0]) ops++;
  if (inst->op[1]) ops++;
  if (inst->op[2]) ops++;

  if (ops==0)
  {
    if (genx86_tab[opcode].narg)
    {
      genx86_tab[opcode].narg(nat);
    }
    else ERR;
  }
  else
  {
    switch (inst->op[0]->type)
    {
      case gotype_IMMEDIATE:
      {
        if (ops==1)
        {
          switch (inst->op[0]->width)
          {
            case gowidth_BYTE:
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

            case gowidth_DWORD:
            {
              if (genx86_tab[opcode].i32)
              {
                genx86_tab[opcode].i32(nat, inst->op[0]->data.imm);
              }
              else ERR;
            }
            break;

            default:
            ERR;
            break;
          }
        }
      }
      break;  // immediate

      case gotype_REGISTER:
      {
        switch (ops)
        {
          case 1:
          {
            if (genx86_tab[opcode].rm32)
            {
              genx86_tab[opcode].rm32(nat, rtasm_reg(inst->op[0]->data.reg));
            }
            else ERR;
          }
          break;
          
          case 2:
          {
            switch (inst->op[1]->type)
            {
              case gotype_REGISTER:
              {
                if (genx86_tab[opcode].r32_rm32)
                {
                  genx86_tab[opcode].r32_rm32(nat, inst->op[0]->data.reg,
                    rtasm_reg(inst->op[1]->data.reg));
                }
                else if (genx86_tab[opcode].rm32_r32)
                {
                  genx86_tab[opcode].rm32_r32(nat, 
                    rtasm_reg(inst->op[0]->data.reg), inst->op[1]->data.reg);
                }
                else if (inst->op[1]->data.reg==ECX && 
                         genx86_tab[opcode].rm32_c)
                {
                  genx86_tab[opcode].rm32_c(nat,
                    rtasm_reg(inst->op[0]->data.reg));
                }
                else ERR;
              }
              break;
              
              case gotype_IMMEDIATE:
              {
                switch (inst->op[1]->width)
                {
                  case gowidth_BYTE:
                  {
                    if (genx86_tab[opcode].rm32_i8)
                    {
                      genx86_tab[opcode].rm32_i8(nat,
                        rtasm_reg(inst->op[0]->data.reg), 
                        inst->op[1]->data.imm);
                    }
                  }
                  break;
                  
                  case gowidth_DWORD:
                  {
                    if (genx86_tab[opcode].rm32_i32)
                    {
                      genx86_tab[opcode].rm32_i32(nat,
                        rtasm_reg(inst->op[0]->data.reg),
                        inst->op[1]->data.imm);
                    }
                  }
                  break;
                  
                  default: ERR;
                  break;
                }
              }
              break;
              
              case gotype_INDREG:
              {
                if (genx86_tab[opcode].r32_rm32)
                {
                  genx86_tab[opcode].r32_rm32(nat, inst->op[0]->data.reg,
                    rtasm_ind(inst->op[1]->data.reg));
                }
              }
              break;
              
              case gotype_INDREGPLUSDISP8:
              {
                if (genx86_tab[opcode].r32_rm32)
                {
                  genx86_tab[opcode].r32_rm32(nat, inst->op[0]->data.reg,
                    rtasm_ind8(inst->op[1]->data.regdisp.base,
                    inst->op[1]->data.regdisp.disp));
                }
              }
              break;
              
              case gotype_INDREGPLUSDISP32:
              {
                if (genx86_tab[opcode].r32_rm32)
                {
                  genx86_tab[opcode].r32_rm32(nat, inst->op[0]->data.reg,
                    rtasm_ind32(inst->op[1]->data.regdisp.base,
                    inst->op[1]->data.regdisp.disp));
                }
              }
              break;
              
              case gotype_INDREGPLUSSCALEDREG:
              {
                if (genx86_tab[opcode].r32_rm32)
                {
                  genx86_tab[opcode].r32_rm32(nat, inst->op[0]->data.reg,
                    rtasm_scind(inst->op[1]->data.regscale.base,
                    inst->op[1]->data.regscale.index,
                    inst->op[1]->data.regscale.scale));
                }
              }
              break;
              
              case gotype_INDREGPLUSSCALEDREGPLUSDISP8:
              {
                if (genx86_tab[opcode].r32_rm32)
                {
                  genx86_tab[opcode].r32_rm32(nat, inst->op[0]->data.reg,
                    rtasm_scind8(inst->op[1]->data.regscaledisp.base,
                    inst->op[1]->data.regscaledisp.index,
                    inst->op[1]->data.regscaledisp.scale,
                    inst->op[1]->data.regscaledisp.offset));
                }
              }
              break;
              
              case gotype_INDREGPLUSSCALEDREGPLUSDISP32:
              {
                if (genx86_tab[opcode].r32_rm32)
                {
                  genx86_tab[opcode].r32_rm32(nat, inst->op[0]->data.reg,
                    rtasm_scind32(inst->op[1]->data.regscaledisp.base,
                    inst->op[1]->data.regscaledisp.index,
                    inst->op[1]->data.regscaledisp.scale,
                    inst->op[1]->data.regscaledisp.offset));
                }
              }
              break;
              
              default:
              ERR;
              break;
            }  // switch (op[1]->type)
          }
          break;
          
          case 3:
          {
            if (inst->op[2]->type==gotype_IMMEDIATE)
            {
              switch (inst->op[2]->width)
              {
                case gowidth_BYTE:
                {
                  switch (inst->op[1]->type)
                  {
                    case gotype_REGISTER:
                    {
                      if (genx86_tab[opcode].r32_rm32)
                      {
                        genx86_tab[opcode].r32_rm32_i8(nat, 
                          inst->op[0]->data.reg,
                          rtasm_reg(inst->op[1]->data.reg),
                          inst->op[2]->data.imm);
                      }
                    }
                    break;

                    case gotype_INDREG:
                    {
                      if (genx86_tab[opcode].r32_rm32)
                      {
                        genx86_tab[opcode].r32_rm32_i8(nat, 
                          inst->op[0]->data.reg,
                          rtasm_ind(inst->op[1]->data.reg),
                          inst->op[2]->data.imm);
                      }
                    }
                    break;

                    case gotype_INDREGPLUSDISP8:
                    {
                      if (genx86_tab[opcode].r32_rm32)
                      {
                        genx86_tab[opcode].r32_rm32_i8(nat, 
                          inst->op[0]->data.reg,
                          rtasm_ind8(inst->op[1]->data.regdisp.base,
                            inst->op[1]->data.regdisp.disp),
                          inst->op[2]->data.imm);
                      }
                    }
                    break;

                    case gotype_INDREGPLUSDISP32:
                    {
                      if (genx86_tab[opcode].r32_rm32)
                      {
                        genx86_tab[opcode].r32_rm32_i8(nat, 
                          inst->op[0]->data.reg,
                          rtasm_ind32(inst->op[1]->data.regdisp.base,
                            inst->op[1]->data.regdisp.disp),
                          inst->op[2]->data.imm);
                      }
                    }
                    break;

                    case gotype_INDREGPLUSSCALEDREG:
                    {
                      if (genx86_tab[opcode].r32_rm32)
                      {
                        genx86_tab[opcode].r32_rm32_i8(nat, 
                          inst->op[0]->data.reg,
                          rtasm_scind(inst->op[1]->data.regscale.base,
                            inst->op[1]->data.regscale.index,
                            inst->op[1]->data.regscale.scale),
                          inst->op[2]->data.imm);
                      }
                    }
                    break;

                    case gotype_INDREGPLUSSCALEDREGPLUSDISP8:
                    {
                      if (genx86_tab[opcode].r32_rm32)
                      {
                        genx86_tab[opcode].r32_rm32_i8(nat, 
                          inst->op[0]->data.reg,
                          rtasm_scind8(inst->op[1]->data.regscaledisp.base,
                            inst->op[1]->data.regscaledisp.index,
                            inst->op[1]->data.regscaledisp.scale,
                            inst->op[1]->data.regscaledisp.offset),
                          inst->op[2]->data.imm);
                      }
                    }
                    break;

                    case gotype_INDREGPLUSSCALEDREGPLUSDISP32:
                    {
                      if (genx86_tab[opcode].r32_rm32)
                      {
                        genx86_tab[opcode].r32_rm32_i8(nat, 
                          inst->op[0]->data.reg,
                          rtasm_scind32(inst->op[1]->data.regscaledisp.base,
                            inst->op[1]->data.regscaledisp.index,
                            inst->op[1]->data.regscaledisp.scale,
                            inst->op[1]->data.regscaledisp.offset),
                          inst->op[2]->data.imm);
                      }
                    }
                    break;
                    
                    default:
                    ERR;
                    break;
                  }  // switch (op1 type)
                }
                break;
                
                case gowidth_DWORD:
                {
                  switch (inst->op[1]->type)
                  {
                    case gotype_REGISTER:
                    {
                      if (genx86_tab[opcode].r32_rm32)
                      {
                        genx86_tab[opcode].r32_rm32_i32(nat, 
                          inst->op[0]->data.reg,
                          rtasm_reg(inst->op[1]->data.reg),
                          inst->op[2]->data.imm);
                      }
                    }
                    break;

                    case gotype_INDREG:
                    {
                      if (genx86_tab[opcode].r32_rm32)
                      {
                        genx86_tab[opcode].r32_rm32_i32(nat, 
                          inst->op[0]->data.reg,
                          rtasm_ind(inst->op[1]->data.reg),
                          inst->op[2]->data.imm);
                      }
                    }
                    break;

                    case gotype_INDREGPLUSDISP8:
                    {
                      if (genx86_tab[opcode].r32_rm32)
                      {
                        genx86_tab[opcode].r32_rm32_i32(nat, 
                          inst->op[0]->data.reg,
                          rtasm_ind8(inst->op[1]->data.regdisp.base,
                            inst->op[1]->data.regdisp.disp),
                          inst->op[2]->data.imm);
                      }
                    }
                    break;

                    case gotype_INDREGPLUSDISP32:
                    {
                      if (genx86_tab[opcode].r32_rm32)
                      {
                        genx86_tab[opcode].r32_rm32_i32(nat, 
                          inst->op[0]->data.reg,
                          rtasm_ind32(inst->op[1]->data.regdisp.base,
                            inst->op[1]->data.regdisp.disp),
                          inst->op[2]->data.imm);
                      }
                    }
                    break;

                    case gotype_INDREGPLUSSCALEDREG:
                    {
                      if (genx86_tab[opcode].r32_rm32)
                      {
                        genx86_tab[opcode].r32_rm32_i32(nat, 
                          inst->op[0]->data.reg,
                          rtasm_scind(inst->op[1]->data.regscale.base,
                            inst->op[1]->data.regscale.index,
                            inst->op[1]->data.regscale.scale),
                          inst->op[2]->data.imm);
                      }
                    }
                    break;

                    case gotype_INDREGPLUSSCALEDREGPLUSDISP8:
                    {
                      if (genx86_tab[opcode].r32_rm32)
                      {
                        genx86_tab[opcode].r32_rm32_i32(nat, 
                          inst->op[0]->data.reg,
                          rtasm_scind8(inst->op[1]->data.regscaledisp.base,
                            inst->op[1]->data.regscaledisp.index,
                            inst->op[1]->data.regscaledisp.scale,
                            inst->op[1]->data.regscaledisp.offset),
                          inst->op[2]->data.imm);
                      }
                    }
                    break;

                    case gotype_INDREGPLUSSCALEDREGPLUSDISP32:
                    {
                      if (genx86_tab[opcode].r32_rm32)
                      {
                        genx86_tab[opcode].r32_rm32_i32(nat, 
                          inst->op[0]->data.reg,
                          rtasm_scind32(inst->op[1]->data.regscaledisp.base,
                            inst->op[1]->data.regscaledisp.index,
                            inst->op[1]->data.regscaledisp.scale,
                            inst->op[1]->data.regscaledisp.offset),
                          inst->op[2]->data.imm);
                      }
                    }
                    break;
                    
                    default:
                    ERR;
                    break;
                  }  // switch (op1 type)
                }
                break;
                
                default:
                ERR;
                break;
              }
            }
          }
          break;
        }  // switch (ops)
      }
      break;

      case gotype_INDREG:
      case gotype_INDREGPLUSDISP8:
      case gotype_INDREGPLUSDISP32:
      case gotype_INDREGPLUSSCALEDREG:
      case gotype_INDREGPLUSSCALEDREGPLUSDISP8:
      case gotype_INDREGPLUSSCALEDREGPLUSDISP32:
      {
        switch (ops)
        {
          case 1:
          {
            if (genx86_tab[opcode].rm32)
            {
              switch (inst->op[0]->type)
              {
                case gotype_INDREG:
                {
                  genx86_tab[opcode].rm32(nat, 
                    rtasm_ind(inst->op[0]->data.reg));
                }
                break;

                case gotype_INDREGPLUSDISP8:
                {
                  genx86_tab[opcode].rm32(nat, 
                    rtasm_ind8(inst->op[0]->data.regdisp.base, 
                    inst->op[0]->data.regdisp.disp));
                }
                break;
                
                case gotype_INDREGPLUSDISP32:
                {
                  genx86_tab[opcode].rm32(nat, 
                    rtasm_ind32(inst->op[0]->data.regdisp.base, 
                    inst->op[0]->data.regdisp.disp));
                }
                break;
                
                case gotype_INDREGPLUSSCALEDREG:
                {
                  genx86_tab[opcode].rm32(nat, 
                    rtasm_scind(inst->op[0]->data.regscale.base,
                    inst->op[0]->data.regscale.index,
                    inst->op[0]->data.regscale.scale));
                }
                break;
                
                case gotype_INDREGPLUSSCALEDREGPLUSDISP8:
                {
                  genx86_tab[opcode].rm32(nat, rtasm_scind8(
                    inst->op[0]->data.regscaledisp.base,
                    inst->op[0]->data.regscaledisp.index,
                    inst->op[0]->data.regscaledisp.scale,
                    inst->op[0]->data.regscaledisp.offset));
                }
                break;

                case gotype_INDREGPLUSSCALEDREGPLUSDISP32:
                {
                  genx86_tab[opcode].rm32(nat, rtasm_scind32(
                    inst->op[0]->data.regscaledisp.base,
                    inst->op[0]->data.regscaledisp.index,
                    inst->op[0]->data.regscaledisp.scale,
                    inst->op[0]->data.regscaledisp.offset));
                }
                break;
                
                default:
                ERR;
                break;
              }
            }
            else ERR;
          }
          break;
          
          case 2:
          {
            switch (inst->op[1]->type)
            {
              case gotype_IMMEDIATE:
              {
                switch (inst->op[1]->width)
                {
                  case gowidth_BYTE:
                  {
                    switch (inst->op[0]->width)
                    {
                      case gowidth_BYTE:
                      {
                        if (genx86_tab[opcode].rm8_i8)
                        {
                          switch (inst->op[0]->type)
                          {
                            case gotype_INDREG:
                            {
                              genx86_tab[opcode].rm8_i8(nat, 
                                rtasm_ind(inst->op[0]->data.reg), 
                                inst->op[1]->data.imm);
                            }
                            break;

                            case gotype_INDREGPLUSDISP8:
                            {
                              genx86_tab[opcode].rm8_i8(nat, 
                                rtasm_ind8(inst->op[0]->data.regdisp.base, 
                                inst->op[0]->data.regdisp.disp),
                                inst->op[1]->data.imm);
                            }
                            break;

                            case gotype_INDREGPLUSDISP32:
                            {
                              genx86_tab[opcode].rm8_i8(nat, 
                                rtasm_ind32(inst->op[0]->data.regdisp.base, 
                                inst->op[0]->data.regdisp.disp),
                                inst->op[1]->data.imm);
                            }
                            break;

                            case gotype_INDREGPLUSSCALEDREG:
                            {
                              genx86_tab[opcode].rm8_i8(nat, 
                                rtasm_scind(inst->op[0]->data.regscale.base,
                                inst->op[0]->data.regscale.index,
                                inst->op[0]->data.regscale.scale),
                                inst->op[1]->data.imm);
                            }
                            break;

                            case gotype_INDREGPLUSSCALEDREGPLUSDISP8:
                            {
                              genx86_tab[opcode].rm8_i8(nat, rtasm_scind8(
                                inst->op[0]->data.regscaledisp.base,
                                inst->op[0]->data.regscaledisp.index,
                                inst->op[0]->data.regscaledisp.scale,
                                inst->op[0]->data.regscaledisp.offset),
                                inst->op[1]->data.imm);
                            }
                            break;

                            case gotype_INDREGPLUSSCALEDREGPLUSDISP32:
                            {
                              genx86_tab[opcode].rm8_i8(nat, rtasm_scind32(
                                inst->op[0]->data.regscaledisp.base,
                                inst->op[0]->data.regscaledisp.index,
                                inst->op[0]->data.regscaledisp.scale,
                                inst->op[0]->data.regscaledisp.offset),
                                inst->op[1]->data.imm);
                            }
                            break;
                            
                            default:
                            ERR;
                            break;
                          }
                        }
                      }
                      break;
                    
                      case gowidth_DWORD:
                      {
                        if (genx86_tab[opcode].rm32_i8)
                        {
                          switch (inst->op[0]->type)
                          {
                            case gotype_INDREG:
                            {
                              genx86_tab[opcode].rm32_i8(nat, 
                                rtasm_ind(inst->op[0]->data.reg), 
                                inst->op[1]->data.imm);
                            }
                            break;

                            case gotype_INDREGPLUSDISP8:
                            {
                              genx86_tab[opcode].rm32_i8(nat, 
                                rtasm_ind8(inst->op[0]->data.regdisp.base, 
                                inst->op[0]->data.regdisp.disp),
                                inst->op[1]->data.imm);
                            }
                            break;

                            case gotype_INDREGPLUSDISP32:
                            {
                              genx86_tab[opcode].rm32_i8(nat, 
                                rtasm_ind32(inst->op[0]->data.regdisp.base, 
                                inst->op[0]->data.regdisp.disp),
                                inst->op[1]->data.imm);
                            }
                            break;

                            case gotype_INDREGPLUSSCALEDREG:
                            {
                              genx86_tab[opcode].rm32_i8(nat, 
                                rtasm_scind(inst->op[0]->data.regscale.base,
                                inst->op[0]->data.regscale.index,
                                inst->op[0]->data.regscale.scale),
                                inst->op[1]->data.imm);
                            }
                            break;

                            case gotype_INDREGPLUSSCALEDREGPLUSDISP8:
                            {
                              genx86_tab[opcode].rm32_i8(nat, rtasm_scind8(
                                inst->op[0]->data.regscaledisp.base,
                                inst->op[0]->data.regscaledisp.index,
                                inst->op[0]->data.regscaledisp.scale,
                                inst->op[0]->data.regscaledisp.offset),
                                inst->op[1]->data.imm);
                            }
                            break;

                            case gotype_INDREGPLUSSCALEDREGPLUSDISP32:
                            {
                              genx86_tab[opcode].rm32_i8(nat, rtasm_scind32(
                                inst->op[0]->data.regscaledisp.base,
                                inst->op[0]->data.regscaledisp.index,
                                inst->op[0]->data.regscaledisp.scale,
                                inst->op[0]->data.regscaledisp.offset),
                                inst->op[1]->data.imm);
                            }
                            break;
                            
                            default:
                            ERR;
                            break;
                          }
                        }
                        else ERR;
                      }
                      break;
                      
                      default:
                      ERR;
                      break;
                    }
                  }
                  break;
                
                  case gowidth_DWORD:
                  {
                    switch (inst->op[0]->type)
                    {
                      case gotype_INDREG:
                      {
                        genx86_tab[opcode].rm32_i32(nat, 
                          rtasm_ind(inst->op[0]->data.reg), 
                          inst->op[1]->data.imm);
                      }
                      break;

                      case gotype_INDREGPLUSDISP8:
                      {
                        genx86_tab[opcode].rm32_i32(nat, 
                          rtasm_ind8(inst->op[0]->data.regdisp.base, 
                          inst->op[0]->data.regdisp.disp),
                          inst->op[1]->data.imm);
                      }
                      break;

                      case gotype_INDREGPLUSDISP32:
                      {
                        genx86_tab[opcode].rm32_i32(nat, 
                          rtasm_ind32(inst->op[0]->data.regdisp.base, 
                          inst->op[0]->data.regdisp.disp),
                          inst->op[1]->data.imm);
                      }
                      break;

                      case gotype_INDREGPLUSSCALEDREG:
                      {
                        genx86_tab[opcode].rm32_i32(nat, 
                          rtasm_scind(inst->op[0]->data.regscale.base,
                          inst->op[0]->data.regscale.index,
                          inst->op[0]->data.regscale.scale),
                          inst->op[1]->data.imm);
                      }
                      break;

                      case gotype_INDREGPLUSSCALEDREGPLUSDISP8:
                      {
                        genx86_tab[opcode].rm32_i32(nat, rtasm_scind8(
                          inst->op[0]->data.regscaledisp.base,
                          inst->op[0]->data.regscaledisp.index,
                          inst->op[0]->data.regscaledisp.scale,
                          inst->op[0]->data.regscaledisp.offset),
                          inst->op[1]->data.imm);
                      }
                      break;

                      case gotype_INDREGPLUSSCALEDREGPLUSDISP32:
                      {
                        genx86_tab[opcode].rm32_i32(nat, rtasm_scind32(
                          inst->op[0]->data.regscaledisp.base,
                          inst->op[0]->data.regscaledisp.index,
                          inst->op[0]->data.regscaledisp.scale,
                          inst->op[0]->data.regscaledisp.offset),
                          inst->op[1]->data.imm);
                      }
                      break;
                      
                      default:
                      ERR;
                      break;
                    }
                  }
                  break;
                  
                  default:
                  ERR;
                  break;
                }
              }
              break;
              
              case gotype_REGISTER:
              {
                if (genx86_tab[opcode].rm32_r32)
                {
                  switch (inst->op[0]->type)
                  {
                    case gotype_INDREG:
                    {
                      genx86_tab[opcode].rm32_r32(nat, 
                        rtasm_ind(inst->op[0]->data.reg), 
                        inst->op[1]->data.reg);
                    }
                    break;

                    case gotype_INDREGPLUSDISP8:
                    {
                      genx86_tab[opcode].rm32_r32(nat, 
                        rtasm_ind8(inst->op[0]->data.regdisp.base, 
                        inst->op[0]->data.regdisp.disp),
                        inst->op[1]->data.reg);
                    }
                    break;

                    case gotype_INDREGPLUSDISP32:
                    {
                      genx86_tab[opcode].rm32_r32(nat, 
                        rtasm_ind32(inst->op[0]->data.regdisp.base, 
                        inst->op[0]->data.regdisp.disp),
                        inst->op[1]->data.reg);
                    }
                    break;

                    case gotype_INDREGPLUSSCALEDREG:
                    {
                      genx86_tab[opcode].rm32_r32(nat, 
                        rtasm_scind(inst->op[0]->data.regscale.base,
                        inst->op[0]->data.regscale.index,
                        inst->op[0]->data.regscale.scale),
                        inst->op[1]->data.reg);
                    }
                    break;

                    case gotype_INDREGPLUSSCALEDREGPLUSDISP8:
                    {
                      genx86_tab[opcode].rm32_r32(nat, rtasm_scind8(
                        inst->op[0]->data.regscaledisp.base,
                        inst->op[0]->data.regscaledisp.index,
                        inst->op[0]->data.regscaledisp.scale,
                        inst->op[0]->data.regscaledisp.offset),
                        inst->op[1]->data.reg);
                    }
                    break;

                    case gotype_INDREGPLUSSCALEDREGPLUSDISP32:
                    {
                      genx86_tab[opcode].rm32_r32(nat, rtasm_scind32(
                        inst->op[0]->data.regscaledisp.base,
                        inst->op[0]->data.regscaledisp.index,
                        inst->op[0]->data.regscaledisp.scale,
                        inst->op[0]->data.regscaledisp.offset),
                        inst->op[1]->data.reg);
                    }
                    break;
                    
                    default:
                    ERR;
                    break;
                  }
                }
                else if (inst->op[1]->data.reg==ECX &&
                         genx86_tab[opcode].rm32_c)
                {
                  switch (inst->op[0]->type)
                  {
                    case gotype_INDREG:
                    {
                      genx86_tab[opcode].rm32_c(nat, 
                        rtasm_ind(inst->op[0]->data.reg));
                    }
                    break;

                    case gotype_INDREGPLUSDISP8:
                    {
                      genx86_tab[opcode].rm32_c(nat, 
                        rtasm_ind8(inst->op[0]->data.regdisp.base, 
                        inst->op[0]->data.regdisp.disp));
                    }
                    break;

                    case gotype_INDREGPLUSDISP32:
                    {
                      genx86_tab[opcode].rm32_c(nat, 
                        rtasm_ind32(inst->op[0]->data.regdisp.base, 
                        inst->op[0]->data.regdisp.disp));
                    }
                    break;

                    case gotype_INDREGPLUSSCALEDREG:
                    {
                      genx86_tab[opcode].rm32_c(nat, 
                        rtasm_scind(inst->op[0]->data.regscale.base,
                        inst->op[0]->data.regscale.index,
                        inst->op[0]->data.regscale.scale));
                    }
                    break;

                    case gotype_INDREGPLUSSCALEDREGPLUSDISP8:
                    {
                      genx86_tab[opcode].rm32_c(nat, rtasm_scind8(
                        inst->op[0]->data.regscaledisp.base,
                        inst->op[0]->data.regscaledisp.index,
                        inst->op[0]->data.regscaledisp.scale,
                        inst->op[0]->data.regscaledisp.offset));
                    }
                    break;

                    case gotype_INDREGPLUSSCALEDREGPLUSDISP32:
                    {
                      genx86_tab[opcode].rm32_c(nat, rtasm_scind32(
                        inst->op[0]->data.regscaledisp.base,
                        inst->op[0]->data.regscaledisp.index,
                        inst->op[0]->data.regscaledisp.scale,
                        inst->op[0]->data.regscaledisp.offset));
                    }
                    break;
                    
                    default:
                    ERR;
                    break;
                  }
                }
              }
              break;
              
              default:
              ERR;
              break;
            }  // switch (op1 type)
          }
          break;
          
          default:
          ERR;
          break;
        }
      }
      break;
      
      default:
      ERR;
      break;
    }
  }
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
      genx86_translatealloc(chunk, src->slot, src);
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

#define COMPOUND(D,S1,S2) (((D)*pal_NUMTYPES*pal_NUMTYPES) + \
                           ((S1)*pal_NUMTYPES) + (S2))

#define ERR { fprintf(stderr, "Specialisation error at %d. " \
  "Opcode=%s, pattern=%s:%s:%s.\n", \
  __LINE__, abname[opcode], allocname[dest->type], allocname[src1->type], \
  allocname[src2->type]); abort(); }

/*
void genx86_out(nativeblockinfo* nat, uint5 opcode, palloc_info* dest,
                palloc_info* src1, palloc_info* src2, list* map)
{
//  dest = genx86_closesplit(dest, line);
//  src1 = genx86_closesplit(src1, line);
//  src2 = genx86_closesplit(src2, line);
  
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
*/

#undef COMPOUND
#undef ERR

// Won't move something into itself, simplifies zero-loads
void genx86_move(pheta_chunk* chunk, genx86_buffer* buf, palloc_info* dest, 
  palloc_info* src)
{
/*  if (dest->type==src->type)
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
      genx86_operand* opdest = genx86_findoperand(chunk, dest);
      genx86_append(chunk, buf, ab_XOR, opdest, opdest, 0);
      return;
    }
  }*/
  genx86_append(chunk, buf, ab_MOV, genx86_findoperand(chunk, dest),
    genx86_findoperand(chunk, src), 0);
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
      list_add(&buf->commit);
      buf->commit->data = dfc = cnew(genx86_delayedfetchcommit);
      dfc->var = chunk->alloc[range->reg].arm_affiliation;
      dfc->src = range->reg;
      dfc->loc = buf->buffer->prev;
    }
  }
}

uint5 genx86_translate_opcode(genx86_buffer* buf, pheta_chunk* chunk,
  pheta_instr* instr)
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
      list_add(&buf->fetch);
      buf->fetch->data = dfc = cnew(genx86_delayedfetchcommit);
      dfc->var = instr->data.op.dest;
      dfc->src = instr->data.op.src1;
      dfc->loc = buf->buffer->prev;

      chunk->alloc[instr->data.op.dest].arm_affiliation = instr->data.op.src1;
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
      dfc->var = instr->data.op.src1;
      dfc->loc = buf->buffer->prev;
      fprintf(stderr, "Storing commit %d to %d\n", instr->data.op.src1,
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
      genx86_append(chunk, buf, ab_SBB, genx86_findoperand(chunk, dest),
        genx86_findoperand(chunk, src2), 0);
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
        genx86_append(chunk, buf, ab_IMUL, genx86_findoperand(chunk, dest),
          genx86_findoperand(chunk, src1), genx86_findoperand(chunk, src2));
      }
      else
      {
        genx86_move(chunk, buf, dest, src1);
        genx86_append(chunk, buf, ab_IMUL, genx86_findoperand(chunk, dest),
          genx86_findoperand(chunk, src2), 0);
      }
    }
    break;

    case ph_RRX:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src = &chunk->alloc[instr->data.op.src1];
      genx86_operand* one = cnew(genx86_operand);
      one->type = gotype_IMMEDIATE;
      one->width = gowidth_BYTE;
      one->data.imm = 1;
      genx86_move(chunk, buf, dest, src);
      genx86_append(chunk, buf, ab_RCR, genx86_findoperand(chunk, dest), one, 
        0);
    }
    break;

    case ph_RLX:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src = &chunk->alloc[instr->data.op.src1];
      genx86_operand* one = cnew(genx86_operand);
      one->type = gotype_IMMEDIATE;
      one->width = gowidth_BYTE;
      one->data.imm = 1;
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
    }
    break;

    case ph_NOT:
    {
      palloc_info* dest = &chunk->alloc[instr->data.op.dest];
      palloc_info* src = &chunk->alloc[instr->data.op.src1];
      genx86_move(chunk, buf, dest, src);
      genx86_append(chunk, buf, ab_NOT, genx86_findoperand(chunk, dest), 0, 0);
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
        off = cnew(genx86_operand);
        off->type = gotype_INDREGPLUSDISP8;
        off->width = gowidth_BYTE;
        off->data.regdisp.base = EBP;
        off->data.regdisp.disp = offsetof(registerinfo, cflag);
        if (buf->beenset & ph_IC)
          genx86_append(chunk, buf, ab_SETAE, off, 0, 0);
        else if (buf->beenset & ph_C)
          genx86_append(chunk, buf, ab_SETB, off, 0, 0);
        else
          assert(!"Carry flag has not been set");
      }
      if (needmask & ph_V)
      {
        off = cnew(genx86_operand);
        off->type = gotype_INDREGPLUSDISP8;
        off->width = gowidth_BYTE;
        off->data.regdisp.base = EBP;
        off->data.regdisp.disp = offsetof(registerinfo, vflag);
        genx86_append(chunk, buf, ab_SETO, off, 0, 0);
      }
      if (needmask & ph_N)
      {
        off = cnew(genx86_operand);
        off->type = gotype_INDREGPLUSDISP8;
        off->width = gowidth_BYTE;
        off->data.regdisp.base = EBP;
        off->data.regdisp.disp = offsetof(registerinfo, nflag);
        genx86_append(chunk, buf, ab_SETS, off, 0, 0);
      }
      if (needmask & ph_Z)
      {
        off = cnew(genx86_operand);
        off->type = gotype_INDREGPLUSDISP8;
        off->width = gowidth_BYTE;
        off->data.regdisp.base = EBP;
        off->data.regdisp.disp = offsetof(registerinfo, zflag);
        genx86_append(chunk, buf, ab_SETE, off, 0, 0);
      }
      if (pred)
      {
        uint5 j;
        for (j=0; j<14; j++)
        {
          if (pred & (1<<j))
          {
            off = cnew(genx86_operand);
            off->type = gotype_INDREGPLUSDISP8;
            off->width = gowidth_BYTE;
            off->data.regdisp.base = EBP;
            off->data.regdisp.disp = offsetof(registerinfo, predbuf[j]);
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
            genx86_operand* off = cnew(genx86_operand);
            off->type = gotype_INDREGPLUSDISP8;
            off->width = gowidth_BYTE;
            off->data.regdisp.base = EBP;
            off->data.regdisp.disp = offsetof(registerinfo, npredbuf[j]);
            genx86_append(chunk, buf, predset[j], off, 0, 0);
          }
        }
      }
    }
    break;

    case ph_FENSURE:
    {
      uint5 mask = instr->data.flag.need;
      genx86_operand* zero, *off;
      if (mask)
      {
        if (mask==ph_C || mask==ph_IC)
        {
          off = cnew(genx86_operand);
          off->type = gotype_INDREGPLUSDISP8;
          off->width = gowidth_BYTE;
          off->data.regdisp.base = EBP;
          off->data.regdisp.disp = offsetof(registerinfo, cflag);
          zero = cnew(genx86_operand);
          zero->type = gotype_IMMEDIATE;
          zero->width = gowidth_BYTE;
          zero->data.imm = 0;
          genx86_append(chunk, buf, ab_BT, off, zero, 0);
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
      genx86_operand* rel;
      rel = cnew(genx86_operand);
      rel->type = gotype_IMMEDIATE;
      rel->width = gowidth_DWORD;
      rel->data.imm = 0;
      /*  memory structure base
       *  address
       */
      genx86_append(chunk, buf, ab_CALL, rel, 0, 0);
      rel = cnew(genx86_operand);
      rel->type = gotype_IMMEDIATE;
      rel->width = gowidth_BYTE;
      rel->data.imm = 0;
      genx86_append(chunk, buf, ab_JECXZ, rel, 0, 0);
      genx86_recover(buf, chunk);
      rel = cnew(genx86_operand);
      rel->type = gotype_IMMEDIATE;
      rel->width = gowidth_DWORD;
      rel->data.imm = 0;
      genx86_append(chunk, buf, ab_CALL, rel, 0, 0);
      genx86_append(chunk, buf, ab_RET, 0, 0, 0);
    }
    break;

    case ph_LDB:
    {
      genx86_operand* rel;
      rel = cnew(genx86_operand);
      rel->type = gotype_IMMEDIATE;
      rel->width = gowidth_DWORD;
      rel->data.imm = 0;
      /*  memory structure base
       *  address
       */
      genx86_append(chunk, buf, ab_CALL, rel, 0, 0);
      rel = cnew(genx86_operand);
      rel->type = gotype_IMMEDIATE;
      rel->width = gowidth_BYTE;
      rel->data.imm = 0;
      genx86_append(chunk, buf, ab_JECXZ, rel, 0, 0);
      genx86_recover(buf, chunk);
      rel = cnew(genx86_operand);
      rel->type = gotype_IMMEDIATE;
      rel->width = gowidth_DWORD;
      rel->data.imm = 0;
      genx86_append(chunk, buf, ab_CALL, rel, 0, 0);
      genx86_append(chunk, buf, ab_RET, 0, 0, 0);
    }
    break;

    case ph_STW:
    case ph_STB:
    {
      genx86_operand* rel;
      rel = cnew(genx86_operand);
      rel->type = gotype_IMMEDIATE;
      rel->width = gowidth_DWORD;
      rel->data.imm = 0;
      /*  memory structure base
       *  address
       */
      genx86_append(chunk, buf, ab_CALL, rel, 0, 0);
      rel = cnew(genx86_operand);
      rel->type = gotype_IMMEDIATE;
      rel->width = gowidth_BYTE;
      rel->data.imm = 0;
      genx86_append(chunk, buf, ab_JECXZ, rel, 0, 0);
      genx86_recover(buf, chunk);
      rel = cnew(genx86_operand);
      rel->type = gotype_IMMEDIATE;
      rel->width = gowidth_DWORD;
      rel->data.imm = 0;
      genx86_append(chunk, buf, ab_CALL, rel, 0, 0);
      genx86_append(chunk, buf, ab_RET, 0, 0, 0);
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
      genx86_operand* dest, *c8;
      dest = cnew(genx86_operand);
      dest->type = gotype_INDREGPLUSDISP8;
      dest->width = gowidth_DWORD;
      dest->data.regdisp.base = EBP;
      dest->data.regdisp.disp = 15*4;
      c8 = cnew(genx86_operand);
      c8->type = gotype_IMMEDIATE;
      c8->width = gowidth_BYTE;
      c8->data.imm = 8;
      genx86_append(chunk, buf, ab_ADD, dest, c8, 0);
      genx86_append(chunk, buf, ab_RET, 0, 0, 0);
    }
    break;
    // !!! things missing
  }
  
  return 0;
}

void genx86_insert_spill_code_inner(genx86_buffer* buf, pheta_chunk* chunk)
{
  list* scan;
  
  for (scan=buf->fetch; scan; scan=scan->prev)
  {
    genx86_delayedfetchcommit* dfc = scan->data;
    if (chunk->alloc[dfc->var].type == pal_IREG)
    {
      genx86_operand* reg = cnew(genx86_operand);
      genx86_operand* mem = cnew(genx86_operand);
      reg->type = gotype_REGISTER;
      reg->width = gowidth_DWORD;
      reg->data.reg = chunk->alloc[dfc->var].info.ireg.num;
      mem->type = dfc->src*4 < 128 ? gotype_INDREGPLUSDISP8 : 
                                     gotype_INDREGPLUSDISP32;
      mem->width = gowidth_DWORD;
      mem->data.regdisp.base = EBP;
      mem->data.regdisp.disp = dfc->src*4;
      fprintf(stderr, "Fetch %d to %d\n", dfc->src, 
        chunk->alloc[dfc->var].info.ireg.num);
      genx86_insert(chunk, buf, dfc->loc->next, ab_MOV, reg, mem, 0);
    }
  }
  
  for (scan=buf->commit; scan; scan=scan->prev)
  {
    genx86_delayedfetchcommit* dfc = scan->data;
    if (chunk->alloc[dfc->var].type == pal_IREG)
    {
      genx86_operand* reg = cnew(genx86_operand);
      genx86_operand* mem = cnew(genx86_operand);
      reg->type = gotype_REGISTER;
      reg->width = gowidth_DWORD;
      reg->data.reg = chunk->alloc[dfc->var].info.ireg.num;
      mem->type = dfc->src < 128 ? gotype_INDREGPLUSDISP8
                                 : gotype_INDREGPLUSDISP32;
      mem->width = gowidth_DWORD;
      mem->data.regdisp.base = EBP;
      mem->data.regdisp.disp = dfc->src*4;
      fprintf(stderr, "Commit %d to %d\n", chunk->alloc[dfc->var].info.ireg.num,
        dfc->src);
      genx86_insert(chunk, buf, dfc->loc->next, ab_MOV, mem, reg, 0);
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

void genx86_flatten_code_inner(pheta_basicblock* blk)
{
  clist* scancode;
  nativeblockinfo* nat = rtasm_new();
  
  for (scancode=blk->gxbuffer->buffer->next; scancode->data; 
       scancode=scancode->next)
  {
    genx86_op* op = scancode->data;
    genx86_asm(nat, op);
  }
  x86dism_block(nat);
}

void genx86_flatten_code(pheta_chunk* chunk)
{
  list* scanblock;
  
  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
  {
    genx86_flatten_code_inner((pheta_basicblock*)scanblock->data);
  }
}
