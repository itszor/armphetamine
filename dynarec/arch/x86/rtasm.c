#include "defs.h"
#include "cnew.h"
#include "x86asm.h"
#include "rtasm.h"
#include "clist.h"

nativeblockinfo* rtasm_new(void)
{
  nativeblockinfo* nat = jt_new(nativeblockinfo);
  nat->base = jt_newarray(char, nat->size=2048);
  nat->length = 0;
  nat->reloc = 0;
  
  return nat;
}

void rtasm_delete(nativeblockinfo* nat)
{
  jt_delete(nat->base);
  jt_delete(nat);
}

void rtasm_putbyte(nativeblockinfo* nat, uint3 byte)
{
  nat->base[nat->length] = byte;
  if (++nat->length==nat->size)
    nat->base = realloc(nat->base, nat->size *= 2);
}

void rtasm_opsize16(nativeblockinfo* nat)
{
  rtasm_putbyte(nat, 0x66);
}

void rtasm_offsize16(nativeblockinfo* nat)
{
  rtasm_putbyte(nat, 0x67);
}

void rtasm_mem(nativeblockinfo* nat, rtasm_mtype* mem)
{
  switch (mem->mod)
  {
    case mod_INDIRECT:
    {
      if (mem->rm == USESIB)
      {
        rtasm_putbyte(nat, (mem->scale<<6) | (mem->index<<3) | mem->base);
      }
      else if (mem->rm == USEDISP)
      {
        rtasm_putbyte(nat, mem->disp & 0xff);
        rtasm_putbyte(nat, (mem->disp>>8) & 0xff);
        rtasm_putbyte(nat, (mem->disp>>16) & 0xff);
        rtasm_putbyte(nat, mem->disp>>24);
      }
      // else nothing else needs to be added
    }
    break;
    
    case mod_DISP8:
    {
      if (mem->rm == USESIB)
      {
        rtasm_putbyte(nat, (mem->scale<<6) | (mem->index<<3) | mem->base);
      }
      rtasm_putbyte(nat, mem->disp);
    }
    break;
    
    case mod_DISP32:
    {
      if (mem->rm == USESIB)
      {
        rtasm_putbyte(nat, (mem->scale<<6) | (mem->index<<3) | mem->base);
      }
      rtasm_putbyte(nat, mem->disp & 0xff);
      rtasm_putbyte(nat, (mem->disp>>8) & 0xff);
      rtasm_putbyte(nat, (mem->disp>>16) & 0xff);
      rtasm_putbyte(nat, mem->disp>>24);
    }
    break;
    
    case mod_REGISTER:
    // nothing extra needs to be done
    break;
  }
}

// use with care, I guess.
rtasm_mtype rtasm_reg(uint3 r)
{
  rtasm_mtype thing;
  thing.mod = mod_REGISTER;
  thing.rm = r;
  return thing;
}

rtasm_mtype rtasm_ind(uint3 r)
{
  rtasm_mtype thing;
  thing.mod = mod_INDIRECT;
  thing.rm = r;
  return thing;
}

rtasm_mtype rtasm_ind8(uint3 base, uint3 offset)
{
  rtasm_mtype thing;
  thing.mod = mod_DISP8;
  thing.rm = base;
  thing.disp = offset;
  return thing;
}

rtasm_mtype rtasm_ind32(uint3 base, uint5 offset)
{
  rtasm_mtype thing;
  thing.mod = mod_DISP32;
  thing.rm = base;
  thing.disp = offset;
  return thing;
}

rtasm_mtype rtasm_scind(uint3 base, uint3 index, uint3 scale)
{
  rtasm_mtype thing;
  thing.mod = mod_INDIRECT;
  thing.rm = USESIB;
  thing.base = base;
  thing.index = index;
  thing.scale = scale;
  return thing;
}

rtasm_mtype rtasm_scind8(uint3 base, uint3 index, uint3 scale, uint3 offset)
{
  rtasm_mtype thing;
  thing.mod = mod_DISP8;
  thing.rm = USESIB;
  thing.base = base;
  thing.index = index;
  thing.scale = scale;
  thing.disp = offset;
  return thing;
}

rtasm_mtype rtasm_scind32(uint3 base, uint3 index, uint3 scale, uint5 offset)
{
  rtasm_mtype thing;
  thing.mod = mod_DISP32;
  thing.rm = USESIB;
  thing.base = base;
  thing.index = index;
  thing.scale = scale;
  thing.disp = offset;
  return thing;
}

rtasm_mtype rtasm_addr(uint5 address)
{
  rtasm_mtype thing;
  thing.mod = mod_INDIRECT;
  thing.rm = USEDISP;
  thing.disp = address;
  return thing;
}

/*
void rtasm_test(void)
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
*/

