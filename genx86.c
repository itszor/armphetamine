#include "genx86.h"
#include "x86asm.h"
#include "x86dism.h"
#include "rtasm.h"

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
