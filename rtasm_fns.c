#include "x86asm.h"
#include "rtasm.h"
/* AAA */
void rtasm_aaa(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x37);
}
/* AAD */
void rtasm_aad(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD5);
	rtasm_putbyte(nat, 0x0A);
}
/* AAM */
void rtasm_aam(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD4);
	rtasm_putbyte(nat, 0x0A);
}
/* AAS */
void rtasm_aas(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x3F);
}
/* ADC r/m8,r8 */
void rtasm_adc_rm8_r8(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x10);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ADC r/m16,r16 */
void rtasm_adc_rm16_r16(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x11);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ADC r/m32,r32 */
void rtasm_adc_rm32_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x11);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ADC r8,r/m8 */
void rtasm_adc_r8_rm8(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x12);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ADC r16,r/m16 */
void rtasm_adc_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x13);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ADC r32,r/m32 */
void rtasm_adc_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x13);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ADD r/m8,r8 */
void rtasm_add_rm8_r8(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x00);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ADD r/m16,r16 */
void rtasm_add_rm16_r16(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x01);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ADD r/m32,r32 */
void rtasm_add_rm32_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x01);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ADD r8,r/m8 */
void rtasm_add_r8_rm8(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x02);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ADD r16,r/m16 */
void rtasm_add_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x03);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ADD r32,r/m32 */
void rtasm_add_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x03);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* AND r/m8,r8 */
void rtasm_and_rm8_r8(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x20);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* AND r/m16,r16 */
void rtasm_and_rm16_r16(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x21);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* AND r/m32,r32 */
void rtasm_and_rm32_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x21);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* AND r8,r/m8 */
void rtasm_and_r8_rm8(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x22);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* AND r16,r/m16 */
void rtasm_and_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x23);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* AND r32,r/m32 */
void rtasm_and_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x23);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ARPL r/m16,r16 */
void rtasm_arpl_rm16_r16(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x63);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* BOUND r16,m16&16 */
void rtasm_bound_r16_m16x2(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x62);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* BOUND r32,m32&32 */
void rtasm_bound_r32_m32x2(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x62);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* BSF r16,r/m16 */
void rtasm_bsf_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xBC);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* BSF r32,r/m32 */
void rtasm_bsf_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xBC);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* BSR r16,r/m16 */
void rtasm_bsr_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xBD);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* BSR r32,r/m32 */
void rtasm_bsr_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xBD);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* BSWAP r32 */
void rtasm_bswap_r32(nativeblockinfo* nat, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xC8+reg);
}
/* BT r/m16,r16 */
void rtasm_bt_rm16_r16(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xA3);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* BT r/m32,r32 */
void rtasm_bt_rm32_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xA3);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* BTC r/m16,r16 */
void rtasm_btc_rm16_r16(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xBB);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* BTC r/m32,r32 */
void rtasm_btc_rm32_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xBB);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* BTR r/m16,r16 */
void rtasm_btr_rm16_r16(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xB3);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* BTR r/m32,r32 */
void rtasm_btr_rm32_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xB3);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* BTS r/m16,r16 */
void rtasm_bts_rm16_r16(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xAB);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* BTS r/m32,r32 */
void rtasm_bts_rm32_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xAB);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CALL rel16 */
void rtasm_call_rel16(nativeblockinfo* nat, uint4 offset)
{
	rtasm_putbyte(nat, 0xE8);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* CALL rel32 */
void rtasm_call_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0xE8);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, (offset>>8) & 0xff);
	rtasm_putbyte(nat, (offset>>16) & 0xff);
	rtasm_putbyte(nat, offset>>24);
}
/* CALL r/m16 */
void rtasm_call_rm16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xFF);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CALL r/m32 */
void rtasm_call_rm32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xFF);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CALL ptr16:16 */
void rtasm_call_ptr16_16(nativeblockinfo* nat, uint4 segment, uint4 offset)
{
	offset = segment | (offset<<16);
	rtasm_putbyte(nat, 0x9A);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, (offset>>8) & 0xff);
	rtasm_putbyte(nat, (offset>>16) & 0xff);
	rtasm_putbyte(nat, offset>>24);
}
/* CALL ptr16:32 */
void rtasm_call_ptr16_32(nativeblockinfo* nat, uint4 segment, uint5 offset)
{
	rtasm_putbyte(nat, 0x9A);
	rtasm_putbyte(nat, segment & 0xff);
	rtasm_putbyte(nat, segment>>8);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, (offset>>8) & 0xff);
	rtasm_putbyte(nat, (offset>>16) & 0xff);
	rtasm_putbyte(nat, offset>>24);
}
/* CALL m16:16 */
void rtasm_call_m16_16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xFF);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CALL m16:32 */
void rtasm_call_m16_32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xFF);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CBW */
void rtasm_cbw(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x98);
}
/* CWDE */
void rtasm_cwde(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x98);
}
/* CLC */
void rtasm_clc(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xF8);
}
/* CLD */
void rtasm_cld(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xFC);
}
/* CLI */
void rtasm_cli(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xFA);
}
/* CLTS */
void rtasm_clts(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x06);
}
/* CMC */
void rtasm_cmc(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xF5);
}
/* CMOVA r16,r/m16 */
void rtasm_cmova_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x47);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVA r32,r/m32 */
void rtasm_cmova_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x47);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVAE r16,r/m16 */
void rtasm_cmovae_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x43);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVAE r32,r/m32 */
void rtasm_cmovae_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x43);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVB r16,r/m16 */
void rtasm_cmovb_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x42);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVB r32,r/m32 */
void rtasm_cmovb_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x42);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVBE r16,r/m16 */
void rtasm_cmovbe_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x46);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVBE r32,r/m32 */
void rtasm_cmovbe_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x46);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVC r16,r/m16 */
void rtasm_cmovc_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x42);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVC r32,r/m32 */
void rtasm_cmovc_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x42);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVE r16,r/m16 */
void rtasm_cmove_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x44);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVE r32,r/m32 */
void rtasm_cmove_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x44);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVG r16,r/m16 */
void rtasm_cmovg_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4F);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVG r32,r/m32 */
void rtasm_cmovg_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4F);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVGE r16,r/m16 */
void rtasm_cmovge_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4D);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVGE r32,r/m32 */
void rtasm_cmovge_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4D);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVL r16,r/m16 */
void rtasm_cmovl_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4C);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVL r32,r/m32 */
void rtasm_cmovl_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4C);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVLE r16,r/m16 */
void rtasm_cmovle_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4E);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVLE r32,r/m32 */
void rtasm_cmovle_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4E);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNA r16,r/m16 */
void rtasm_cmovna_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x46);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNA r32,r/m32 */
void rtasm_cmovna_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x46);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNAE r16,r/m16 */
void rtasm_cmovnae_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x42);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNAE r32,r/m32 */
void rtasm_cmovnae_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x42);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNB r16,r/m16 */
void rtasm_cmovnb_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x43);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNB r32,r/m32 */
void rtasm_cmovnb_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x43);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNBE r16,r/m16 */
void rtasm_cmovnbe_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x47);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNBE r32,r/m32 */
void rtasm_cmovnbe_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x47);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNC r16,r/m16 */
void rtasm_cmovnc_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x43);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNC r32,r/m32 */
void rtasm_cmovnc_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x43);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNE r16,r/m16 */
void rtasm_cmovne_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x45);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNE r32,r/m32 */
void rtasm_cmovne_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x45);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNG r16,r/m16 */
void rtasm_cmovng_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4E);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNG r32,r/m32 */
void rtasm_cmovng_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4E);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNGE r16,r/m16 */
void rtasm_cmovnge_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4C);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNGE r32,r/m32 */
void rtasm_cmovnge_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4C);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNL r16,r/m16 */
void rtasm_cmovnl_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4D);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNL r32,r/m32 */
void rtasm_cmovnl_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4D);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNLE r16,r/m16 */
void rtasm_cmovnle_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4F);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNLE r32,r/m32 */
void rtasm_cmovnle_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4F);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNO r16,r/m16 */
void rtasm_cmovno_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x41);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNO r32,r/m32 */
void rtasm_cmovno_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x41);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNP r16,r/m16 */
void rtasm_cmovnp_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4B);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNP r32,r/m32 */
void rtasm_cmovnp_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4B);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNS r16,r/m16 */
void rtasm_cmovns_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x49);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNS r32,r/m32 */
void rtasm_cmovns_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x49);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNZ r16,r/m16 */
void rtasm_cmovnz_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x45);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVNZ r32,r/m32 */
void rtasm_cmovnz_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x45);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVO r16,r/m16 */
void rtasm_cmovo_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x40);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVO r32,r/m32 */
void rtasm_cmovo_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x40);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVP r16,r/m16 */
void rtasm_cmovp_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4A);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVP r32,r/m32 */
void rtasm_cmovp_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4A);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVPE r16,r/m16 */
void rtasm_cmovpe_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4A);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVPE r32,r/m32 */
void rtasm_cmovpe_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4A);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVPO r16,r/m16 */
void rtasm_cmovpo_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4B);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVPO r32,r/m32 */
void rtasm_cmovpo_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x4B);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVS r16,r/m16 */
void rtasm_cmovs_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x48);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVS r32,r/m32 */
void rtasm_cmovs_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x48);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVZ r16,r/m16 */
void rtasm_cmovz_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x44);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMOVZ r32,r/m32 */
void rtasm_cmovz_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x44);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMP r/m8,r8 */
void rtasm_cmp_rm8_r8(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x38);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMP r/m16,r16 */
void rtasm_cmp_rm16_r16(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x39);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMP r/m32,r32 */
void rtasm_cmp_rm32_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x39);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMP r8,r/m8 */
void rtasm_cmp_r8_rm8(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x3A);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMP r16,r/m16 */
void rtasm_cmp_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x3B);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMP r32,r/m32 */
void rtasm_cmp_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x3B);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMPSB */
void rtasm_cmpsb(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xA6);
}
/* CMPSW */
void rtasm_cmpsw(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xA7);
}
/* CMPSD */
void rtasm_cmpsd(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xA7);
}
/* CMPXCHG r/m8,r8 */
void rtasm_cmpxchg_rm8_r8(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xB0);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMPXCHG r/m16,r16 */
void rtasm_cmpxchg_rm16_r16(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xB1);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMPXCHG r/m32,r32 */
void rtasm_cmpxchg_rm32_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xB1);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CMPXCHG8B m64 */
void rtasm_cmpxchg8b_m64(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xC7);
	rtasm_putbyte(nat, mem->rm | 8 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* CPUID */
void rtasm_cpuid(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xA2);
}
/* CWD */
void rtasm_cwd(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x99);
}
/* CDQ */
void rtasm_cdq(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x99);
}
/* DAA */
void rtasm_daa(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x27);
}
/* DAS */
void rtasm_das(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x2F);
}
/* DEC r/m8 */
void rtasm_dec_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xFE);
	rtasm_putbyte(nat, mem->rm | 8 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* DEC r/m16 */
void rtasm_dec_rm16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xFF);
	rtasm_putbyte(nat, mem->rm | 8 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* DEC r/m32 */
void rtasm_dec_rm32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xFF);
	rtasm_putbyte(nat, mem->rm | 8 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* DEC r16 */
void rtasm_dec_r16(nativeblockinfo* nat, uint3 reg)
{
	rtasm_putbyte(nat, 0x48+reg);
}
/* DEC r32 */
void rtasm_dec_r32(nativeblockinfo* nat, uint3 reg)
{
	rtasm_putbyte(nat, 0x48+reg);
}
/* DIV r/m8 */
void rtasm_div_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xF6);
	rtasm_putbyte(nat, mem->rm | 48 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* DIV r/m16 */
void rtasm_div_rm16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xF7);
	rtasm_putbyte(nat, mem->rm | 48 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* DIV r/m32 */
void rtasm_div_rm32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xF7);
	rtasm_putbyte(nat, mem->rm | 48 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* EMMS */
void rtasm_emms(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x77);
}
/* F2XM1 */
void rtasm_f2xm1(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xF0);
}
/* FABS */
void rtasm_fabs(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xE1);
}
/* FADD m32real */
void rtasm_fadd_m32real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD8);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FADD m64real */
void rtasm_fadd_m64real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDC);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FADD ST(0),ST(i) */
void rtasm_fadd_st0_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xD8);
	rtasm_putbyte(nat, 0xC0+stackn);
}
/* FADD ST(i),ST(0) */
void rtasm_fadd_sti_st0(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDC);
	rtasm_putbyte(nat, 0xC0+stackn);
}
/* FADDP ST(i),ST(0) */
void rtasm_faddp_sti_st0(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, 0xC0+stackn);
}
/* FADDP */
void rtasm_faddp(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, 0xC1);
}
/* FIADD m32int */
void rtasm_fiadd_m32int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDA);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FIADD m16int */
void rtasm_fiadd_m16int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FBLD m80dec */
void rtasm_fbld_m80dec(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDF);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FBSTP m80bcd */
void rtasm_fbstp_m80bcd(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDF);
	rtasm_putbyte(nat, mem->rm | 48 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FCHS */
void rtasm_fchs(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xE0);
}
/* FCLEX */
void rtasm_fclex(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x9B);
	rtasm_putbyte(nat, 0xDB);
	rtasm_putbyte(nat, 0xE2);
}
/* FNCLEX */
void rtasm_fnclex(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xDB);
	rtasm_putbyte(nat, 0xE2);
}
/* FCMOVB ST(0),ST(i) */
void rtasm_fcmovb_st0_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDA);
	rtasm_putbyte(nat, 0xC0+stackn);
}
/* FCMOVE ST(0),ST(i) */
void rtasm_fcmove_st0_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDA);
	rtasm_putbyte(nat, 0xC8+stackn);
}
/* FCMOVBE ST(0),ST(i) */
void rtasm_fcmovbe_st0_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDA);
	rtasm_putbyte(nat, 0xD0+stackn);
}
/* FCMOVU ST(0),ST(i) */
void rtasm_fcmovu_st0_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDA);
	rtasm_putbyte(nat, 0xD8+stackn);
}
/* FCMOVNB ST(0),ST(i) */
void rtasm_fcmovnb_st0_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDB);
	rtasm_putbyte(nat, 0xC0+stackn);
}
/* FCMOVNE ST(0),ST(i) */
void rtasm_fcmovne_st0_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDB);
	rtasm_putbyte(nat, 0xC8+stackn);
}
/* FCMOVNBE ST(0),ST(i) */
void rtasm_fcmovnbe_st0_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDB);
	rtasm_putbyte(nat, 0xD0+stackn);
}
/* FCMOVNU ST(0),ST(i) */
void rtasm_fcmovnu_st0_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDB);
	rtasm_putbyte(nat, 0xD8+stackn);
}
/* FCOM m32real */
void rtasm_fcom_m32real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD8);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FCOM m64real */
void rtasm_fcom_m64real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDC);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FCOM ST(i) */
void rtasm_fcom_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xD8);
	rtasm_putbyte(nat, 0xD0+stackn);
}
/* FCOM */
void rtasm_fcom(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD8);
	rtasm_putbyte(nat, 0xD1);
}
/* FCOMP m32real */
void rtasm_fcomp_m32real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD8);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FCOMP m64real */
void rtasm_fcomp_m64real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDC);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FCOMP ST(i) */
void rtasm_fcomp_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xD8);
	rtasm_putbyte(nat, 0xD8+stackn);
}
/* FCOMP */
void rtasm_fcomp(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD8);
	rtasm_putbyte(nat, 0xD9);
}
/* FCOMPP */
void rtasm_fcompp(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, 0xD9);
}
/* FCOMI ST,ST(i) */
void rtasm_fcomi_st_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDB);
	rtasm_putbyte(nat, 0xF0+stackn);
}
/* FCOMIP ST,ST(i) */
void rtasm_fcomip_st_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDF);
	rtasm_putbyte(nat, 0xF0+stackn);
}
/* FUCOMI ST,ST(i) */
void rtasm_fucomi_st_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDB);
	rtasm_putbyte(nat, 0xE8+stackn);
}
/* FUCOMIP ST,ST(i) */
void rtasm_fucomip_st_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDF);
	rtasm_putbyte(nat, 0xE8+stackn);
}
/* FCOS */
void rtasm_fcos(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xFF);
}
/* FDECSTP */
void rtasm_fdecstp(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xF6);
}
/* FDIV m32real */
void rtasm_fdiv_m32real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD8);
	rtasm_putbyte(nat, mem->rm | 48 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FDIV m64real */
void rtasm_fdiv_m64real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDC);
	rtasm_putbyte(nat, mem->rm | 48 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FDIV ST(0),ST(i) */
void rtasm_fdiv_st0_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xD8);
	rtasm_putbyte(nat, 0xF0+stackn);
}
/* FDIV ST(i),ST(0) */
void rtasm_fdiv_sti_st0(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDC);
	rtasm_putbyte(nat, 0xF8+stackn);
}
/* FDIVP ST(i),ST(0) */
void rtasm_fdivp_sti_st0(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, 0xF8+stackn);
}
/* FDIVP */
void rtasm_fdivp(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, 0xF9);
}
/* FIDIV m32int */
void rtasm_fidiv_m32int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDA);
	rtasm_putbyte(nat, mem->rm | 48 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FIDIV m16int */
void rtasm_fidiv_m16int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, mem->rm | 48 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FDIVR m32real */
void rtasm_fdivr_m32real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD8);
	rtasm_putbyte(nat, mem->rm | 56 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FDIVR m64real */
void rtasm_fdivr_m64real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDC);
	rtasm_putbyte(nat, mem->rm | 56 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FDIVR ST(0),ST(i) */
void rtasm_fdivr_st0_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xD8);
	rtasm_putbyte(nat, 0xF8+stackn);
}
/* FDIVR ST(i),ST(0) */
void rtasm_fdivr_sti_st0(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDC);
	rtasm_putbyte(nat, 0xF0+stackn);
}
/* FDIVRP ST(i),ST(0) */
void rtasm_fdivrp_sti_st0(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, 0xF0+stackn);
}
/* FDIVRP */
void rtasm_fdivrp(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, 0xF1);
}
/* FIDIVR m32int */
void rtasm_fidivr_m32int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDA);
	rtasm_putbyte(nat, mem->rm | 56 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FIDIVR m16int */
void rtasm_fidivr_m16int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, mem->rm | 56 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FFREE ST(i) */
void rtasm_ffree_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDD);
	rtasm_putbyte(nat, 0xC0+stackn);
}
/* FICOM m16int */
void rtasm_ficom_m16int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FICOM m32int */
void rtasm_ficom_m32int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDA);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FICOMP m16int */
void rtasm_ficomp_m16int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FICOMP m32int */
void rtasm_ficomp_m32int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDA);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FILD m16int */
void rtasm_fild_m16int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDF);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FILD m32int */
void rtasm_fild_m32int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDB);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FILD m64int */
void rtasm_fild_m64int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDF);
	rtasm_putbyte(nat, mem->rm | 40 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FINCSTP */
void rtasm_fincstp(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xF7);
}
/* FINIT */
void rtasm_finit(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x9B);
	rtasm_putbyte(nat, 0xDB);
	rtasm_putbyte(nat, 0xE3);
}
/* FNINIT */
void rtasm_fninit(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xDB);
	rtasm_putbyte(nat, 0xE3);
}
/* FIST m16int */
void rtasm_fist_m16int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDF);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FIST m32int */
void rtasm_fist_m32int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDB);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FISTP m16int */
void rtasm_fistp_m16int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDF);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FISTP m32int */
void rtasm_fistp_m32int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDB);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FISTP m64int */
void rtasm_fistp_m64int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDF);
	rtasm_putbyte(nat, mem->rm | 56 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FLD m32real */
void rtasm_fld_m32real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FLD m64real */
void rtasm_fld_m64real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDD);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FLD m80real */
void rtasm_fld_m80real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDB);
	rtasm_putbyte(nat, mem->rm | 40 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FLD ST(i) */
void rtasm_fld_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xC0+stackn);
}
/* FLD1 */
void rtasm_fld1(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xE8);
}
/* FLDL2T */
void rtasm_fldl2t(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xE9);
}
/* FLDL2E */
void rtasm_fldl2e(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xEA);
}
/* FLDPI */
void rtasm_fldpi(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xEB);
}
/* FLDLG2 */
void rtasm_fldlg2(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xEC);
}
/* FLDLN2 */
void rtasm_fldln2(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xED);
}
/* FLDZ */
void rtasm_fldz(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xEE);
}
/* FLDCW m2byte */
void rtasm_fldcw_m2byte(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, mem->rm | 40 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FLDENV m14/28byte */
void rtasm_fldenv_m1428byte(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FMUL m32real */
void rtasm_fmul_m32real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD8);
	rtasm_putbyte(nat, mem->rm | 8 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FMUL m64real */
void rtasm_fmul_m64real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDC);
	rtasm_putbyte(nat, mem->rm | 8 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FMUL ST(0),ST(i) */
void rtasm_fmul_st0_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xD8);
	rtasm_putbyte(nat, 0xC8+stackn);
}
/* FMUL ST(i),ST(0) */
void rtasm_fmul_sti_st0(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDC);
	rtasm_putbyte(nat, 0xC8+stackn);
}
/* FMULP ST(i),ST(0) */
void rtasm_fmulp_sti_st0(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, 0xC8+stackn);
}
/* FMULP */
void rtasm_fmulp(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, 0xC9);
}
/* FIMUL m32int */
void rtasm_fimul_m32int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDA);
	rtasm_putbyte(nat, mem->rm | 8 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FIMUL m16int */
void rtasm_fimul_m16int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, mem->rm | 8 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FNOP */
void rtasm_fnop(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xD0);
}
/* FPATAN */
void rtasm_fpatan(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xF3);
}
/* FPREM */
void rtasm_fprem(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xF8);
}
/* FPREM1 */
void rtasm_fprem1(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xF5);
}
/* FRNDINT */
void rtasm_frndint(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xFC);
}
/* FRSTOR m94/108byte */
void rtasm_frstor_m94108byte(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDD);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FSAVE m94/108byte */
void rtasm_fsave_m94108byte(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x9B);
	rtasm_putbyte(nat, 0xDD);
	rtasm_putbyte(nat, mem->rm | 48 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FNSAVE m94/108byte */
void rtasm_fnsave_m94108byte(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDD);
	rtasm_putbyte(nat, mem->rm | 48 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FSCALE */
void rtasm_fscale(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xFD);
}
/* FSIN */
void rtasm_fsin(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xFE);
}
/* FSINCOS */
void rtasm_fsincos(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xFB);
}
/* FSQRT */
void rtasm_fsqrt(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xFA);
}
/* FST m32real */
void rtasm_fst_m32real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FST m64real */
void rtasm_fst_m64real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDD);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FST ST(i) */
void rtasm_fst_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDD);
	rtasm_putbyte(nat, 0xD0+stackn);
}
/* FSTP m32real */
void rtasm_fstp_m32real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FSTP m64real */
void rtasm_fstp_m64real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDD);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FSTP m80real */
void rtasm_fstp_m80real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDB);
	rtasm_putbyte(nat, mem->rm | 56 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FSTP ST(i) */
void rtasm_fstp_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDD);
	rtasm_putbyte(nat, 0xD8+stackn);
}
/* FSTCW m2byte */
void rtasm_fstcw_m2byte(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x9B);
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, mem->rm | 56 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FNSTCW m2byte */
void rtasm_fnstcw_m2byte(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, mem->rm | 56 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FSTENV m14/28byte */
void rtasm_fstenv_m1428byte(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x9B);
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, mem->rm | 48 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FNSTENV m14/28byte */
void rtasm_fnstenv_m1428byte(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, mem->rm | 48 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FSTSW m2byte */
void rtasm_fstsw_m2byte(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x9B);
	rtasm_putbyte(nat, 0xDD);
	rtasm_putbyte(nat, mem->rm | 56 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FSTSW AX */
void rtasm_fstsw_ax(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x9B);
	rtasm_putbyte(nat, 0xDF);
	rtasm_putbyte(nat, 0xE0);
}
/* FNSTSW m2byte */
void rtasm_fnstsw_m2byte(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDD);
	rtasm_putbyte(nat, mem->rm | 56 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FNSTSW AX */
void rtasm_fnstsw_ax(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xDF);
	rtasm_putbyte(nat, 0xE0);
}
/* FSUB m32real */
void rtasm_fsub_m32real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD8);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FSUB m64real */
void rtasm_fsub_m64real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDC);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FSUB ST(0),ST(i) */
void rtasm_fsub_st0_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xD8);
	rtasm_putbyte(nat, 0xE0+stackn);
}
/* FSUB ST(i),ST(0) */
void rtasm_fsub_sti_st0(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDC);
	rtasm_putbyte(nat, 0xE8+stackn);
}
/* FSUBP ST(i),ST(0) */
void rtasm_fsubp_sti_st0(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, 0xE8+stackn);
}
/* FSUBP */
void rtasm_fsubp(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, 0xE9);
}
/* FISUB m32int */
void rtasm_fisub_m32int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDA);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FISUB m16int */
void rtasm_fisub_m16int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FSUBR m32real */
void rtasm_fsubr_m32real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD8);
	rtasm_putbyte(nat, mem->rm | 40 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FSUBR m64real */
void rtasm_fsubr_m64real(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDC);
	rtasm_putbyte(nat, mem->rm | 40 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FSUBR ST(0),ST(i) */
void rtasm_fsubr_st0_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xD8);
	rtasm_putbyte(nat, 0xE8+stackn);
}
/* FSUBR ST(i),ST(0) */
void rtasm_fsubr_sti_st0(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDC);
	rtasm_putbyte(nat, 0xE0+stackn);
}
/* FSUBRP ST(i),ST(0) */
void rtasm_fsubrp_sti_st0(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, 0xE0+stackn);
}
/* FSUBRP */
void rtasm_fsubrp(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, 0xE1);
}
/* FISUBR m32int */
void rtasm_fisubr_m32int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDA);
	rtasm_putbyte(nat, mem->rm | 40 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FISUBR m16int */
void rtasm_fisubr_m16int(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xDE);
	rtasm_putbyte(nat, mem->rm | 40 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FTST */
void rtasm_ftst(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xE4);
}
/* FUCOM ST(i) */
void rtasm_fucom_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDD);
	rtasm_putbyte(nat, 0xE0+stackn);
}
/* FUCOM */
void rtasm_fucom(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xDD);
	rtasm_putbyte(nat, 0xE1);
}
/* FUCOMP ST(i) */
void rtasm_fucomp_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xDD);
	rtasm_putbyte(nat, 0xE8+stackn);
}
/* FUCOMP */
void rtasm_fucomp(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xDD);
	rtasm_putbyte(nat, 0xE9);
}
/* FUCOMPP */
void rtasm_fucompp(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xDA);
	rtasm_putbyte(nat, 0xE9);
}
/* FXAM */
void rtasm_fxam(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xE5);
}
/* FXCH ST(i) */
void rtasm_fxch_sti(nativeblockinfo* nat, uint3 stackn)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xC8+stackn);
}
/* FXCH */
void rtasm_fxch(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xC9);
}
/* FXRSTOR m512byte */
void rtasm_fxrstor_m512byte(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xAE);
	rtasm_putbyte(nat, mem->rm | 8 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FXSAVE m512byte */
void rtasm_fxsave_m512byte(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xAE);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* FXTRACT */
void rtasm_fxtract(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xF4);
}
/* FYL2X */
void rtasm_fyl2x(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xF1);
}
/* FYL2XP1 */
void rtasm_fyl2xp1(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xD9);
	rtasm_putbyte(nat, 0xF9);
}
/* HLT */
void rtasm_hlt(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xF4);
}
/* IDIV r/m8 */
void rtasm_idiv_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xF6);
	rtasm_putbyte(nat, mem->rm | 56 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* IDIV r/m16 */
void rtasm_idiv_rm16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xF7);
	rtasm_putbyte(nat, mem->rm | 56 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* IDIV r/m32 */
void rtasm_idiv_rm32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xF7);
	rtasm_putbyte(nat, mem->rm | 56 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* IMUL r/m8 */
void rtasm_imul_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xF6);
	rtasm_putbyte(nat, mem->rm | 40 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* IMUL r/m16 */
void rtasm_imul_rm16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xF7);
	rtasm_putbyte(nat, mem->rm | 40 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* IMUL r/m32 */
void rtasm_imul_rm32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xF7);
	rtasm_putbyte(nat, mem->rm | 40 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* IMUL r16,r/m16 */
void rtasm_imul_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xAF);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* IMUL r32,r/m32 */
void rtasm_imul_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xAF);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* IN AL,DX */
void rtasm_in_al_dx(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xEC);
}
/* IN AX,DX */
void rtasm_in_ax_dx(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xED);
}
/* IN EAX,DX */
void rtasm_in_eax_dx(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xED);
}
/* INC r/m8 */
void rtasm_inc_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xFE);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* INC r/m16 */
void rtasm_inc_rm16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xFF);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* INC r/m32 */
void rtasm_inc_rm32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xFF);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* INC r16 */
void rtasm_inc_r16(nativeblockinfo* nat, uint3 reg)
{
}
/* INC r32 */
void rtasm_inc_r32(nativeblockinfo* nat, uint3 reg)
{
}
/* INS m8,DX */
void rtasm_ins_m8_dx(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x6C);
}
/* INS m16,DX */
void rtasm_ins_m16_dx(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x6D);
}
/* INS m32,DX */
void rtasm_ins_m32_dx(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x6D);
}
/* INSB */
void rtasm_insb(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x6C);
}
/* INSW */
void rtasm_insw(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x6D);
}
/* INSD */
void rtasm_insd(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x6D);
}
/* INT 3 */
void rtasm_int_3(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xCC);
}
/* INTO */
void rtasm_into(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xCE);
}
/* INVD */
void rtasm_invd(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x08);
}
/* INVLPG m */
void rtasm_invlpg_m(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x01);
	rtasm_putbyte(nat, mem->rm | 56 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* IRET */
void rtasm_iret(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xCF);
}
/* IRETD */
void rtasm_iretd(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xCF);
}
/* JA rel8 */
void rtasm_ja_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x77);
}
/* JAE rel8 */
void rtasm_jae_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x73);
}
/* JB rel8 */
void rtasm_jb_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x72);
}
/* JBE rel8 */
void rtasm_jbe_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x76);
}
/* JC rel8 */
void rtasm_jc_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x72);
}
/* JCXZ rel8 */
void rtasm_jcxz_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0xE3);
}
/* JECXZ rel8 */
void rtasm_jecxz_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0xE3);
}
/* JE rel8 */
void rtasm_je_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x74);
}
/* JG rel8 */
void rtasm_jg_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x7F);
}
/* JGE rel8 */
void rtasm_jge_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x7D);
}
/* JL rel8 */
void rtasm_jl_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x7C);
}
/* JLE rel8 */
void rtasm_jle_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x7E);
}
/* JNA rel8 */
void rtasm_jna_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x76);
}
/* JNAE rel8 */
void rtasm_jnae_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x72);
}
/* JNB rel8 */
void rtasm_jnb_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x73);
}
/* JNBE rel8 */
void rtasm_jnbe_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x77);
}
/* JNC rel8 */
void rtasm_jnc_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x73);
}
/* JNE rel8 */
void rtasm_jne_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x75);
}
/* JNG rel8 */
void rtasm_jng_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x7E);
}
/* JNGE rel8 */
void rtasm_jnge_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x7C);
}
/* JNL rel8 */
void rtasm_jnl_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x7D);
}
/* JNLE rel8 */
void rtasm_jnle_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x7F);
}
/* JNO rel8 */
void rtasm_jno_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x71);
}
/* JNP rel8 */
void rtasm_jnp_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x7B);
}
/* JNS rel8 */
void rtasm_jns_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x79);
}
/* JNZ rel8 */
void rtasm_jnz_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x75);
}
/* JO rel8 */
void rtasm_jo_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x70);
}
/* JP rel8 */
void rtasm_jp_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x7A);
}
/* JPE rel8 */
void rtasm_jpe_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x7A);
}
/* JPO rel8 */
void rtasm_jpo_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x7B);
}
/* JS rel8 */
void rtasm_js_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x78);
}
/* JZ rel8 */
void rtasm_jz_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0x74);
}
/* JA rel16/32 */
void rtasm_ja_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x87);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JAE rel16/32 */
void rtasm_jae_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x83);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JB rel16/32 */
void rtasm_jb_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x82);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JBE rel16/32 */
void rtasm_jbe_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x86);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JC rel16/32 */
void rtasm_jc_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x82);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JE rel16/32 */
void rtasm_je_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x84);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JG rel16/32 */
void rtasm_jg_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x8F);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JGE rel16/32 */
void rtasm_jge_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x8D);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JL rel16/32 */
void rtasm_jl_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x8C);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JLE rel16/32 */
void rtasm_jle_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x8E);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JNA rel16/32 */
void rtasm_jna_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x86);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JNAE rel16/32 */
void rtasm_jnae_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x82);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JNB rel16/32 */
void rtasm_jnb_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x83);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JNBE rel16/32 */
void rtasm_jnbe_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x87);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JNC rel16/32 */
void rtasm_jnc_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x83);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JNE rel16/32 */
void rtasm_jne_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x85);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JNG rel16/32 */
void rtasm_jng_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x8E);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JNGE rel16/32 */
void rtasm_jnge_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x8C);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JNL rel16/32 */
void rtasm_jnl_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x8D);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JNLE rel16/32 */
void rtasm_jnle_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x8F);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JNO rel16/32 */
void rtasm_jno_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x81);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JNP rel16/32 */
void rtasm_jnp_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x8B);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JNS rel16/32 */
void rtasm_jns_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x89);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JNZ rel16/32 */
void rtasm_jnz_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x85);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JO rel16/32 */
void rtasm_jo_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x80);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JP rel16/32 */
void rtasm_jp_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x8A);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JPE rel16/32 */
void rtasm_jpe_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x8A);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JPO rel16/32 */
void rtasm_jpo_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x8B);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JS rel16/32 */
void rtasm_js_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x88);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JZ rel16/32 */
void rtasm_jz_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x84);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JMP rel8 */
void rtasm_jmp_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0xEB);
}
/* JMP rel16 */
void rtasm_jmp_rel16(nativeblockinfo* nat, uint4 offset)
{
	rtasm_putbyte(nat, 0xE9);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, offset>>8);
}
/* JMP rel32 */
void rtasm_jmp_rel32(nativeblockinfo* nat, uint5 offset)
{
	rtasm_putbyte(nat, 0xE9);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, (offset>>8) & 0xff);
	rtasm_putbyte(nat, (offset>>16) & 0xff);
	rtasm_putbyte(nat, offset>>24);
}
/* JMP r/m16 */
void rtasm_jmp_rm16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xFF);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* JMP r/m32 */
void rtasm_jmp_rm32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xFF);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* JMP ptr16:16 */
void rtasm_jmp_ptr16_16(nativeblockinfo* nat, uint4 segment, uint4 offset)
{
	offset = segment | (offset<<16);
	rtasm_putbyte(nat, 0xEA);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, (offset>>8) & 0xff);
	rtasm_putbyte(nat, (offset>>16) & 0xff);
	rtasm_putbyte(nat, offset>>24);
}
/* JMP ptr16:32 */
void rtasm_jmp_ptr16_32(nativeblockinfo* nat, uint4 segment, uint5 offset)
{
	rtasm_putbyte(nat, 0xEA);
	rtasm_putbyte(nat, segment & 0xff);
	rtasm_putbyte(nat, segment>>8);
	rtasm_putbyte(nat, offset & 0xff);
	rtasm_putbyte(nat, (offset>>8) & 0xff);
	rtasm_putbyte(nat, (offset>>16) & 0xff);
	rtasm_putbyte(nat, offset>>24);
}
/* JMP m16:16 */
void rtasm_jmp_m16_16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xFF);
	rtasm_putbyte(nat, mem->rm | 40 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* JMP m16:32 */
void rtasm_jmp_m16_32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xFF);
	rtasm_putbyte(nat, mem->rm | 40 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LAHF */
void rtasm_lahf(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x9F);
}
/* LAR r16,r/m16 */
void rtasm_lar_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x02);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LAR r32,r/m32 */
void rtasm_lar_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x02);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LDMXCSR m32 */
void rtasm_ldmxcsr_m32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xAE);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LDS r16,m16:16 */
void rtasm_lds_r16_m16_16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xC5);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LDS r32,m16:32 */
void rtasm_lds_r32_m16_32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xC5);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LSS r16,m16:16 */
void rtasm_lss_r16_m16_16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xB2);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LSS r32,m16:32 */
void rtasm_lss_r32_m16_32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xB2);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LES r16,m16:16 */
void rtasm_les_r16_m16_16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xC4);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LES r32,m16:32 */
void rtasm_les_r32_m16_32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xC4);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LFS r16,m16:16 */
void rtasm_lfs_r16_m16_16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xB4);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LFS r32,m16:32 */
void rtasm_lfs_r32_m16_32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xB4);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LGS r16,m16:16 */
void rtasm_lgs_r16_m16_16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xB5);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LGS r32,m16:32 */
void rtasm_lgs_r32_m16_32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xB5);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LEA r16,m */
void rtasm_lea_r16_m(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x8D);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LEA r32,m */
void rtasm_lea_r32_m(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x8D);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LEAVE */
void rtasm_leave(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xC9);
}
/* LGDT m16&32 */
void rtasm_lgdt_m16n32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x01);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LIDT m16&32 */
void rtasm_lidt_m16n32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x01);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LLDT r/m16 */
void rtasm_lldt_rm16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x00);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LMSW r/m16 */
void rtasm_lmsw_rm16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x01);
	rtasm_putbyte(nat, mem->rm | 48 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LOCK */
void rtasm_lock(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xF0);
}
/* LODS m8 */
void rtasm_lods_m8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xAC);
}
/* LODS m16 */
void rtasm_lods_m16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xAD);
}
/* LODS m32 */
void rtasm_lods_m32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xAD);
}
/* LODSB */
void rtasm_lodsb(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xAC);
}
/* LODSW */
void rtasm_lodsw(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xAD);
}
/* LODSD */
void rtasm_lodsd(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xAD);
}
/* LOOP rel8 */
void rtasm_loop_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0xE2);
}
/* LOOPE rel8 */
void rtasm_loope_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0xE1);
}
/* LOOPZ rel8 */
void rtasm_loopz_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0xE1);
}
/* LOOPNE rel8 */
void rtasm_loopne_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0xE0);
}
/* LOOPNZ rel8 */
void rtasm_loopnz_rel8(nativeblockinfo* nat, uint3 offset)
{
	rtasm_putbyte(nat, 0xE0);
}
/* LSL r16,r/m16 */
void rtasm_lsl_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x03);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LSL r32,r/m32 */
void rtasm_lsl_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x03);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* LTR r/m16 */
void rtasm_ltr_rm16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x00);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOV r/m8,r8 */
void rtasm_mov_rm8_r8(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x88);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOV r/m16,r16 */
void rtasm_mov_rm16_r16(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x89);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOV r/m32,r32 */
void rtasm_mov_rm32_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x89);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOV r8,r/m8 */
void rtasm_mov_r8_rm8(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x8A);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOV r16,r/m16 */
void rtasm_mov_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x8B);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOV r32,r/m32 */
void rtasm_mov_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x8B);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOV r/m16,Sreg */
void rtasm_mov_rm16_Sreg(nativeblockinfo* nat, rtasm_mtype* mem, uint5 reg)
{
	rtasm_putbyte(nat, 0x8C);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOV Sreg,r/m16 */
void rtasm_mov_Sreg_rm16(nativeblockinfo* nat, uint5 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x8E);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOV AL,moffs8 */
void rtasm_mov_al_moffs8(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xA0);
}
/* MOV AX,moffs16 */
void rtasm_mov_ax_moffs16(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xA1);
}
/* MOV EAX,moffs32 */
void rtasm_mov_eax_moffs32(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xA1);
}
/* MOV moffs8,AL */
void rtasm_mov_moffs8_al(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xA2);
}
/* MOV moffs16,AX */
void rtasm_mov_moffs16_ax(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xA3);
}
/* MOV moffs32,EAX */
void rtasm_mov_moffs32_eax(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xA3);
}
/* MOV CR0,r32 */
void rtasm_mov_cr0_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x22);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOV CR2,r32 */
void rtasm_mov_cr2_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x22);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOV CR3,r32 */
void rtasm_mov_cr3_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x22);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOV CR4,r32 */
void rtasm_mov_cr4_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x22);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOV r32,CR0 */
void rtasm_mov_r32_cr0(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x20);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOV r32,CR2 */
void rtasm_mov_r32_cr2(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x20);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOV r32,CR3 */
void rtasm_mov_r32_cr3(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x20);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOV r32,CR4 */
void rtasm_mov_r32_cr4(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x20);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOV r32,DR0-DR7 */
void rtasm_mov_r32_dbg(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x21);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOV DR0-DR7,r32 */
void rtasm_mov_dbg_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x23);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOVSB */
void rtasm_movsb(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xA4);
}
/* MOVSW */
void rtasm_movsw(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xA5);
}
/* MOVSD */
void rtasm_movsd(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xA5);
}
/* MOVSX r16,r/m8 */
void rtasm_movsx_r16_rm8(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xBE);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOVSX r32,r/m8 */
void rtasm_movsx_r32_rm8(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xBE);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOVSX r32,r/m16 */
void rtasm_movsx_r32_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xBF);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOVZX r16,r/m8 */
void rtasm_movzx_r16_rm8(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xB6);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOVZX r32,r/m8 */
void rtasm_movzx_r32_rm8(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xB6);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MOVZX r32,r/m16 */
void rtasm_movzx_r32_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xB7);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MUL r/m8 */
void rtasm_mul_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xF6);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MUL r/m16 */
void rtasm_mul_rm16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xF7);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* MUL r/m32 */
void rtasm_mul_rm32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xF7);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* NEG r/m8 */
void rtasm_neg_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xF6);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* NEG r/m16 */
void rtasm_neg_rm16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xF7);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* NEG r/m32 */
void rtasm_neg_rm32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xF7);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* NOP */
void rtasm_nop(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x90);
}
/* NOT r/m8 */
void rtasm_not_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xF6);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* NOT r/m16 */
void rtasm_not_rm16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xF7);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* NOT r/m32 */
void rtasm_not_rm32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xF7);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* OR r/m8,r8 */
void rtasm_or_rm8_r8(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x08);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* OR r/m16,r16 */
void rtasm_or_rm16_r16(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x09);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* OR r/m32,r32 */
void rtasm_or_rm32_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x09);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* OR r8,r/m8 */
void rtasm_or_r8_rm8(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0A);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* OR r16,r/m16 */
void rtasm_or_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0B);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* OR r32,r/m32 */
void rtasm_or_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0B);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* OUT DX,AL */
void rtasm_out_dx_al(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xEE);
}
/* OUT DX,AX */
void rtasm_out_dx_ax(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xEF);
}
/* OUT DX,EAX */
void rtasm_out_dx_eax(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xEF);
}
/* OUTS DX,m8 */
void rtasm_outs_dx_m8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x6E);
}
/* OUTS DX,m16 */
void rtasm_outs_dx_m16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x6F);
}
/* OUTS DX,m32 */
void rtasm_outs_dx_m32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x6F);
}
/* OUTSB */
void rtasm_outsb(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x6E);
}
/* OUTSW */
void rtasm_outsw(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x6F);
}
/* OUTSD */
void rtasm_outsd(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x6F);
}
/* POP m16 */
void rtasm_pop_m16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x8F);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* POP m32 */
void rtasm_pop_m32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x8F);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* POP r16 */
void rtasm_pop_r16(nativeblockinfo* nat, uint3 reg)
{
}
/* POP r32 */
void rtasm_pop_r32(nativeblockinfo* nat, uint3 reg)
{
}
/* POP DS */
void rtasm_pop_ds(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x1F);
}
/* POP ES */
void rtasm_pop_es(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x07);
}
/* POP SS */
void rtasm_pop_ss(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x17);
}
/* POP FS */
void rtasm_pop_fs(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xA1);
}
/* POP GS */
void rtasm_pop_gs(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xA9);
}
/* POPA */
void rtasm_popa(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x61);
}
/* POPAD */
void rtasm_popad(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x61);
}
/* POPF */
void rtasm_popf(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x9D);
}
/* POPFD */
void rtasm_popfd(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x9D);
}
/* PREFETCHT0 m8 */
void rtasm_prefetcht0_m8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x18);
	rtasm_putbyte(nat, mem->rm | 8 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* PREFETCHT1 m8 */
void rtasm_prefetcht1_m8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x18);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* PREFETCHT2 m8 */
void rtasm_prefetcht2_m8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x18);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* PREFETCHNTA m8 */
void rtasm_prefetchnta_m8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x18);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* PUSH r/m16 */
void rtasm_push_rm16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xFF);
	rtasm_putbyte(nat, mem->rm | 48 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* PUSH r/m32 */
void rtasm_push_rm32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xFF);
	rtasm_putbyte(nat, mem->rm | 48 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* PUSH r16 */
void rtasm_push_r16(nativeblockinfo* nat, uint3 reg)
{
	rtasm_putbyte(nat, 0x50+reg);
}
/* PUSH r32 */
void rtasm_push_r32(nativeblockinfo* nat, uint3 reg)
{
	rtasm_putbyte(nat, 0x50+reg);
}
/* PUSH CS */
void rtasm_push_cs(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x0E);
}
/* PUSH SS */
void rtasm_push_ss(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x16);
}
/* PUSH DS */
void rtasm_push_ds(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x1E);
}
/* PUSH ES */
void rtasm_push_es(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x06);
}
/* PUSH FS */
void rtasm_push_fs(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xA0);
}
/* PUSH GS */
void rtasm_push_gs(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xA8);
}
/* PUSHA */
void rtasm_pusha(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x60);
}
/* PUSHAD */
void rtasm_pushad(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x60);
}
/* PUSHF */
void rtasm_pushf(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x9C);
}
/* PUSHFD */
void rtasm_pushfd(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x9C);
}
/* RCL r/m8,1 */
void rtasm_rcl_rm8_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD0);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* RCL r/m8,CL */
void rtasm_rcl_rm8_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD2);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* RCL r/m16,1 */
void rtasm_rcl_rm16_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD1);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* RCL r/m16,CL */
void rtasm_rcl_rm16_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD3);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* RCL r/m32,1 */
void rtasm_rcl_rm32_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD1);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* RCL r/m32,CL */
void rtasm_rcl_rm32_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD3);
	rtasm_putbyte(nat, mem->rm | 16 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* RCR r/m8,1 */
void rtasm_rcr_rm8_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD0);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* RCR r/m8,CL */
void rtasm_rcr_rm8_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD2);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* RCR r/m16,1 */
void rtasm_rcr_rm16_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD1);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* RCR r/m16,CL */
void rtasm_rcr_rm16_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD3);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* RCR r/m32,1 */
void rtasm_rcr_rm32_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD1);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* RCR r/m32,CL */
void rtasm_rcr_rm32_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD3);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ROL r/m8,1 */
void rtasm_rol_rm8_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD0);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ROL r/m8,CL */
void rtasm_rol_rm8_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD2);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ROL r/m16,1 */
void rtasm_rol_rm16_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD1);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ROL r/m16,CL */
void rtasm_rol_rm16_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD3);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ROL r/m32,1 */
void rtasm_rol_rm32_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD1);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ROL r/m32,CL */
void rtasm_rol_rm32_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD3);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ROR r/m8,1 */
void rtasm_ror_rm8_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD0);
	rtasm_putbyte(nat, mem->rm | 8 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ROR r/m8,CL */
void rtasm_ror_rm8_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD2);
	rtasm_putbyte(nat, mem->rm | 8 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ROR r/m16,1 */
void rtasm_ror_rm16_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD1);
	rtasm_putbyte(nat, mem->rm | 8 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ROR r/m16,CL */
void rtasm_ror_rm16_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD3);
	rtasm_putbyte(nat, mem->rm | 8 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ROR r/m32,1 */
void rtasm_ror_rm32_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD1);
	rtasm_putbyte(nat, mem->rm | 8 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* ROR r/m32,CL */
void rtasm_ror_rm32_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD3);
	rtasm_putbyte(nat, mem->rm | 8 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* RDMSR */
void rtasm_rdmsr(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x32);
}
/* RDPMC */
void rtasm_rdpmc(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x33);
}
/* RDTSC */
void rtasm_rdtsc(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x31);
}
/* RET */
void rtasm_ret(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xC3);
}
/* RETF */
void rtasm_retf(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xCB);
}
/* RSM */
void rtasm_rsm(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xAA);
}
/* SAL r/m8,1 */
void rtasm_sal_rm8_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD0);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SAL r/m8,CL */
void rtasm_sal_rm8_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD2);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SAL r/m16,1 */
void rtasm_sal_rm16_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD1);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SAL r/m16,CL */
void rtasm_sal_rm16_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD3);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SAL r/m32,1 */
void rtasm_sal_rm32_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD1);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SAL r/m32,CL */
void rtasm_sal_rm32_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD3);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SAR r/m8,1 */
void rtasm_sar_rm8_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD0);
	rtasm_putbyte(nat, mem->rm | 56 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SAR r/m8,CL */
void rtasm_sar_rm8_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD2);
	rtasm_putbyte(nat, mem->rm | 56 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SAR r/m16,1 */
void rtasm_sar_rm16_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD1);
	rtasm_putbyte(nat, mem->rm | 56 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SAR r/m16,CL */
void rtasm_sar_rm16_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD3);
	rtasm_putbyte(nat, mem->rm | 56 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SAR r/m32,1 */
void rtasm_sar_rm32_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD1);
	rtasm_putbyte(nat, mem->rm | 56 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SAR r/m32,CL */
void rtasm_sar_rm32_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD3);
	rtasm_putbyte(nat, mem->rm | 56 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SHL r/m8,1 */
void rtasm_shl_rm8_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD0);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SHL r/m8,CL */
void rtasm_shl_rm8_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD2);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SHL r/m16,1 */
void rtasm_shl_rm16_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD1);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SHL r/m16,CL */
void rtasm_shl_rm16_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD3);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SHL r/m32,1 */
void rtasm_shl_rm32_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD1);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SHL r/m32,CL */
void rtasm_shl_rm32_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD3);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SHR r/m8,1 */
void rtasm_shr_rm8_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD0);
	rtasm_putbyte(nat, mem->rm | 40 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SHR r/m8,CL */
void rtasm_shr_rm8_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD2);
	rtasm_putbyte(nat, mem->rm | 40 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SHR r/m16,1 */
void rtasm_shr_rm16_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD1);
	rtasm_putbyte(nat, mem->rm | 40 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SHR r/m16,CL */
void rtasm_shr_rm16_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD3);
	rtasm_putbyte(nat, mem->rm | 40 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SHR r/m32,1 */
void rtasm_shr_rm32_1(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD1);
	rtasm_putbyte(nat, mem->rm | 40 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SHR r/m32,CL */
void rtasm_shr_rm32_cl(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD3);
	rtasm_putbyte(nat, mem->rm | 40 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SBB r/m8,r8 */
void rtasm_sbb_rm8_r8(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x18);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SBB r/m16,r16 */
void rtasm_sbb_rm16_r16(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x19);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SBB r/m32,r32 */
void rtasm_sbb_rm32_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x19);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SBB r8,r/m8 */
void rtasm_sbb_r8_rm8(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x1A);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SBB r16,r/m16 */
void rtasm_sbb_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x1B);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SBB r32,r/m32 */
void rtasm_sbb_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x1B);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SCAS m8 */
void rtasm_scas_m8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xAE);
}
/* SCAS m16 */
void rtasm_scas_m16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xAF);
}
/* SCAS m32 */
void rtasm_scas_m32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xAF);
}
/* SCASB */
void rtasm_scasb(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xAE);
}
/* SCASW */
void rtasm_scasw(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xAF);
}
/* SCASD */
void rtasm_scasd(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xAF);
}
/* SETA r/m8 */
void rtasm_seta_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x97);
}
/* SETAE r/m8 */
void rtasm_setae_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x93);
}
/* SETB r/m8 */
void rtasm_setb_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x92);
}
/* SETBE r/m8 */
void rtasm_setbe_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x96);
}
/* SETC r/m8 */
void rtasm_setc_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x92);
}
/* SETE r/m8 */
void rtasm_sete_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x94);
}
/* SETG r/m8 */
void rtasm_setg_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x9F);
}
/* SETGE r/m8 */
void rtasm_setge_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x9D);
}
/* SETL r/m8 */
void rtasm_setl_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x9C);
}
/* SETLE r/m8 */
void rtasm_setle_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x9E);
}
/* SETNA r/m8 */
void rtasm_setna_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x96);
}
/* SETNAE r/m8 */
void rtasm_setnae_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x92);
}
/* SETNB r/m8 */
void rtasm_setnb_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x93);
}
/* SETNBE r/m8 */
void rtasm_setnbe_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x97);
}
/* SETNC r/m8 */
void rtasm_setnc_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x93);
}
/* SETNE r/m8 */
void rtasm_setne_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x95);
}
/* SETNG r/m8 */
void rtasm_setng_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x9E);
}
/* SETNGE r/m8 */
void rtasm_setnge_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x9C);
}
/* SETNL r/m8 */
void rtasm_setnl_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x9D);
}
/* SETNLE r/m8 */
void rtasm_setnle_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x9F);
}
/* SETNO r/m8 */
void rtasm_setno_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x91);
}
/* SETNP r/m8 */
void rtasm_setnp_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x9B);
}
/* SETNS r/m8 */
void rtasm_setns_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x99);
}
/* SETNZ r/m8 */
void rtasm_setnz_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x95);
}
/* SETO r/m8 */
void rtasm_seto_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x90);
}
/* SETP r/m8 */
void rtasm_setp_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x9A);
}
/* SETPE r/m8 */
void rtasm_setpe_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x9A);
}
/* SETPO r/m8 */
void rtasm_setpo_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x9B);
}
/* SETS r/m8 */
void rtasm_sets_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x98);
}
/* SETZ r/m8 */
void rtasm_setz_rm8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x94);
}
/* SFENCE          */
void rtasm_sfence(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xAE);
	rtasm_putbyte(nat, 0xF8);
}
/* SGDT m */
void rtasm_sgdt_m(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x01);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SIDT m */
void rtasm_sidt_m(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x01);
	rtasm_putbyte(nat, mem->rm | 8 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SHLD r/m16,r16,CL */
void rtasm_shld_rm16_r16_cl(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xA5);
}
/* SHLD r/m32,r32,CL */
void rtasm_shld_rm32_r32_cl(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xA5);
}
/* SHRD r/m16,r16,CL */
void rtasm_shrd_rm16_r16_cl(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xAD);
}
/* SHRD r/m32,r32,CL */
void rtasm_shrd_rm32_r32_cl(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xAD);
}
/* SLDT r/m16 */
void rtasm_sldt_rm16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x00);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SLDT r/m32 */
void rtasm_sldt_rm32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x00);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SMSW r/m16 */
void rtasm_smsw_rm16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x01);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SMSW r32/m16 */
void rtasm_smsw_r32m16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x01);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* STC */
void rtasm_stc(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xF9);
}
/* STD */
void rtasm_std(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xFD);
}
/* STI */
void rtasm_sti(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xFB);
}
/* STMXCSR m32 */
void rtasm_stmxcsr_m32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xAE);
	rtasm_putbyte(nat, mem->rm | 24 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* STOS m8 */
void rtasm_stos_m8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xAA);
}
/* STOS m16 */
void rtasm_stos_m16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xAB);
}
/* STOS m32 */
void rtasm_stos_m32(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xAB);
}
/* STOSB */
void rtasm_stosb(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xAA);
}
/* STOSW */
void rtasm_stosw(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xAB);
}
/* STOSD */
void rtasm_stosd(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0xAB);
}
/* STR r/m16 */
void rtasm_str_rm16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x00);
	rtasm_putbyte(nat, mem->rm | 8 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SUB r/m8,r8 */
void rtasm_sub_rm8_r8(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x28);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SUB r/m16,r16 */
void rtasm_sub_rm16_r16(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x29);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SUB r/m32,r32 */
void rtasm_sub_rm32_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x29);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SUB r8,r/m8 */
void rtasm_sub_r8_rm8(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x2A);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SUB r16,r/m16 */
void rtasm_sub_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x2B);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SUB r32,r/m32 */
void rtasm_sub_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x2B);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* SYSENTER */
void rtasm_sysenter(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x34);
}
/* SYSEXIT */
void rtasm_sysexit(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x35);
}
/* TEST r/m8,r8 */
void rtasm_test_rm8_r8(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x84);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* TEST r/m16,r16 */
void rtasm_test_rm16_r16(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x85);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* TEST r/m32,r32 */
void rtasm_test_rm32_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x85);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* UD2 */
void rtasm_ud2(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x0B);
}
/* VERR r/m16 */
void rtasm_verr_rm16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x00);
	rtasm_putbyte(nat, mem->rm | 32 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* VERW r/m16 */
void rtasm_verw_rm16(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x00);
	rtasm_putbyte(nat, mem->rm | 40 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* WAIT */
void rtasm_wait(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x9B);
}
/* FWAIT */
void rtasm_fwait(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x9B);
}
/* WBINVD */
void rtasm_wbinvd(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x09);
}
/* WRMSR */
void rtasm_wrmsr(nativeblockinfo* nat)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0x30);
}
/* XADD r/m8,r8 */
void rtasm_xadd_rm8_r8(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xC0);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* XADD r/m16,r16 */
void rtasm_xadd_rm16_r16(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xC1);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* XADD r/m32,r32 */
void rtasm_xadd_rm32_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x0F);
	rtasm_putbyte(nat, 0xC1);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* XCHG AX,r16 */
void rtasm_xchg_ax_r16(nativeblockinfo* nat, uint3 reg)
{
	rtasm_putbyte(nat, 0x90+reg);
}
/* XCHG r16,AX */
void rtasm_xchg_r16_ax(nativeblockinfo* nat, uint3 reg)
{
	rtasm_putbyte(nat, 0x90+reg);
}
/* XCHG EAX,r32 */
void rtasm_xchg_eax_r32(nativeblockinfo* nat, uint3 reg)
{
	rtasm_putbyte(nat, 0x90+reg);
}
/* XCHG r32,EAX */
void rtasm_xchg_r32_eax(nativeblockinfo* nat, uint3 reg)
{
	rtasm_putbyte(nat, 0x90+reg);
}
/* XCHG r/m8,r8 */
void rtasm_xchg_rm8_r8(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x86);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* XCHG r8,r/m8 */
void rtasm_xchg_r8_rm8(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x86);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* XCHG r/m16,r16 */
void rtasm_xchg_rm16_r16(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x87);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* XCHG r16,r/m16 */
void rtasm_xchg_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x87);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* XCHG r/m32,r32 */
void rtasm_xchg_rm32_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x87);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* XCHG r32,r/m32 */
void rtasm_xchg_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x87);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* XLAT m8 */
void rtasm_xlat_m8(nativeblockinfo* nat, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0xD7);
	rtasm_putbyte(nat, mem->rm | 0 | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* XOR r/m8,r8 */
void rtasm_xor_rm8_r8(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x30);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* XOR r/m16,r16 */
void rtasm_xor_rm16_r16(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x31);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* XOR r/m32,r32 */
void rtasm_xor_rm32_r32(nativeblockinfo* nat, rtasm_mtype* mem, uint3 reg)
{
	rtasm_putbyte(nat, 0x31);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* XOR r8,r/m8 */
void rtasm_xor_r8_rm8(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x32);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* XOR r16,r/m16 */
void rtasm_xor_r16_rm16(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x33);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
/* XOR r32,r/m32 */
void rtasm_xor_r32_rm32(nativeblockinfo* nat, uint3 reg, rtasm_mtype* mem)
{
	rtasm_putbyte(nat, 0x33);
	rtasm_putbyte(nat, mem->rm | (reg<<3) | (mem->mod<<6));
	rtasm_mem(nat, mem);
}
