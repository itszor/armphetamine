#include "genx86.h"
#include "x86asm.h"
#include "x86dism.h"
#include "rtasm.h"
#include "pheta.h"
#include "palloc.h"

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

void genx86_regmove(nativeblockinfo* nat, uint3 dest, uint3 src)
{
  if (src != dest)
  {
    rtasm_mov_r32_rm32(nat, dest, rtasm_reg(src));
  }
}

nativeblockinfo* genx86_translate(pheta_chunk* chunk, pheta_basicblock* blk)
{
  uint5 i;
  nativeblockinfo* nat = x86asm_newnative();
  nativeblockinfo* truebranch = 0, *falsebranch = 0;
  
  for (i=0; i<blk->length; i++)
  {
    uint5 opcode = blk->base[i];
    
    switch (opcode)
    {
      case ph_FETCH:
      {
        uint5 dest = blk->base[i+1];
        uint5 armsrc = blk->base[i+2];
        switch (chunk->alloc[dest].type)
        {
          case pal_IREG:
          {
            rtasm_mov_r32_rm32(nat, chunk->alloc[dest].info.ireg.num,
              rtasm_ind8(EBP, armsrc*4));
          }
          break;
          
          case pal_RFILE:
          {
            chunk->alloc[dest].info.value = armsrc;
          }
          break;
        }
      }
      break;
      
      case ph_COMMIT:
      {
        uint5 armdest = blk->base[i+1];
        uint5 src = blk->base[i+2];
        if (chunk->alloc[src].type == pal_IREG)
        {
          rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, armdest*4),
            chunk->alloc[src].info.ireg.num);
        }
      }
      break;
      
      case ph_LSL:
      case ph_LSR:
      case ph_ASR:
      case ph_ROR:
      case ph_ROL:
      case ph_AND:
      case ph_OR:
      case ph_EOR:
      case ph_ADD:
      case ph_ADC:
      case ph_SUB:
      case ph_SBC:
      case ph_MUL:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src1 = &chunk->alloc[blk->base[i+2]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+3]];
        switch (src2->type)
        {
          case pal_CONST:
          case pal_CONSTB:  // src2 is constant
          {
            switch (src1->type)
            {
              case pal_IREG:  // src1 is ireg
              {
                switch (dest->type)
                {
                  case pal_IREG:  // destination is ireg
                  {
                    // register <- register (op) constant
                    switch (opcode)  // again... :-/
                    {
                      case ph_LSL:
                      {
                        genx86_regmove(nat, dest->info.ireg.num,
                          src1->info.ireg.num);
                        rtasm_shl_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num),
                          src2->info.value);
                      }
                      break;
                      
                      case ph_LSR:
                      {
                        genx86_regmove(nat, dest->info.ireg.num,
                          src1->info.ireg.num);
                        rtasm_shr_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num),
                          src2->info.value);
                      }
                      break;
                      
                      case ph_ASR:
                      {
                        genx86_regmove(nat, dest->info.ireg.num,
                          src1->info.ireg.num);
                        rtasm_sar_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num),
                          src2->info.value);
                      }
                      break;
                      
                      case ph_ROR:
                      {
                        genx86_regmove(nat, dest->info.ireg.num,
                          src1->info.ireg.num);
                        rtasm_ror_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num),
                          src2->info.value);
                      }
                      break;
                      
                      case ph_ROL:
                      {
                        genx86_regmove(nat, dest->info.ireg.num,
                          src1->info.ireg.num);
                        rtasm_rol_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num),
                          src2->info.value);
                      }
                      break;
                      
                      case ph_AND:
                      {
                        if (src2->info.value>=256)
                        {
                          genx86_regmove(nat, dest->info.ireg.num,
                            src1->info.ireg.num);
                          rtasm_and_rm32_imm32(nat, 
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                        else
                        {
                          genx86_regmove(nat, dest->info.ireg.num,
                            src1->info.ireg.num);
                          rtasm_and_rm32_imm8(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                      }
                      break;

                      case ph_OR:
                      {
                        if (src2->info.value>=256)
                        {
                          genx86_regmove(nat, dest->info.ireg.num,
                            src1->info.ireg.num);
                          rtasm_or_rm32_imm32(nat, 
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                        else
                        {
                          genx86_regmove(nat, dest->info.ireg.num,
                            src1->info.ireg.num);
                          rtasm_or_rm32_imm8(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                      }
                      break;

                      case ph_EOR:
                      {
                        if (src2->info.value>=256)
                        {
                          genx86_regmove(nat, dest->info.ireg.num,
                            src1->info.ireg.num);
                          rtasm_xor_rm32_imm32(nat, 
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                        else
                        {
                          genx86_regmove(nat, dest->info.ireg.num,
                            src1->info.ireg.num);
                          rtasm_xor_rm32_imm8(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                      }
                      break;

                      case ph_ADD:
                      {
                        if (src2->info.value>=256)
                        {
                          genx86_regmove(nat, dest->info.ireg.num,
                            src1->info.ireg.num);
                          rtasm_add_rm32_imm32(nat, 
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                        else
                        {
                          genx86_regmove(nat, dest->info.ireg.num,
                            src1->info.ireg.num);
                          rtasm_add_rm32_imm8(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                      }
                      break;

                      case ph_ADC:
                      {
                        if (src2->info.value>=256)
                        {
                          genx86_regmove(nat, dest->info.ireg.num,
                            src1->info.ireg.num);
                          rtasm_adc_rm32_imm32(nat, 
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                        else
                        {
                          genx86_regmove(nat, dest->info.ireg.num,
                            src1->info.ireg.num);
                          rtasm_adc_rm32_imm8(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                      }
                      break;
                      
                      case ph_SUB:
                      {
                        if (src2->info.value>=256)
                        {
                          genx86_regmove(nat, dest->info.ireg.num,
                            src1->info.ireg.num);
                          rtasm_sub_rm32_imm32(nat, 
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                        else
                        {
                          genx86_regmove(nat, dest->info.ireg.num,
                            src1->info.ireg.num);
                          rtasm_sub_rm32_imm8(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                      }
                      break;

                      case ph_SBC:
                      {
                        if (src2->info.value>=256)
                        {
                          genx86_regmove(nat, dest->info.ireg.num,
                            src1->info.ireg.num);
                          rtasm_sbb_rm32_imm32(nat, 
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                        else
                        {
                          genx86_regmove(nat, dest->info.ireg.num,
                            src1->info.ireg.num);
                          rtasm_sbb_rm32_imm8(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                       }
                      break;
                      
                      case ph_MUL:
                      {
                        if (src2->info.value>=256)
                        {
                          rtasm_imul_r32_rm32_imm32(nat, dest->info.ireg.num,
                            rtasm_reg(src1->info.ireg.num),
                            src2->info.value);
                        }
                        else
                        {
                          rtasm_imul_r32_rm32_imm8(nat, dest->info.ireg.num,
                            rtasm_reg(src1->info.ireg.num),
                            src2->info.value);
                        }
                      }
                      break;
                    }
                  }
                  break;
                  
                  // mem <- register (op) constant
                  case pal_RFILE:  
                  {
                    switch (opcode)
                    {
                      case ph_LSL:
                      {
                        rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, 
                          dest->info.value), src1->info.ireg.num);
                        rtasm_shl_rm32_imm8(nat, rtasm_ind8(EBP, 
                          dest->info.value), src2->info.value);
                      }
                      break;

                      case ph_LSR:
                      {
                        rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, 
                          dest->info.value), src1->info.ireg.num);
                        rtasm_shr_rm32_imm8(nat, rtasm_ind8(EBP, 
                          dest->info.value), src2->info.value);
                      }
                      break;

                      case ph_ASR:
                      {
                        rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, 
                          dest->info.value), src1->info.ireg.num);
                        rtasm_sar_rm32_imm8(nat, rtasm_ind8(EBP, 
                          dest->info.value), src2->info.value);
                      }
                      break;

                      case ph_ROR:
                      {
                        rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, 
                          dest->info.value), src1->info.ireg.num);
                        rtasm_ror_rm32_imm8(nat, rtasm_ind8(EBP, 
                          dest->info.value), src2->info.value);
                      }
                      break;

                      case ph_ROL:
                      {
                        rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, 
                          dest->info.value), src1->info.ireg.num);
                        rtasm_rol_rm32_imm8(nat, rtasm_ind8(EBP, 
                          dest->info.value), src2->info.value);
                      }
                      break;

                      case ph_AND:
                      {
                        if (src2->info.value>=256)
                        {
                          rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, 
                            dest->info.value), src1->info.ireg.num);
                          rtasm_and_rm32_imm32(nat, rtasm_ind8(EBP,
                            dest->info.value), src2->info.value);
                        }
                        else
                        {
                          rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, 
                            dest->info.value), src1->info.ireg.num);
                          rtasm_and_rm32_imm8(nat, rtasm_ind8(EBP,
                            dest->info.value), src2->info.value);
                        }
                      }
                      break;

                      case ph_OR:
                      {
                        if (src2->info.value>=256)
                        {
                          rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, 
                            dest->info.value), src1->info.ireg.num);
                          rtasm_or_rm32_imm32(nat, rtasm_ind8(EBP,
                            dest->info.value), src2->info.value);
                        }
                        else
                        {
                          rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, 
                            dest->info.value), src1->info.ireg.num);
                          rtasm_or_rm32_imm8(nat, rtasm_ind8(EBP,
                            dest->info.value), src2->info.value);
                        }
                      }
                      break;

                      case ph_EOR:
                      {
                        if (src2->info.value>=256)
                        {
                          rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, 
                            dest->info.value), src1->info.ireg.num);
                          rtasm_xor_rm32_imm32(nat, rtasm_ind8(EBP,
                            dest->info.value), src2->info.value);
                        }
                        else
                        {
                          rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, 
                            dest->info.value), src1->info.ireg.num);
                          rtasm_xor_rm32_imm8(nat, rtasm_ind8(EBP,
                            dest->info.value), src2->info.value);
                        }
                      }
                      break;

                      case ph_ADD:
                      {
                        if (src2->info.value>=256)
                        {
                          rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, 
                            dest->info.value), src1->info.ireg.num);
                          rtasm_add_rm32_imm32(nat, rtasm_ind8(EBP,
                            dest->info.value), src2->info.value);
                        }
                        else
                        {
                          rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, 
                            dest->info.value), src1->info.ireg.num);
                          rtasm_add_rm32_imm8(nat, rtasm_ind8(EBP,
                            dest->info.value), src2->info.value);
                        }
                      }
                      break;

                      case ph_ADC:
                      {
                        if (src2->info.value>=256)
                        {
                          rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, 
                            dest->info.value), src1->info.ireg.num);
                          rtasm_adc_rm32_imm32(nat, rtasm_ind8(EBP,
                            dest->info.value), src2->info.value);
                        }
                        else
                        {
                          rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, 
                            dest->info.value), src1->info.ireg.num);
                          rtasm_adc_rm32_imm8(nat, rtasm_ind8(EBP,
                            dest->info.value), src2->info.value);
                        }
                      }
                      break;

                      case ph_SUB:
                      {
                        if (src2->info.value>=256)
                        {
                          rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, 
                            dest->info.value), src1->info.ireg.num);
                          rtasm_sub_rm32_imm32(nat, rtasm_ind8(EBP,
                            dest->info.value), src2->info.value);
                        }
                        else
                        {
                          rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, 
                            dest->info.value), src1->info.ireg.num);
                          rtasm_sub_rm32_imm8(nat, rtasm_ind8(EBP,
                            dest->info.value), src2->info.value);
                        }
                      }
                      break;

                      case ph_SBC:
                      {
                        if (src2->info.value>=256)
                        {
                          rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, 
                            dest->info.value), src1->info.ireg.num);
                          rtasm_sbb_rm32_imm32(nat, rtasm_ind8(EBP,
                            dest->info.value), src2->info.value);
                        }
                        else
                        {
                          rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, 
                            dest->info.value), src1->info.ireg.num);
                          rtasm_sbb_rm32_imm8(nat, rtasm_ind8(EBP,
                            dest->info.value), src2->info.value);
                        }
                      }
                      break;

                      case ph_MUL:
                      {
                        rtasm_xchg_r32_rm32(nat, src1->info.ireg.num, 
                          rtasm_ind8(EBP,
                          dest->info.value));
                        if (src2->info.value>=256)
                        {
                          rtasm_imul_r32_rm32_imm32(nat, src1->info.ireg.num,
                            rtasm_ind8(EBP, dest->info.value),
                            src2->info.value);
                        }
                        else
                        {
                          rtasm_imul_r32_rm32_imm8(nat, src1->info.ireg.num,
                            rtasm_ind8(EBP, dest->info.value),
                            src2->info.value);
                        }
                        rtasm_xchg_r32_rm32(nat, src1->info.ireg.num, 
                          rtasm_ind8(EBP,
                          dest->info.value));
                      }
                      break;
                    }  // switch (opcode)
                  }  // case src1==pal_RFILE
                  break;
                  
                  default:
                  {
                    fprintf(stderr, "Unknown src1 type\n");
                    exit(1);
                  }
                }  // switch (dest->type)
              }  // case src1==pal_IREG
              break;
              
              case pal_RFILE:  // src1 rfile, src2 constant
              {
                switch (dest->type)
                {
                  // register <- memory (op) constant
                  case pal_IREG:
                  {
                    switch (opcode)
                    {
                      case ph_LSL:
                      {
                        rtasm_mov_r32_rm32(nat, dest->info.ireg.num,
                          rtasm_ind8(EBP, src1->info.value));
                        rtasm_shl_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num),
                          src2->info.value);
                      }
                      break;

                      case ph_LSR:
                      {
                        rtasm_mov_r32_rm32(nat, dest->info.ireg.num,
                          rtasm_ind8(EBP, src1->info.value));
                        rtasm_shr_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num),
                          src2->info.value);
                      }
                      break;

                      case ph_ASR:
                      {
                        rtasm_mov_r32_rm32(nat, dest->info.ireg.num,
                          rtasm_ind8(EBP, src1->info.value));
                        rtasm_sar_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num),
                          src2->info.value);
                      }
                      break;

                      case ph_ROR:
                      {
                        rtasm_mov_r32_rm32(nat, dest->info.ireg.num,
                          rtasm_ind8(EBP, src1->info.value));
                        rtasm_ror_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num),
                          src2->info.value);
                      }
                      break;

                      case ph_ROL:
                      {
                        rtasm_mov_r32_rm32(nat, dest->info.ireg.num,
                          rtasm_ind8(EBP, src1->info.value));
                        rtasm_rol_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num),
                          src2->info.value);
                      }
                      break;

                      case ph_AND:
                      {
                        if (src2->info.value>=256)
                        {
                          rtasm_mov_r32_rm32(nat, dest->info.ireg.num,
                            rtasm_ind8(EBP, src1->info.value));
                          rtasm_and_rm32_imm32(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                        else
                        {
                          rtasm_mov_r32_rm32(nat, dest->info.ireg.num,
                            rtasm_ind8(EBP, src1->info.value));
                          rtasm_and_rm32_imm8(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                      }
                      break;

                      case ph_OR:
                      {
                        if (src2->info.value>=256)
                        {
                          rtasm_mov_r32_rm32(nat, dest->info.ireg.num,
                            rtasm_ind8(EBP, src1->info.value));
                          rtasm_or_rm32_imm32(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                        else
                        {
                          rtasm_mov_r32_rm32(nat, dest->info.ireg.num,
                            rtasm_ind8(EBP, src1->info.value));
                          rtasm_or_rm32_imm8(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                      }
                      break;

                      case ph_EOR:
                      {
                        if (src2->info.value>=256)
                        {
                          rtasm_mov_r32_rm32(nat, dest->info.ireg.num,
                            rtasm_ind8(EBP, src1->info.value));
                          rtasm_xor_rm32_imm32(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                        else
                        {
                          rtasm_mov_r32_rm32(nat, dest->info.ireg.num,
                            rtasm_ind8(EBP, src1->info.value));
                          rtasm_xor_rm32_imm8(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                      }
                      break;

                      case ph_ADD:
                      {
                        if (src2->info.value>=256)
                        {
                          rtasm_mov_r32_rm32(nat, dest->info.ireg.num,
                            rtasm_ind8(EBP, src1->info.value));
                          rtasm_add_rm32_imm32(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                        else
                        {
                          rtasm_mov_r32_rm32(nat, dest->info.ireg.num,
                            rtasm_ind8(EBP, src1->info.value));
                          rtasm_add_rm32_imm8(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                      }
                      break;

                      case ph_ADC:
                      {
                        if (src2->info.value>=256)
                        {
                          rtasm_mov_r32_rm32(nat, dest->info.ireg.num,
                            rtasm_ind8(EBP, src1->info.value));
                          rtasm_adc_rm32_imm32(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                        else
                        {
                          rtasm_mov_r32_rm32(nat, dest->info.ireg.num,
                            rtasm_ind8(EBP, src1->info.value));
                          rtasm_adc_rm32_imm8(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                      }
                      break;

                      case ph_SUB:
                      {
                        if (src2->info.value>=256)
                        {
                          rtasm_mov_r32_rm32(nat, dest->info.ireg.num,
                            rtasm_ind8(EBP, src1->info.value));
                          rtasm_sub_rm32_imm32(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                        else
                        {
                          rtasm_mov_r32_rm32(nat, dest->info.ireg.num,
                            rtasm_ind8(EBP, src1->info.value));
                          rtasm_sub_rm32_imm8(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                      }
                      break;

                      case ph_SBC:
                      {
                        if (src2->info.value>=256)
                        {
                          rtasm_mov_r32_rm32(nat, dest->info.ireg.num,
                            rtasm_ind8(EBP, src1->info.value));
                          rtasm_sbb_rm32_imm32(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                        else
                        {
                          rtasm_mov_r32_rm32(nat, dest->info.ireg.num,
                            rtasm_ind8(EBP, src1->info.value));
                          rtasm_sbb_rm32_imm8(nat,
                            rtasm_reg(dest->info.ireg.num),
                            src2->info.value);
                        }
                      }
                      break;

                      case ph_MUL:
                      {
                        if (src2->info.value>=256)
                        {
                          rtasm_imul_r32_rm32_imm32(nat, dest->info.ireg.num,
                            rtasm_ind8(EBP, src1->info.value),
                            src2->info.value);
                        }
                        else
                        {
                          rtasm_imul_r32_rm32_imm8(nat, dest->info.ireg.num,
                            rtasm_ind8(EBP, src1->info.value),
                            src2->info.value);
                        }
                      }
                      break;
                    }  // switch (opcode)
                  }  // case dest==pal_IREG
                  break;
                  
                  // memory <- memory (op) constant
                  // (this would need a spare register. Maybe I should put
                  // one at random onto the stack, but that's not very nice.)
                  case pal_RFILE:
                  {
                    fprintf(stderr, "I don't do (memory*memory)!\n");
                    exit(1);
                  }  // case dest==pal_RFILE
                  break;
                }
              }
              break;
            }
          }
          break;
          
          case pal_IREG:  // src2 is ireg
          {
            rtasm_nop(nat);
          }
          break;
          
          case pal_RFILE:  // src2 is in rfile
          {
            rtasm_nop(nat);
          }
          break;
          
          default:
          {
            rtasm_nop(nat);
          }
        }
      }
    }
    
    i += pheta_instlength[opcode];
  }  // for (...)

  phetadism_block(blk);
  x86dism_block(nat);

  blk->marker = 1;

  if (blk->trueblk && !blk->trueblk->marker)
    truebranch = genx86_translate(chunk, blk->trueblk);
  
  if (blk->falseblk && !blk->falseblk->marker)
    falsebranch = genx86_translate(chunk, blk->falseblk);

  /* join parent block to true/false blocks... */

  return nat;
}
