#include <stdio.h>

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
      
#define COMPOUND(D,S1,S2) (((D)<<6) | ((S1)<<3) | (S2))

      case ph_LSL:
      case ph_LSR:
      case ph_ASR:
      case ph_ROR:
      case ph_ROL:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src1 = &chunk->alloc[blk->base[i+2]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+3]];
        
        switch (COMPOUND(dest->type, src1->type, src2->type))
        {
          case COMPOUND(pal_IREG, pal_IREG, pal_CONST):
          {
            genx86_regmove(nat, dest->info.ireg.num, src1->info.ireg.num);
            switch (opcode)
            {
              case ph_LSL:
              rtasm_shl_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num),
                src2->info.value);
              break;
              
              case ph_LSR:
              rtasm_shl_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num),
                src2->info.value);
              break;

              case ph_ASR:
              rtasm_sar_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num),
                src2->info.value);
              break;

              case ph_ROR:
              rtasm_ror_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num),
                src2->info.value);
              break;

              case ph_ROL:
              rtasm_rol_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num),
                src2->info.value);
              break;
            }
          }
          break;
          
          case COMPOUND(pal_IREG, pal_IREG, pal_IREG):
          {
            genx86_regmove(nat, dest->info.ireg.num, src1->info.ireg.num);
            if (src2->info.ireg.num!=ECX)
            {
              if (dest->info.ireg.num!=ECX)
              {
                rtasm_push_r32(nat, ECX);
              }
              else
              {
                rtasm_xchg_r32_rm32(nat, ECX, rtasm_reg(src2->info.ireg.num));
              }
            }
            switch (opcode)
            {
              case ph_LSL:
              rtasm_shl_rm32_cl(nat, rtasm_reg(dest->info.ireg.num));
              break;

              case ph_LSR:
              rtasm_shr_rm32_cl(nat, rtasm_reg(dest->info.ireg.num));
              break;

              case ph_ASR:
              rtasm_sar_rm32_cl(nat, rtasm_reg(dest->info.ireg.num));
              break;

              case ph_ROR:
              rtasm_ror_rm32_cl(nat, rtasm_reg(dest->info.ireg.num));
              break;

              case ph_ROL:
              rtasm_rol_rm32_cl(nat, rtasm_reg(dest->info.ireg.num));
              break;
            }
            if (src2->info.ireg.num!=ECX)
            {
              if (dest->info.ireg.num!=ECX)
              {
                rtasm_pop_r32(nat, ECX);
              }
              else
              {
                rtasm_xchg_r32_rm32(nat, ECX, rtasm_reg(src2->info.ireg.num));
              }
            }
          }
          break;
          
          case COMPOUND(pal_IREG, pal_IREG, pal_RFILE):
          {
            rtasm_nop(nat);
          }
          break;
          
          case COMPOUND(pal_IREG, pal_RFILE, pal_CONST):
          {
            rtasm_mov_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
              src1->info.value*4));
            switch (opcode)
            {
              case ph_LSL:
              rtasm_shl_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num),
                src2->info.value);
              break;
              
              case ph_LSR:
              rtasm_shr_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num),
                src2->info.value);
              break;
              
              case ph_ASR:
              rtasm_sar_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num),
                src2->info.value);
              break;
              
              case ph_ROR:
              rtasm_ror_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num),
                src2->info.value);
              break;
              
              case ph_ROL:
              rtasm_rol_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num),
                src2->info.value);
              break;
            }
          }
          break;
          
          case COMPOUND(pal_IREG, pal_RFILE, pal_IREG):
          {
            rtasm_nop(nat);
          }
          break;
          
          case COMPOUND(pal_IREG, pal_RFILE, pal_RFILE):
          {
            rtasm_nop(nat);
          }
          break;
          
          case COMPOUND(pal_IREG, pal_CONST, pal_CONST):
          {
            uint5 value;
            switch (opcode)
            {
              case ph_LSL:
              value = src1->info.value << src2->info.value;
              break;
              
              case ph_LSR:
              value = src1->info.value >> src2->info.value;
              break;
              
              case ph_ASR:
              value = (sint5)src1->info.value >> src2->info.value;
              break;
              
              case ph_ROR:
              value = ROR(src1->info.value, src2->info.value);
              break;
              
              case ph_ROL:
              value = ROR(src1->info.value, 31-src2->info.value);
              break;
            }
            rtasm_mov_r32_imm32(nat, dest->info.ireg.num, value);
          }
          break;
          
          case COMPOUND(pal_IREG, pal_CONST, pal_IREG):
          {
            rtasm_nop(nat);
          }
          break;
          
          case COMPOUND(pal_IREG, pal_CONST, pal_RFILE):
          {
            rtasm_nop(nat);
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_IREG, pal_CONST):
          {
            rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
              src1->info.ireg.num);
            switch (opcode)
            {
              case ph_LSL:
              rtasm_shl_rm32_imm8(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.value);
              break;
              
              case ph_LSR:
              rtasm_shr_rm32_imm8(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.value);
              break;
              
              case ph_ASR:
              rtasm_sar_rm32_imm8(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.value);
              break;
              
              case ph_ROR:
              rtasm_ror_rm32_imm8(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.value);
              break;
              
              case ph_ROL:
              rtasm_rol_rm32_imm8(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.value);
              break;
            }
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_IREG, pal_IREG):
          {
            rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
              src1->info.ireg.num);
            if (src2->info.ireg.num!=ECX)
            {
              rtasm_push_r32(nat, ECX);
              rtasm_mov_r32_rm32(nat, ECX, rtasm_reg(src2->info.ireg.num));
            }
            switch (opcode)
            {
              case ph_LSL:
              rtasm_shl_rm32_cl(nat, rtasm_ind8(EBP, dest->info.value*4));
              break;
              
              case ph_LSR:
              rtasm_shr_rm32_cl(nat, rtasm_ind8(EBP, dest->info.value*4));
              break;

              case ph_ASR:
              rtasm_sar_rm32_cl(nat, rtasm_ind8(EBP, dest->info.value*4));
              break;

              case ph_ROR:
              rtasm_ror_rm32_cl(nat, rtasm_ind8(EBP, dest->info.value*4));
              break;

              case ph_ROL:
              rtasm_rol_rm32_cl(nat, rtasm_ind8(EBP, dest->info.value*4));
              break;
            }
            if (src2->info.ireg.num!=ECX)
            {
              rtasm_pop_r32(nat, ECX);
            }
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_IREG, pal_RFILE):
          {
            rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
              src1->info.ireg.num);
            rtasm_push_r32(nat, ECX);
            rtasm_mov_r32_rm32(nat, ECX, rtasm_ind8(EBP, src2->info.ireg.num));
            switch (opcode)
            {
              case ph_LSL:
              rtasm_shl_rm32_cl(nat, rtasm_ind8(EBP, dest->info.value*4));
              break;
              
              case ph_LSR:
              rtasm_shr_rm32_cl(nat, rtasm_ind8(EBP, dest->info.value*4));
              break;
              
              case ph_ASR:
              rtasm_sar_rm32_cl(nat, rtasm_ind8(EBP, dest->info.value*4));
              break;
              
              case ph_ROR:
              rtasm_ror_rm32_cl(nat, rtasm_ind8(EBP, dest->info.value*4));
              break;
              
              case ph_ROL:
              rtasm_rol_rm32_cl(nat, rtasm_ind8(EBP, dest->info.value*4));
              break;
            }
            rtasm_pop_r32(nat, ECX);
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_RFILE, pal_CONST):
          {
            rtasm_nop(nat);
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_RFILE, pal_IREG):
          {
            rtasm_nop(nat);
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_RFILE, pal_RFILE):
          {
            rtasm_nop(nat);
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_CONST, pal_CONST):
          {
            uint5 value;
            switch (opcode)
            {
              case ph_LSL:
              value = src1->info.value << src2->info.value;
              break;
              
              case ph_LSR:
              value = src1->info.value >> src2->info.value;
              break;
              
              case ph_ASR:
              value = (sint5)src1->info.value >> src2->info.value;
              break;
              
              case ph_ROR:
              value = ROR(src1->info.value, src2->info.value);
              break;
              
              case ph_ROL:
              value = ROR(src1->info.value, 31-src2->info.value);
              break;
            }
            rtasm_mov_rm32_imm32(nat, rtasm_ind8(EBP, dest->info.value*4),
              value);
          }
          break;

          case COMPOUND(pal_RFILE, pal_CONST, pal_IREG):
          {
            rtasm_nop(nat);
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_CONST, pal_RFILE):
          {
            rtasm_nop(nat);
          }
          break;
        }
      }
      break;  // three-arg shift operations
        
      case ph_AND:
      case ph_OR:
      case ph_EOR:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src1 = &chunk->alloc[blk->base[i+2]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+3]];
        
        switch (COMPOUND(dest->type, src1->type, src2->type))
        {
          case COMPOUND(pal_IREG, pal_IREG, pal_CONST):
          {
            uint5 value = src2->info.value;
            genx86_regmove(nat, dest->info.ireg.num, src1->info.ireg.num);
            switch (opcode)
            {
              case ph_AND:
              if (value<128)
                rtasm_and_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num), value);
              else
                rtasm_and_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num), 
                  value);
              break;
              
              case ph_OR:
              if (value<128)
                rtasm_or_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num), value);
              else
                rtasm_or_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num), 
                  value);
              break;
              
              case ph_EOR:
              if (value<128)
                rtasm_xor_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num), value);
              else
                rtasm_xor_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num), 
                  value);
              break;
            }
          }
          break;
          
          case COMPOUND(pal_IREG, pal_IREG, pal_IREG):
          {
            genx86_regmove(nat, dest->info.ireg.num, src1->info.ireg.num);
            switch (opcode)
            {
              case ph_AND:
              rtasm_and_r32_rm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num));
              break;
              
              case ph_OR:
              rtasm_or_r32_rm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num));
              break;
              
              case ph_EOR:
              rtasm_xor_r32_rm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num));
              break;
            }
          }
          break;
          
          case COMPOUND(pal_IREG, pal_IREG, pal_RFILE):
          {
            genx86_regmove(nat, dest->info.ireg.num, src1->info.ireg.num);
            switch (opcode)
            {
              case ph_AND:
              rtasm_and_r32_rm32(nat, dest->info.ireg.num,
                rtasm_ind8(EBP, src2->info.value*4));
              break;
              
              case ph_OR:
              rtasm_or_r32_rm32(nat, dest->info.ireg.num,
                rtasm_ind8(EBP, src2->info.value*4));
              break;
              
              case ph_EOR:
              rtasm_xor_r32_rm32(nat, dest->info.ireg.num,
                rtasm_ind8(EBP, src2->info.value*4));
              break;
            }
          }
          break;
          
          case COMPOUND(pal_IREG, pal_RFILE, pal_CONST):
          {
            uint5 value = src2->info.value;
            rtasm_mov_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
              src1->info.value));
            switch (opcode)
            {
              case ph_AND:
              if (value<128)
                rtasm_and_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num), value);
              else
                rtasm_and_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num), 
                  value);
              break;

              case ph_OR:
              if (value<128)
                rtasm_and_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num), value);
              else
                rtasm_and_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num), 
                  value);
              break;

              case ph_EOR:
              if (value<128)
                rtasm_and_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num), value);
              else
                rtasm_and_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num), 
                  value);
              break;
            }
          }
          break;
          
          case COMPOUND(pal_IREG, pal_RFILE, pal_IREG):
          {
            rtasm_mov_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
              src1->info.value*4));
            switch (opcode)
            {
              case ph_AND:
              rtasm_and_r32_rm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num));
              break;
              
              case ph_OR:
              rtasm_or_r32_rm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num));
              break;
              
              case ph_EOR:
              rtasm_xor_r32_rm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num));
              break;
            }
          }
          break;
          
          case COMPOUND(pal_IREG, pal_RFILE, pal_RFILE):
          {
            rtasm_mov_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
              src1->info.value*4));
            switch (opcode)
            {
              case ph_AND:
              rtasm_and_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
                src2->info.value*4));
              break;

              case ph_OR:
              rtasm_or_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
                src2->info.value*4));
              break;

              case ph_EOR:
              rtasm_xor_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
                src2->info.value*4));
              break;
            }
          }
          break;
          
          case COMPOUND(pal_IREG, pal_CONST, pal_CONST):
          {
            uint5 value;
            switch (opcode)
            {
              case ph_AND:
              value = src1->info.value & src2->info.value;
              break;
              
              case ph_OR:
              value = src1->info.value | src2->info.value;
              break;
              
              case ph_EOR:
              value = src1->info.value ^ src2->info.value;
              break;
            }
            rtasm_mov_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num), value);
          }
          break;
          
          case COMPOUND(pal_IREG, pal_CONST, pal_IREG):
          {
            rtasm_mov_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num),
              src1->info.value);
            switch (opcode)
            {
              case ph_AND:
              rtasm_and_r32_rm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num));
              break;
              
              case ph_OR:
              rtasm_or_r32_rm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num));
              break;
              
              case ph_EOR:
              rtasm_xor_r32_rm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num));
              break;
            }
          }
          break;
          
          case COMPOUND(pal_IREG, pal_CONST, pal_RFILE):
          {
            rtasm_mov_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num),
              src1->info.value);
            switch (opcode)
            {
              case ph_AND:
              rtasm_and_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
                src2->info.value*4));
              break;

              case ph_OR:
              rtasm_or_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
                src2->info.value*4));
              break;

              case ph_EOR:
              rtasm_xor_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
                src2->info.value*4));
              break;
            }
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_IREG, pal_CONST):
          {
            uint5 value = src2->info.value;
            rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
              src1->info.ireg.num);
            switch (opcode)
            {
              case ph_AND:
              if (value<128)
                rtasm_and_rm32_imm8(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              else
                rtasm_and_rm32_imm32(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              break;

              case ph_OR:
              if (value<128)
                rtasm_or_rm32_imm8(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              else
                rtasm_or_rm32_imm32(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              break;

              case ph_EOR:
              if (value<128)
                rtasm_xor_rm32_imm8(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              else
                rtasm_xor_rm32_imm32(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              break;
            }
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_IREG, pal_IREG):
          {
            rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
              src1->info.ireg.num);
            switch (opcode)
            {
              case ph_AND:
              rtasm_and_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;

              case ph_OR:
              rtasm_or_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;

              case ph_EOR:
              rtasm_xor_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;
            }
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_IREG, pal_RFILE):
          {
            rtasm_nop(nat);
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_RFILE, pal_CONST):
          {
            uint5 value = src2->info.value;
            if (dest->info.value != src1->info.value)
            {
              fprintf(stderr, "Bad allocation\n");
              exit(1);
            }
            switch (opcode)
            {
              case ph_AND:
              if (value<128)
                rtasm_and_rm32_imm8(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              else
                rtasm_and_rm32_imm32(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              break;

              case ph_OR:
              if (value<128)
                rtasm_or_rm32_imm8(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              else
                rtasm_or_rm32_imm32(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              break;

              case ph_EOR:
              if (value<128)
                rtasm_xor_rm32_imm8(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              else
                rtasm_xor_rm32_imm32(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              break;
            }
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_RFILE, pal_IREG):
          {
            if (dest->info.value != src1->info.value)
            {
              fprintf(stderr, "Bad allocation\n");
              exit(1);
            }
            switch (opcode)
            {
              case ph_AND:
              rtasm_and_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;
              
              case ph_OR:
              rtasm_or_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;
              
              case ph_EOR:
              rtasm_xor_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;
            }
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_RFILE, pal_RFILE):
          {
            rtasm_nop(nat);
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_CONST, pal_CONST):
          {
            uint5 value;
            switch (opcode)
            {
              case ph_AND:
              value = src1->info.value & src2->info.value;
              break;
              
              case ph_OR:
              value = src1->info.value | src2->info.value;
              break;
              
              case ph_EOR:
              value = src1->info.value ^ src2->info.value;
              break;
            }
            rtasm_mov_rm32_imm32(nat, rtasm_ind8(EBP, dest->info.value*4),
              value);
          }
          break;

          case COMPOUND(pal_RFILE, pal_CONST, pal_IREG):
          {
            rtasm_mov_rm32_imm32(nat, rtasm_ind8(EBP, dest->info.value*4),
              src1->info.value);
            switch (opcode)
            {
              case ph_AND:
              rtasm_and_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;

              case ph_OR:
              rtasm_or_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;

              case ph_EOR:
              rtasm_xor_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;
            }
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_CONST, pal_RFILE):
          {
            rtasm_nop(nat);
          }
          break;
        }
      }
      break;  // three-arg logic operations
      
      case ph_ADD:
      case ph_ADC:
      case ph_SUB:
      case ph_SBC:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src1 = &chunk->alloc[blk->base[i+2]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+3]];
        
        switch (COMPOUND(dest->type, src1->type, src2->type))
        {
          case COMPOUND(pal_IREG, pal_IREG, pal_CONST):
          {
            uint5 value = src2->info.value;
            genx86_regmove(nat, dest->info.ireg.num, src1->info.ireg.num);
            switch (opcode)
            {
              case ph_ADD:
              if (value<128)
                rtasm_add_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num), value);
              else
                rtasm_add_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num), 
                  value);
              break;
              
              case ph_ADC:
              if (value<128)
                rtasm_adc_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num), value);
              else
                rtasm_adc_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num), 
                  value);
              break;
              
              case ph_SUB:
              if (value<128)
                rtasm_sub_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num), value);
              else
                rtasm_sub_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num), 
                  value);
              break;

              case ph_SBC:
              if (value<128)
                rtasm_sbb_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num), value);
              else
                rtasm_sbb_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num), 
                  value);
              break;
            }
          }
          break;
          
          case COMPOUND(pal_IREG, pal_IREG, pal_IREG):
          {
            genx86_regmove(nat, dest->info.ireg.num, src1->info.ireg.num);
            switch (opcode)
            {
              case ph_ADD:
              rtasm_lea_r32_m32(nat, dest->info.ireg.num,
                rtasm_scind(src1->info.ireg.num, src2->info.ireg.num, scale_1));
              break;
              
              case ph_ADC:
              genx86_regmove(nat, dest->info.ireg.num, src1->info.ireg.num);
              rtasm_adc_r32_rm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num));
              break;
              
              case ph_SUB:
              genx86_regmove(nat, dest->info.ireg.num, src1->info.ireg.num);
              rtasm_sub_r32_rm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num));
              break;

              case ph_SBC:
              genx86_regmove(nat, dest->info.ireg.num, src1->info.ireg.num);
              rtasm_sbb_r32_rm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num));
              break;
            }
          }
          break;
          
          case COMPOUND(pal_IREG, pal_IREG, pal_RFILE):
          {
            genx86_regmove(nat, dest->info.ireg.num, src1->info.ireg.num);
            switch (opcode)
            {
              case ph_ADD:
              rtasm_add_r32_rm32(nat, dest->info.ireg.num,
                rtasm_ind8(EBP, src2->info.value*4));
              break;
              
              case ph_ADC:
              rtasm_adc_r32_rm32(nat, dest->info.ireg.num,
                rtasm_ind8(EBP, src2->info.value*4));
              break;
              
              case ph_SUB:
              rtasm_sub_r32_rm32(nat, dest->info.ireg.num,
                rtasm_ind8(EBP, src2->info.value*4));
              break;

              case ph_SBC:
              rtasm_sbb_r32_rm32(nat, dest->info.ireg.num,
                rtasm_ind8(EBP, src2->info.value*4));
              break;
            }
          }
          break;
          
          case COMPOUND(pal_IREG, pal_RFILE, pal_CONST):
          {
            uint5 value = src2->info.value;
            rtasm_mov_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
              src1->info.value));
            switch (opcode)
            {
              case ph_ADD:
              if (value<128)
                rtasm_add_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num), value);
              else
                rtasm_add_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num), 
                  value);
              break;

              case ph_ADC:
              if (value<128)
                rtasm_adc_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num), value);
              else
                rtasm_adc_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num), 
                  value);
              break;

              case ph_SUB:
              if (value<128)
                rtasm_sub_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num), value);
              else
                rtasm_sub_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num), 
                  value);
              break;

              case ph_SBC:
              if (value<128)
                rtasm_sbb_rm32_imm8(nat, rtasm_reg(dest->info.ireg.num), value);
              else
                rtasm_sbb_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num), 
                  value);
              break;
            }
          }
          break;
          
          case COMPOUND(pal_IREG, pal_RFILE, pal_IREG):
          {
            rtasm_mov_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
              src1->info.value*4));
            switch (opcode)
            {
              case ph_ADD:
              rtasm_add_r32_rm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num));
              break;
              
              case ph_ADC:
              rtasm_adc_r32_rm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num));
              break;
              
              case ph_SUB:
              rtasm_sub_r32_rm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num));
              break;

              case ph_SBC:
              rtasm_sbb_r32_rm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num));
              break;
            }
          }
          break;
          
          case COMPOUND(pal_IREG, pal_RFILE, pal_RFILE):
          {
            rtasm_mov_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
              src1->info.value*4));
            switch (opcode)
            {
              case ph_ADD:
              rtasm_add_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
                src2->info.value*4));
              break;

              case ph_ADC:
              rtasm_adc_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
                src2->info.value*4));
              break;

              case ph_SUB:
              rtasm_sub_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
                src2->info.value*4));
              break;

              case ph_SBC:
              rtasm_sbb_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
                src2->info.value*4));
              break;
            }
          }
          break;
          
          case COMPOUND(pal_IREG, pal_CONST, pal_CONST):
          {
            uint5 value;
            switch (opcode)
            {
              case ph_ADD:
              value = src1->info.value + src2->info.value;
              break;
              
              case ph_ADC:
              case ph_SBC:
              fprintf(stderr, "Allocation error\n");
              exit(1);
              break;
              
              case ph_SUB:
              value = src1->info.value - src2->info.value;
              break;
            }
            rtasm_mov_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num), value);
          }
          break;
          
          case COMPOUND(pal_IREG, pal_CONST, pal_IREG):
          {
            rtasm_mov_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num),
              src1->info.value);
            switch (opcode)
            {
              case ph_ADD:
              rtasm_add_r32_rm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num));
              break;
              
              case ph_ADC:
              rtasm_adc_r32_rm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num));
              break;
              
              case ph_SUB:
              rtasm_sub_r32_rm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num));
              break;

              case ph_SBC:
              rtasm_sbb_r32_rm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num));
              break;
            }
          }
          break;
          
          case COMPOUND(pal_IREG, pal_CONST, pal_RFILE):
          {
            rtasm_mov_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num),
              src1->info.value);
            switch (opcode)
            {
              case ph_ADD:
              rtasm_add_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
                src2->info.value*4));
              break;

              case ph_ADC:
              rtasm_adc_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
                src2->info.value*4));
              break;

              case ph_SUB:
              rtasm_sub_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
                src2->info.value*4));
              break;

              case ph_SBC:
              rtasm_sbb_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
                src2->info.value*4));
              break;
            }
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_IREG, pal_CONST):
          {
            uint5 value = src2->info.value;
            rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
              src1->info.ireg.num);
            switch (opcode)
            {
              case ph_ADD:
              if (value<128)
                rtasm_add_rm32_imm8(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              else
                rtasm_add_rm32_imm32(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              break;

              case ph_ADC:
              if (value<128)
                rtasm_adc_rm32_imm8(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              else
                rtasm_adc_rm32_imm32(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              break;

              case ph_SUB:
              if (value<128)
                rtasm_sub_rm32_imm8(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              else
                rtasm_sub_rm32_imm32(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              break;

              case ph_SBC:
              if (value<128)
                rtasm_sbb_rm32_imm8(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              else
                rtasm_sbb_rm32_imm32(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              break;
            }
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_IREG, pal_IREG):
          {
            rtasm_mov_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
              src1->info.ireg.num);
            switch (opcode)
            {
              case ph_ADD:
              rtasm_add_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;

              case ph_ADC:
              rtasm_adc_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;

              case ph_SUB:
              rtasm_sub_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;

              case ph_SBC:
              rtasm_sbb_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;
            }
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_IREG, pal_RFILE):
          {
            rtasm_nop(nat);
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_RFILE, pal_CONST):
          {
            uint5 value = src2->info.value;
            if (dest->info.value != src1->info.value)
            {
              fprintf(stderr, "Bad allocation\n");
              exit(1);
            }
            switch (opcode)
            {
              case ph_ADD:
              if (value<128)
                rtasm_add_rm32_imm8(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              else
                rtasm_add_rm32_imm32(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              break;

              case ph_ADC:
              if (value<128)
                rtasm_adc_rm32_imm8(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              else
                rtasm_adc_rm32_imm32(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              break;

              case ph_SUB:
              if (value<128)
                rtasm_sub_rm32_imm8(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              else
                rtasm_sub_rm32_imm32(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              break;

              case ph_SBC:
              if (value<128)
                rtasm_sbb_rm32_imm8(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              else
                rtasm_sbb_rm32_imm32(nat, rtasm_ind8(EBP, dest->info.value*4),
                  value);
              break;
            }
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_RFILE, pal_IREG):
          {
            if (dest->info.value != src1->info.value)
            {
              fprintf(stderr, "Bad allocation\n");
              exit(1);
            }
            switch (opcode)
            {
              case ph_ADD:
              rtasm_add_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;
              
              case ph_ADC:
              rtasm_adc_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;
              
              case ph_SUB:
              rtasm_sub_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;

              case ph_SBC:
              rtasm_sbb_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;
            }
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_RFILE, pal_RFILE):
          {
            rtasm_nop(nat);
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_CONST, pal_CONST):
          {
            uint5 value;
            switch (opcode)
            {
              case ph_ADD:
              value = src1->info.value + src2->info.value;
              break;
              
              case ph_SUB:
              value = src1->info.value - src2->info.value;
              break;
              
              case ph_ADC:
              case ph_SBC:
              fprintf(stderr, "Allocation error\n");
              exit(1);
              break;
            }
            rtasm_mov_rm32_imm32(nat, rtasm_ind8(EBP, dest->info.value*4),
              value);
          }
          break;

          case COMPOUND(pal_RFILE, pal_CONST, pal_IREG):
          {
            rtasm_mov_rm32_imm32(nat, rtasm_ind8(EBP, dest->info.value*4),
              src1->info.value);
            switch (opcode)
            {
              case ph_ADD:
              rtasm_add_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;

              case ph_ADC:
              rtasm_adc_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;

              case ph_SUB:
              rtasm_sub_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;

              case ph_SBC:
              rtasm_sbb_rm32_r32(nat, rtasm_ind8(EBP, dest->info.value*4),
                src2->info.ireg.num);
              break;
            }
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_CONST, pal_RFILE):
          {
            rtasm_nop(nat);
          }
          break;
        }
      }
      break;  // three-arg arithmetic operations
      
      case ph_MUL:
      {
        palloc_info* dest = &chunk->alloc[blk->base[i+1]];
        palloc_info* src1 = &chunk->alloc[blk->base[i+2]];
        palloc_info* src2 = &chunk->alloc[blk->base[i+3]];
        
        switch (COMPOUND(dest->type, src1->type, src2->type))
        {
          case COMPOUND(pal_IREG, pal_IREG, pal_CONST):
          {
            uint5 value = src2->info.value;
            if (value<128)
              rtasm_imul_r32_rm32_imm8(nat, dest->info.ireg.num,
                rtasm_reg(src1->info.ireg.num), src2->info.value);
            else
              rtasm_imul_r32_rm32_imm32(nat, dest->info.ireg.num,
                rtasm_reg(src1->info.ireg.num), src2->info.value);
          }
          break;
          
          case COMPOUND(pal_IREG, pal_IREG, pal_IREG):
          {
            rtasm_mov_r32_rm32(nat, dest->info.ireg.num, 
              rtasm_reg(src1->info.ireg.num));
            rtasm_imul_r32_rm32(nat, dest->info.ireg.num,
              rtasm_reg(src2->info.ireg.num));
          }
          break;
          
          case COMPOUND(pal_IREG, pal_IREG, pal_RFILE):
          {
            rtasm_mov_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
              src2->info.value*4));
            rtasm_imul_r32_rm32(nat, dest->info.ireg.num,
              rtasm_reg(src1->info.ireg.num));
          }
          break;
          
          case COMPOUND(pal_IREG, pal_RFILE, pal_CONST):
          {
            uint5 value = src2->info.value;
            if (value<128)
              rtasm_imul_r32_rm32_imm8(nat, dest->info.ireg.num, rtasm_ind8(EBP,
                src1->info.value*4), src2->info.value);
            else
              rtasm_imul_r32_rm32_imm32(nat, dest->info.ireg.num,
                rtasm_ind8(EBP, src1->info.value*4), src2->info.value);
          }
          break;
          
          case COMPOUND(pal_IREG, pal_RFILE, pal_IREG):
          {
            rtasm_mov_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
              src1->info.value*4));
            rtasm_imul_r32_rm32(nat, dest->info.ireg.num,
              rtasm_reg(src2->info.ireg.num));
          }
          break;
          
          case COMPOUND(pal_IREG, pal_RFILE, pal_RFILE):
          {
            rtasm_mov_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
              src1->info.value*4));
            rtasm_imul_r32_rm32(nat, dest->info.ireg.num, rtasm_ind8(EBP,
              src2->info.value*4));
          }
          break;
          
          case COMPOUND(pal_IREG, pal_CONST, pal_CONST):
          {
            rtasm_mov_rm32_imm32(nat, rtasm_reg(dest->info.ireg.num),
              src1->info.value * src2->info.value);
          }
          break;
          
          case COMPOUND(pal_IREG, pal_CONST, pal_IREG):
          {
            uint5 value = src1->info.value;
            if (value<128)
              rtasm_imul_r32_rm32_imm8(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num), value);
            else
              rtasm_imul_r32_rm32_imm32(nat, dest->info.ireg.num,
                rtasm_reg(src2->info.ireg.num), value);
          }
          break;
          
          case COMPOUND(pal_IREG, pal_CONST, pal_RFILE):
          {
            uint5 value = src1->info.value;
            if (value<128)
              rtasm_imul_r32_rm32_imm8(nat, dest->info.ireg.num, rtasm_ind8(EBP,
                src2->info.value*4), value);
            else
              rtasm_imul_r32_rm32_imm32(nat, dest->info.ireg.num,
                rtasm_ind8(EBP, src2->info.value*4), value);
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_IREG, pal_CONST):
          {
            rtasm_nop(nat);
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_IREG, pal_IREG):
          {
            rtasm_nop(nat);
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_IREG, pal_RFILE):
          {
            rtasm_nop(nat);
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_RFILE, pal_CONST):
          {
            rtasm_nop(nat);
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_RFILE, pal_IREG):
          {
            rtasm_nop(nat);
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_RFILE, pal_RFILE):
          {
            rtasm_nop(nat);
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_CONST, pal_CONST):
          {
            rtasm_nop(nat);
          }
          break;

          case COMPOUND(pal_RFILE, pal_CONST, pal_IREG):
          {
            rtasm_nop(nat);
          }
          break;
          
          case COMPOUND(pal_RFILE, pal_CONST, pal_RFILE):
          {
            rtasm_nop(nat);
          }
          break;
        }
      }
      break;

#undef COMPOUND
      
      case ph_RRX:
      case ph_RLX:
      break;  // two-arg shift operations
      
      case ph_MOV:
      case ph_NOT:
      rtasm_nop(nat);
      break;  // two-arg logic operations
      
      case ph_TEQ:
      case ph_TST:
      case ph_CMP:
      case ph_CMN:
      rtasm_nop(nat);
      break;  // comparison operations
      
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
    truebranch = genx86_translate(chunk, blk->trueblk);
  
  if (blk->falseblk && !blk->falseblk->marker)
    falsebranch = genx86_translate(chunk, blk->falseblk);

  /* join parent block to true/false blocks... */

  return nat;
}
