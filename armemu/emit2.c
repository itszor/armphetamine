#include "defs.h"
#ifndef E2HEADER
#include "emit2.h"
#endif

p86_table p86tab[] = {
  /*p86_ADC*/    {CFLAG|VFLAG|NFLAG|ZFLAG, 0,
                   {0, p86_adc_ri, p86_adc_mi, 0,
                    0, p86_adc_rr, p86_adc_mr, 0,
                    0, p86_adc_rm, 0,          0,
                    0, 0,          0,          0}},
  /*p86_ADD*/    {CFLAG|VFLAG|NFLAG|ZFLAG, 0,
                   {0, p86_add_ri, p86_add_mi, 0,
                    0, p86_add_rr, p86_add_mr, 0,
                    0, p86_add_rm, 0,          0,
                    0, 0,          0,          0}},
  /*p86_AND*/    {NFLAG|ZFLAG, CFLAG|VFLAG,
                   {0, p86_and_ri, p86_and_mi, 0,
                    0, p86_and_rr, p86_and_mr, 0,
                    0, p86_and_rm, 0,          0,
                    0, 0,          0,          0}},
  /*p86_BT*/     {CFLAG, VFLAG|NFLAG|ZFLAG,
                   {0, p86_bt_ri, p86_bt_mi, 0,
                    0, p86_bt_rr, 0,         0,
                    0, p86_bt_rm, 0,         0,
                    0, 0,         0,         0}},
  /*p86_CALL*/   {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_call_xr,
                    0, 0, 0, p86_call_xm,
                    0, 0, 0, 0}},
  /*p86_CLC*/    {CFLAG, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, p86_clc_xx}},
  /*p86_CMC*/    {CFLAG, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, p86_cmc_xx}},
  /*p86_CMP*/    {ICFLAG|VFLAG|NFLAG|ZFLAG, 0,
                   {0, p86_cmp_ri, p86_cmp_mi, 0,
                    0, p86_cmp_rr, p86_cmp_mr, 0,
                    0, p86_cmp_rm, 0,          0,
                    0, 0,          0,          0}},
  /*p86_DEC*/    {VFLAG|NFLAG|ZFLAG, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_dec_xr,
                    0, 0, 0, p86_dec_xm,
                    0, 0, 0, 0}},
  /*p86_IMUL*/   {0, CFLAG|VFLAG|NFLAG|ZFLAG,
                   {0, 0,           0, 0,
                    0, p86_imul_rr, 0, 0,
                    0, p86_imul_rm, 0, 0,
                    0, 0,           0, 0}},
  /*p86_INC*/    {VFLAG|NFLAG|ZFLAG, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_inc_xr,
                    0, 0, 0, p86_inc_xm,
                    0, 0, 0, 0}},
  /*p86_JA*/     {0, 0,
                   {0, 0, 0, p86_ja_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JAE*/    {0, 0,
                   {0, 0, 0, p86_jae_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JB*/     {0, 0,
                   {0, 0, 0, p86_jb_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JBE*/    {0, 0,
                   {0, 0, 0, p86_jbe_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JC*/     {0, 0,
                   {0, 0, 0, p86_jc_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JE*/     {0, 0,
                   {0, 0, 0, p86_je_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JG*/     {0, 0,
                   {0, 0, 0, p86_jg_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JGE*/    {0, 0,
                   {0, 0, 0, p86_jge_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JL*/     {0, 0,
                   {0, 0, 0, p86_jl_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JLE*/    {0, 0,
                   {0, 0, 0, p86_jle_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JNA*/    {0, 0,
                   {0, 0, 0, p86_jna_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JNB*/    {0, 0,
                   {0, 0, 0, p86_jnb_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JNBE*/   {0, 0,
                   {0, 0, 0, p86_jnbe_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JNC*/    {0, 0,
                   {0, 0, 0, p86_jnc_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JNE*/    {0, 0,
                   {0, 0, 0, p86_jne_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JNG*/    {0, 0,
                   {0, 0, 0, p86_jng_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JNGE*/   {0, 0,
                   {0, 0, 0, p86_jnge_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JNL*/    {0, 0,
                   {0, 0, 0, p86_jnl_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JNLE*/   {0, 0,
                   {0, 0, 0, p86_jnle_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JNO*/    {0, 0,
                   {0, 0, 0, p86_jno_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JNP*/    {0, 0,
                   {0, 0, 0, p86_jnp_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JNS*/    {0, 0,
                   {0, 0, 0, p86_jns_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JNZ*/    {0, 0,
                   {0, 0, 0, p86_jnz_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JO*/     {0, 0,
                   {0, 0, 0, p86_jo_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JP*/     {0, 0,
                   {0, 0, 0, p86_jp_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JPE*/    {0, 0,
                   {0, 0, 0, p86_jpe_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JPO*/    {0, 0,
                   {0, 0, 0, p86_jpo_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JS*/     {0, 0,
                   {0, 0, 0, p86_js_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JZ*/     {0, 0,
                   {0, 0, 0, p86_jz_xi,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0}},
  /*p86_JMP*/    {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_jmp_xr,
                    0, 0, 0, p86_jmp_xm,
                    0, 0, 0, 0}},
  /*p86_MOV*/    {0, 0,
                   {0, p86_mov_ri, p86_mov_mi, 0,
                    0, p86_mov_rr, p86_mov_mr, 0,
                    0, p86_mov_rm, 0,          0,
                    0, 0,          0,          0}},
  /*p86_NEG*/    {CFLAG|VFLAG|NFLAG|ZFLAG, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_neg_xr,
                    0, 0, 0, p86_neg_xm,
                    0, 0, 0, 0}},
  /*p86_NOP*/    {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, p86_nop_xx}},
  /*p86_NOT*/    {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_not_xr,
                    0, 0, 0, p86_not_xm,
                    0, 0, 0, 0}},
  /*p86_OR*/     {NFLAG|ZFLAG, CFLAG|VFLAG,
                   {0, p86_or_ri, p86_or_mi, 0,
                    0, p86_or_rr, p86_or_mr, 0,
                    0, p86_or_rm, 0,         0,
                    0, 0,         0,         0}},
  /*p86_POP*/    {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_pop_xr,
                    0, 0, 0, p86_pop_xm,
                    0, 0, 0, 0}},
  /*p86_POPF*/   {CFLAG|VFLAG|NFLAG|ZFLAG, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, p86_popf_xx}},
  /*p86_PUSH*/   {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_push_xr,
                    0, 0, 0, p86_push_xm,
                    0, 0, 0, 0}},
  /*p86_PUSHF*/  {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, p86_pushf_xx}},
  /*p86_RCL*/    {CFLAG, VFLAG,
                   {0, p86_rcl_ri, 0, 0,
                    0, 0,          0, 0,
                    0, 0,          0, 0,
                    0, p86_rcl_rx, 0, 0}},
  /*p86_RCR*/    {CFLAG, VFLAG,
                   {0, p86_rcr_ri, 0, 0,
                    0, 0,          0, 0,
                    0, 0,          0, 0,
                    0, p86_rcr_rx, 0, 0}},
  /*p86_RET*/    {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, p86_ret_xx}},
  /*p86_ROL*/    {CFLAG, VFLAG,
                   {0, p86_rol_ri, 0, 0,
                    0, 0,          0, 0,
                    0, 0,          0, 0,
                    0, p86_rol_rx, 0, 0}},
  /*p86_ROR*/    {CFLAG, VFLAG,
                   {0, p86_ror_ri, 0, 0,
                    0, 0,          0, 0,
                    0, 0,          0, 0,
                    0, p86_ror_rx, 0, 0}},
  /*p86_SHL*/    {CFLAG, VFLAG|NFLAG|ZFLAG,
                   {0, p86_shl_ri, 0, 0,
                    0, 0,          0, 0,
                    0, 0,          0, 0,
                    0, p86_shl_rx, 0, 0}},
  /*p86_SAR*/    {CFLAG, VFLAG|NFLAG|ZFLAG,
                   {0, p86_sar_ri, 0, 0,
                    0, 0,          0, 0,
                    0, 0,          0, 0,
                    0, p86_sar_rx, 0, 0}},
  /*p86_SBB*/    {ICFLAG|VFLAG|NFLAG|ZFLAG, 0,
                   {0, p86_sbb_ri, p86_sbb_mi, 0,
                    0, p86_sbb_rr, p86_sbb_mr, 0,
                    0, p86_sbb_rm, 0,          0,
                    0, 0,         0,           0}},
  /*p86_SETA*/   {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_seta_xr,
                    0, 0, 0, p86_seta_xm,
                    0, 0, 0, 0}},
  /*p86_SETAE*/  {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setae_xr,
                    0, 0, 0, p86_setae_xm,
                    0, 0, 0, 0}},
  /*p86_SETB*/   {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setb_xr,
                    0, 0, 0, p86_setb_xm,
                    0, 0, 0, 0}},
  /*p86_SETBE*/  {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setbe_xr,
                    0, 0, 0, p86_setbe_xm,
                    0, 0, 0, 0}},
  /*p86_SETC*/   {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setc_xr,
                    0, 0, 0, p86_setc_xm,
                    0, 0, 0, 0}},
  /*p86_SETE*/   {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_sete_xr,
                    0, 0, 0, p86_sete_xm,
                    0, 0, 0, 0}},
  /*p86_SETG*/   {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setg_xr,
                    0, 0, 0, p86_setg_xm,
                    0, 0, 0, 0}},
  /*p86_SETGE*/  {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setge_xr,
                    0, 0, 0, p86_setge_xm,
                    0, 0, 0, 0}},
  /*p86_SETL*/   {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setl_xr,
                    0, 0, 0, p86_setl_xm,
                    0, 0, 0, 0}},
  /*p86_SETLE*/  {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setle_xr,
                    0, 0, 0, p86_setle_xm,
                    0, 0, 0, 0}},
  /*p86_SETNA*/  {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setna_xr,
                    0, 0, 0, p86_setna_xm,
                    0, 0, 0, 0}},
  /*p86_SETNB*/  {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setnb_xr,
                    0, 0, 0, p86_setnb_xm,
                    0, 0, 0, 0}},
  /*p86_SETNBE*/ {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setnbe_xr,
                    0, 0, 0, p86_setnbe_xm,
                    0, 0, 0, 0}},
  /*p86_SETNC*/  {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setnc_xr,
                    0, 0, 0, p86_setnc_xm,
                    0, 0, 0, 0}},
  /*p86_SETNE*/  {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setne_xr,
                    0, 0, 0, p86_setne_xm,
                    0, 0, 0, 0}},
  /*p86_SETNG*/  {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setng_xr,
                    0, 0, 0, p86_setng_xm,
                    0, 0, 0, 0}},
  /*p86_SETNGE*/ {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setnge_xr,
                    0, 0, 0, p86_setnge_xm,
                    0, 0, 0, 0}},
  /*p86_SETNL*/  {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setnl_xr,
                    0, 0, 0, p86_setnl_xm,
                    0, 0, 0, 0}},
  /*p86_SETNLE*/ {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setnle_xr,
                    0, 0, 0, p86_setnle_xm,
                    0, 0, 0, 0}},
  /*p86_SETNO*/  {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setno_xr,
                    0, 0, 0, p86_setno_xm,
                    0, 0, 0, 0}},
  /*p86_SETNP*/  {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setnp_xr,
                    0, 0, 0, p86_setnp_xm,
                    0, 0, 0, 0}},
  /*p86_SETNS*/  {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setns_xr,
                    0, 0, 0, p86_setns_xm,
                    0, 0, 0, 0}},
  /*p86_SETNZ*/  {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setnz_xr,
                    0, 0, 0, p86_setnz_xm,
                    0, 0, 0, 0}},
  /*p86_SETO*/   {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_seto_xr,
                    0, 0, 0, p86_seto_xm,
                    0, 0, 0, 0}},
  /*p86_SETP*/   {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setp_xr,
                    0, 0, 0, p86_setp_xm,
                    0, 0, 0, 0}},
  /*p86_SETPE*/  {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setpe_xr,
                    0, 0, 0, p86_setpe_xm,
                    0, 0, 0, 0}},
  /*p86_SETPO*/  {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setpo_xr,
                    0, 0, 0, p86_setpo_xm,
                    0, 0, 0, 0}},
  /*p86_SETS*/   {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_sets_xr,
                    0, 0, 0, p86_sets_xm,
                    0, 0, 0, 0}},
  /*p86_SETZ*/   {0, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, p86_setz_xr,
                    0, 0, 0, p86_setz_xm,
                    0, 0, 0, 0}},
  /*p86_SHR*/    {CFLAG, VFLAG|NFLAG|ZFLAG,
                   {0, p86_shr_ri, 0, 0,
                    0, 0,          0, 0,
                    0, 0,          0, 0,
                    0, p86_shr_rx, 0, 0}},
  /*p86_STC*/    {CFLAG, 0,
                   {0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, p86_stc_xx}},
  /*p86_SUB*/    {CFLAG|VFLAG|NFLAG|ZFLAG, 0,
                   {0, p86_sub_ri, p86_sub_mi, 0,
                    0, p86_sub_rr, p86_sub_mr, 0,
                    0, p86_sub_rm, 0,          0,
                    0, 0,         0,           0}},
  /*p86_TEST*/   {NFLAG|ZFLAG, CFLAG|VFLAG,
                   {0, p86_test_ri, p86_test_mi, 0,
                    0, p86_test_rr, p86_test_mr, 0,
                    0, p86_test_rm, 0,           0,
                    0, 0,           0,           0}},
  /*p86_XADD*/   {CFLAG|VFLAG|NFLAG|ZFLAG, 0,
                   {0, 0,           0,           0,
                    0, p86_xadd_rr, p86_xadd_mr, 0,
                    0, p86_xadd_rm, 0,           0,
                    0, 0,           0,           0}},
  /*p86_XCHG*/   {0, 0,
                   {0, 0,           0,           0,
                    0, p86_xchg_rr, p86_xchg_mr, 0,
                    0, p86_xchg_rm, 0,           0,
                    0, 0,           0,           0}},
  /*p86_XOR*/    {NFLAG|ZFLAG, CFLAG|VFLAG,
                   {0, p86_xor_ri, p86_xor_mi, 0,
                    0, p86_xor_rr, p86_xor_mr, 0,
                    0, p86_xor_rm, 0,          0,
                    0, 0,          0,          0}},
};

// Everything below is utterly hideous.

void p86_gen_ri(nativeblockinfo* nat, uint5 op, uint5 len, uint5 imm, uint5 rm)
{
  if (len!=p86l_long) exit(1);
  switch (op)
  {
    case p86_ADC:  x86asm_out(nat, ADClri,  0); return;
    case p86_ADD:  x86asm_out(nat, ADDlri,  0); return;
    case p86_AND:  x86asm_out(nat, ANDlri,  0); return;
    case p86_BT:   x86asm_out(nat, BTlri,   0); return;
    case p86_CMP:  x86asm_out(nat, CMPlri,  0); return;
    case p86_MOV:  x86asm_out(nat, MOVlri,  0); return;
    case p86_OR:   x86asm_out(nat, ORlri,   0); return;
    case p86_RCL:  x86asm_out(nat, RCLlri,  0); return;
    case p86_RCR:  x86asm_out(nat, RCRlri,  0); return;
    case p86_ROL:  x86asm_out(nat, ROLlri,  0); return;
    case p86_ROR:  x86asm_out(nat, RORlri,  0); return;
    case p86_SHL:  x86asm_out(nat, SHLlri,  0); return;
    case p86_SAR:  x86asm_out(nat, SARlri,  0); return;
    case p86_SBB:  x86asm_out(nat, SBBlri,  0); return;
    case p86_SHR:  x86asm_out(nat, SHRlri,  0); return;
    case p86_SUB:  x86asm_out(nat, SUBlri,  0); return;
    case p86_TEST: x86asm_out(nat, TESTlri, 0); return;
    case p86_XOR:  x86asm_out(nat, XORlri,  0); return;
  }
}

void p86_gen_rc(nativeblockinfo* nat, uint5 op, uint5 len, uint5 rm)
{
  if (len!=p86l_long) exit(1);
  switch (op)
  {
    case p86_RCL:  x86asm_out(nat, RCLlrc,  0); return;
    case p86_RCR:  x86asm_out(nat, RCRlrc,  0); return;
    case p86_ROL:  x86asm_out(nat, ROLlrc,  0); return;
    case p86_ROR:  x86asm_out(nat, RORlrc,  0); return;
    case p86_SHL:  x86asm_out(nat, SHLlrc,  0); return;
    case p86_SAR:  x86asm_out(nat, SARlrc,  0); return;
    case p86_SHR:  x86asm_out(nat, SHRlrc,  0); return;
  }
}

void p86_gen_rr(nativeblockinfo* nat, uint5 op, uint5 len, uint5 reg, uint5 rm)
{
  if (len!=p86l_long) exit(1);
  switch (op)
  {
    case p86_ADC:  x86asm_out(nat, ADClrr,  0); return;
    case p86_ADD:  x86asm_out(nat, ADDlrr,  0); return;
    case p86_AND:  x86asm_out(nat, ANDlrr,  0); return;
    case p86_BT:   x86asm_out(nat, BTlrr,   0); return;
    case p86_CMP:  x86asm_out(nat, CMPlrr,  0); return;
    case p86_IMUL: x86asm_out(nat, IMULlrr, 0); return;
    case p86_MOV:  x86asm_out(nat, MOVlrr,  0); return;
    case p86_OR:   x86asm_out(nat, ORlrr,   0); return;
    case p86_SBB:  x86asm_out(nat, SBBlrr,  0); return;
    case p86_SUB:  x86asm_out(nat, SUBlrr,  0); return;
    case p86_TEST: x86asm_out(nat, TESTlrr, 0); return;
    case p86_XADD: x86asm_out(nat, XADDlrr, 0); return;
    case p86_XCHG: x86asm_out(nat, XCHGlrr, 0); return;
    case p86_XOR:  x86asm_out(nat, XORlrr,  0); return;
  }
}

void p86_gen_xr(nativeblockinfo* nat, uint5 op, uint5 len, uint5 rm)
{
  switch (len)
  {
    case p86l_byte:
    switch (op)
    {
      case p86_SETA:   x86asm_out(nat, SETAr, 0);   return;
      case p86_SETAE:  x86asm_out(nat, SETAEr, 0);  return;
      case p86_SETB:   x86asm_out(nat, SETBr, 0);   return;
      case p86_SETBE:  x86asm_out(nat, SETBEr, 0);  return;
      case p86_SETC:   x86asm_out(nat, SETCr, 0);   return;
      case p86_SETE:   x86asm_out(nat, SETEr, 0);   return;
      case p86_SETG:   x86asm_out(nat, SETGr, 0);   return;
      case p86_SETGE:  x86asm_out(nat, SETGEr, 0);  return;
      case p86_SETL:   x86asm_out(nat, SETLr, 0);   return;
      case p86_SETLE:  x86asm_out(nat, SETLEr, 0);  return;
      case p86_SETNA:  x86asm_out(nat, SETNAr, 0);  return;
      case p86_SETNB:  x86asm_out(nat, SETNBr, 0);  return;
      case p86_SETNBE: x86asm_out(nat, SETNBEr, 0); return;
      case p86_SETNC:  x86asm_out(nat, SETNCr, 0);  return;
      case p86_SETNE:  x86asm_out(nat, SETNEr, 0);  return;
      case p86_SETNG:  x86asm_out(nat, SETNGr, 0);  return;
      case p86_SETNGE: x86asm_out(nat, SETNGEr, 0); return;
      case p86_SETNL:  x86asm_out(nat, SETNLr, 0);  return;
      case p86_SETNLE: x86asm_out(nat, SETNLEr, 0); return;
      case p86_SETNO:  x86asm_out(nat, SETNOr, 0);  return;
      case p86_SETNP:  x86asm_out(nat, SETNPr, 0);  return;
      case p86_SETNS:  x86asm_out(nat, SETNSr, 0);  return;
      case p86_SETNZ:  x86asm_out(nat, SETNZr, 0);  return;
      case p86_SETO:   x86asm_out(nat, SETOr, 0);   return;
      case p86_SETP:   x86asm_out(nat, SETPr, 0);   return;
      case p86_SETPE:  x86asm_out(nat, SETPEr, 0);  return;
      case p86_SETPO:  x86asm_out(nat, SETPOr, 0);  return;
      case p86_SETS:   x86asm_out(nat, SETSr, 0);   return;
      case p86_SETZ:   x86asm_out(nat, SETZr, 0);   return;
    }
    break;
    
    case p86l_long:
    {
      switch (op)
      {
        case p86_CALL:   x86asm_out(nat, CALLnar, 0); return;
        case p86_DEC:    x86asm_out(nat, DEClr, 0);   return;
        case p86_INC:    x86asm_out(nat, INClr, 0);   return;
        case p86_JMP:    x86asm_out(nat, JMPnar, 0);  return;
        case p86_NEG:    x86asm_out(nat, NEGlr, 0);   return;
        case p86_NOT:    x86asm_out(nat, NOTlr, 0);   return;
        case p86_POP:    x86asm_out(nat, POPlr, 0);   return;
        case p86_PUSH:   x86asm_out(nat, PUSHlr, 0);  return;
      }
    }
    break;
  }
}

void p86_gen_xx(nativeblockinfo* nat, uint5 op, uint5 len)
{
  switch (op)
  {
    case p86_CLC:   x86asm_out(nat, CLC, 0);   return;
    case p86_CMC:   x86asm_out(nat, CMC, 0);   return;
    case p86_NOP:   x86asm_out(nat, NOP, 0);   return;
    case p86_POPF:  x86asm_out(nat, POPF, 0);  return;
    case p86_PUSHF: x86asm_out(nat, PUSHF, 0); return;
    case p86_RET:   x86asm_out(nat, RET, 0);   return;
    case p86_STC:   x86asm_out(nat, STC, 0);   return;
  }
}

void p86_gen_xm(nativeblockinfo* nat, uint5 op, uint5 len, p86_mtype* dest)
{
  #define XMSWITCH \
  switch (op) \
  { \
    case p86_CALL:   x86asm_out(nat, CALLnam, 0); return; \
    case p86_DEC:    x86asm_out(nat, DEClm, 0);   return; \
    case p86_INC:    x86asm_out(nat, INClm, 0);   return; \
    case p86_JMP:    x86asm_out(nat, JMPnam, 0);  return; \
    case p86_NEG:    x86asm_out(nat, NEGlm, 0);   return; \
    case p86_NOT:    x86asm_out(nat, NOTlm, 0);   return; \
    case p86_POP:    x86asm_out(nat, POPlm, 0);   return; \
    case p86_PUSH:   x86asm_out(nat, PUSHlm, 0);  return; \
    case p86_SETA:   x86asm_out(nat, SETAm, 0);   return; \
    case p86_SETAE:  x86asm_out(nat, SETAEm, 0);  return; \
    case p86_SETB:   x86asm_out(nat, SETBm, 0);   return; \
    case p86_SETBE:  x86asm_out(nat, SETBEm, 0);  return; \
    case p86_SETC:   x86asm_out(nat, SETCm, 0);   return; \
    case p86_SETE:   x86asm_out(nat, SETEm, 0);   return; \
    case p86_SETG:   x86asm_out(nat, SETGm, 0);   return; \
    case p86_SETGE:  x86asm_out(nat, SETGEm, 0);  return; \
    case p86_SETL:   x86asm_out(nat, SETLm, 0);   return; \
    case p86_SETLE:  x86asm_out(nat, SETLEm, 0);  return; \
    case p86_SETNA:  x86asm_out(nat, SETNAm, 0);  return; \
    case p86_SETNB:  x86asm_out(nat, SETNBm, 0);  return; \
    case p86_SETNBE: x86asm_out(nat, SETNBEm, 0); return; \
    case p86_SETNC:  x86asm_out(nat, SETNCm, 0);  return; \
    case p86_SETNE:  x86asm_out(nat, SETNEm, 0);  return; \
    case p86_SETNG:  x86asm_out(nat, SETNGm, 0);  return; \
    case p86_SETNGE: x86asm_out(nat, SETNGEm, 0); return; \
    case p86_SETNL:  x86asm_out(nat, SETNLm, 0);  return; \
    case p86_SETNLE: x86asm_out(nat, SETNLEm, 0); return; \
    case p86_SETNO:  x86asm_out(nat, SETNOm, 0);  return; \
    case p86_SETNP:  x86asm_out(nat, SETNPm, 0);  return; \
    case p86_SETNS:  x86asm_out(nat, SETNSm, 0);  return; \
    case p86_SETNZ:  x86asm_out(nat, SETNZm, 0);  return; \
    case p86_SETO:   x86asm_out(nat, SETOm, 0);   return; \
    case p86_SETP:   x86asm_out(nat, SETPm, 0);   return; \
    case p86_SETPE:  x86asm_out(nat, SETPEm, 0);  return; \
    case p86_SETPO:  x86asm_out(nat, SETPOm, 0);  return; \
    case p86_SETS:   x86asm_out(nat, SETSm, 0);   return; \
    case p86_SETZ:   x86asm_out(nat, SETZm, 0);   return; \
  }

  switch (dest->type)
  {
    case p86t_MEMDISP8:
    {
      uint5 mod = mod_DISP8;
      uint3 disp = dest->data.md8.displ;
      uint5 rm = dest->data.md8.rnum;
      #define MODSIBDISP MODDISP8
      XMSWITCH
      #undef MODSIBDISP
    }
    break;
    
    case p86t_MEMDISP32:
    {
      uint5 mod = mod_DISP8;
      uint5 disp = dest->data.md32.displ;
      uint5 rm = dest->data.md32.rnum;
      #define MODSIBDISP MODDISP32
      XMSWITCH
      #undef MODSIBDISP
    }
    break;
    
    case p86t_MEMSI:
    {
      uint5 mod = mod_INDIRECT;  // possibly?
      uint5 base = dest->data.msi.base;
      uint5 index = dest->data.msi.index;
      uint5 scale = dest->data.msi.scale;
      uint5 rm = USESIB;
      #define MODSIBDISP MODSIB
      XMSWITCH
      #undef MODSIBDISP
    }
    break;
    
    case p86t_MEMSIDISP8:
    {
      uint5 mod = mod_DISP8;  // possibly?
      uint5 base = dest->data.msid8.base;
      uint5 index = dest->data.msid8.index;
      uint5 scale = dest->data.msid8.scale;
      uint5 rm = USESIBDISP;
      uint3 disp = dest->data.msid8.displ;
      #define MODSIBDISP MODSIBDISP8
      XMSWITCH
      #undef MODSIBDISP
    }
    break;

    case p86t_MEMSIDISP32:
    {
      uint5 mod = mod_DISP32;  // possibly?
      uint5 base = dest->data.msid32.base;
      uint5 index = dest->data.msid32.index;
      uint5 scale = dest->data.msid32.scale;
      uint5 rm = USESIBDISP;
      uint3 disp = dest->data.msid32.displ;
      #define MODSIBDISP MODSIBDISP32
      XMSWITCH
      #undef MODSIBDISP
    }
    break;
  }
}

void p86_gen_mi(nativeblockinfo* nat, uint5 op, uint5 len, uint5 imm,
                p86_mtype* dest)
{
  #define MISWITCH \
  switch (op) \
  { \
    case p86_ADC:  x86asm_out(nat, ADClmi,  0); return; \
    case p86_ADD:  x86asm_out(nat, ADDlmi,  0); return; \
    case p86_AND:  x86asm_out(nat, ANDlmi,  0); return; \
    case p86_BT:   x86asm_out(nat, BTlmi,   0); return; \
    case p86_CMP:  x86asm_out(nat, CMPlmi,  0); return; \
    case p86_MOV:  x86asm_out(nat, MOVlmi,  0); return; \
    case p86_OR:   x86asm_out(nat, ORlmi,   0); return; \
    case p86_SBB:  x86asm_out(nat, SBBlmi,  0); return; \
    case p86_SUB:  x86asm_out(nat, SUBlmi,  0); return; \
    case p86_XOR:  x86asm_out(nat, XORlmi,  0); return; \
  }

  switch (dest->type)
  {
    case p86t_MEMDISP8:
    {
      uint5 mod = mod_DISP8;
      uint3 disp = dest->data.md8.displ;
      uint5 rm = dest->data.md8.rnum;
      #define MODSIBDISP MODDISP8
      MISWITCH
      #undef MODSIBDISP
    }
    break;
    
    case p86t_MEMDISP32:
    {
      uint5 mod = mod_DISP8;
      uint5 disp = dest->data.md32.displ;
      uint5 rm = dest->data.md32.rnum;
      #define MODSIBDISP MODDISP32
      MISWITCH
      #undef MODSIBDISP
    }
    break;
    
    case p86t_MEMSI:
    {
      uint5 mod = mod_INDIRECT;  // possibly?
      uint5 base = dest->data.msi.base;
      uint5 index = dest->data.msi.index;
      uint5 scale = dest->data.msi.scale;
      uint5 rm = USESIB;
      #define MODSIBDISP MODSIB
      MISWITCH
      #undef MODSIBDISP
    }
    break;
    
    case p86t_MEMSIDISP8:
    {
      uint5 mod = mod_DISP8;  // possibly?
      uint5 base = dest->data.msid8.base;
      uint5 index = dest->data.msid8.index;
      uint5 scale = dest->data.msid8.scale;
      uint5 rm = USESIBDISP;
      uint3 disp = dest->data.msid8.displ;
      #define MODSIBDISP MODSIBDISP8
      MISWITCH
      #undef MODSIBDISP
    }
    break;

    case p86t_MEMSIDISP32:
    {
      uint5 mod = mod_DISP32;  // possibly?
      uint5 base = dest->data.msid32.base;
      uint5 index = dest->data.msid32.index;
      uint5 scale = dest->data.msid32.scale;
      uint5 rm = USESIBDISP;
      uint3 disp = dest->data.msid32.displ;
      #define MODSIBDISP MODSIBDISP32
      MISWITCH
      #undef MODSIBDISP
    }
    break;
  }
}

void p86_gen_mr(nativeblockinfo* nat, uint5 op, uint5 len, uint5 reg,
                p86_mtype* dest)
{
  #define MRSWITCH \
  switch (op) \
  { \
    case p86_ADC:  x86asm_out(nat, ADClmr,  0); return; \
    case p86_ADD:  x86asm_out(nat, ADDlmr,  0); return; \
    case p86_AND:  x86asm_out(nat, ANDlmr,  0); return; \
    case p86_CMP:  x86asm_out(nat, CMPlmr,  0); return; \
    case p86_MOV:  x86asm_out(nat, MOVlmr,  0); return; \
    case p86_OR:   x86asm_out(nat, ORlmr,   0); return; \
    case p86_SBB:  x86asm_out(nat, SBBlmr,  0); return; \
    case p86_SUB:  x86asm_out(nat, SUBlmr,  0); return; \
    case p86_TEST: x86asm_out(nat, TESTlmr, 0); return; \
    case p86_XADD: x86asm_out(nat, XADDlmr, 0); return; \
    case p86_XCHG: x86asm_out(nat, XCHGlmr, 0); return; \
    case p86_XOR:  x86asm_out(nat, XORlmr,  0); return; \
  }

  switch (dest->type)
  {
    case p86t_MEMDISP8:
    {
      uint5 mod = mod_DISP8;
      uint3 disp = dest->data.md8.displ;
      uint5 rm = dest->data.md8.rnum;
      #define MODSIBDISP MODDISP8
      MRSWITCH
      #undef MODSIBDISP
    }
    break;
    
    case p86t_MEMDISP32:
    {
      uint5 mod = mod_DISP8;
      uint5 disp = dest->data.md32.displ;
      uint5 rm = dest->data.md32.rnum;
      #define MODSIBDISP MODDISP32
      MRSWITCH
      #undef MODSIBDISP
    }
    break;
    
    case p86t_MEMSI:
    {
      uint5 mod = mod_INDIRECT;  // possibly?
      uint5 base = dest->data.msi.base;
      uint5 index = dest->data.msi.index;
      uint5 scale = dest->data.msi.scale;
      uint5 rm = USESIB;
      #define MODSIBDISP MODSIB
      MRSWITCH
      #undef MODSIBDISP
    }
    break;
    
    case p86t_MEMSIDISP8:
    {
      uint5 mod = mod_DISP8;  // possibly?
      uint5 base = dest->data.msid8.base;
      uint5 index = dest->data.msid8.index;
      uint5 scale = dest->data.msid8.scale;
      uint5 rm = USESIBDISP;
      uint3 disp = dest->data.msid8.displ;
      #define MODSIBDISP MODSIBDISP8
      MRSWITCH
      #undef MODSIBDISP
    }
    break;

    case p86t_MEMSIDISP32:
    {
      uint5 mod = mod_DISP32;  // possibly?
      uint5 base = dest->data.msid32.base;
      uint5 index = dest->data.msid32.index;
      uint5 scale = dest->data.msid32.scale;
      uint5 rm = USESIBDISP;
      uint3 disp = dest->data.msid32.displ;
      #define MODSIBDISP MODSIBDISP32
      MRSWITCH
      #undef MODSIBDISP
    }
    break;
  }
}

void p86_gen_rm(nativeblockinfo* nat, uint5 op, uint5 len, p86_mtype* dest,
                uint5 reg)
{
  #define RMSWITCH \
  switch (op) \
  { \
    case p86_ADC:  x86asm_out(nat, ADClrm,  0); return; \
    case p86_ADD:  x86asm_out(nat, ADDlrm,  0); return; \
    case p86_AND:  x86asm_out(nat, ANDlrm,  0); return; \
    case p86_BT:   x86asm_out(nat, BTlrm,   0); return; \
    case p86_CMP:  x86asm_out(nat, CMPlrm,  0); return; \
    case p86_MOV:  x86asm_out(nat, MOVlrm,  0); return; \
    case p86_OR:   x86asm_out(nat, ORlrm,   0); return; \
    case p86_SBB:  x86asm_out(nat, SBBlrm,  0); return; \
    case p86_SUB:  x86asm_out(nat, SUBlrm,  0); return; \
    case p86_TEST: x86asm_out(nat, TESTlrm, 0); return; \
    case p86_XADD: x86asm_out(nat, XADDlrm, 0); return; \
    case p86_XCHG: x86asm_out(nat, XCHGlrm, 0); return; \
    case p86_XOR:  x86asm_out(nat, XORlrm,  0); return; \
  }

  switch (dest->type)
  {
    case p86t_MEMDISP8:
    {
      uint5 mod = mod_DISP8;
      uint3 disp = dest->data.md8.displ;
      uint5 rm = dest->data.md8.rnum;
      #define MODSIBDISP MODDISP8
      RMSWITCH
      #undef MODSIBDISP
    }
    break;
    
    case p86t_MEMDISP32:
    {
      uint5 mod = mod_DISP8;
      uint5 disp = dest->data.md32.displ;
      uint5 rm = dest->data.md32.rnum;
      #define MODSIBDISP MODDISP32
      RMSWITCH
      #undef MODSIBDISP
    }
    break;
    
    case p86t_MEMSI:
    {
      uint5 mod = mod_INDIRECT;  // possibly?
      uint5 base = dest->data.msi.base;
      uint5 index = dest->data.msi.index;
      uint5 scale = dest->data.msi.scale;
      uint5 rm = USESIB;
      #define MODSIBDISP MODSIB
      RMSWITCH
      #undef MODSIBDISP
    }
    break;
    
    case p86t_MEMSIDISP8:
    {
      uint5 mod = mod_DISP8;  // possibly?
      uint5 base = dest->data.msid8.base;
      uint5 index = dest->data.msid8.index;
      uint5 scale = dest->data.msid8.scale;
      uint5 rm = USESIBDISP;
      uint3 disp = dest->data.msid8.displ;
      #define MODSIBDISP MODSIBDISP8
      RMSWITCH
      #undef MODSIBDISP
    }
    break;

    case p86t_MEMSIDISP32:
    {
      uint5 mod = mod_DISP32;  // possibly?
      uint5 base = dest->data.msid32.base;
      uint5 index = dest->data.msid32.index;
      uint5 scale = dest->data.msid32.scale;
      uint5 rm = USESIBDISP;
      uint3 disp = dest->data.msid32.displ;
      #define MODSIBDISP MODSIBDISP32
      RMSWITCH
      #undef MODSIBDISP
    }
    break;
  }
}

void p86_gen_branch(nativeblockinfo* nat, uint5 op, uint5 len, uint5 disp)
{
  #define BRSWITCH(L) \
  switch (op) \
  { \
    case p86_JA:   x86asm_out(nat, JA##L##r, 0);   return; \
    case p86_JAE:  x86asm_out(nat, JAE##L##r, 0);  return; \
    case p86_JB:   x86asm_out(nat, JB##L##r, 0);   return; \
    case p86_JBE:  x86asm_out(nat, JBE##L##r, 0);  return; \
    case p86_JC:   x86asm_out(nat, JC##L##r, 0);   return; \
    case p86_JE:   x86asm_out(nat, JE##L##r, 0);   return; \
    case p86_JG:   x86asm_out(nat, JG##L##r, 0);   return; \
    case p86_JGE:  x86asm_out(nat, JGE##L##r, 0);  return; \
    case p86_JL:   x86asm_out(nat, JL##L##r, 0);   return; \
    case p86_JLE:  x86asm_out(nat, JLE##L##r, 0);  return; \
    case p86_JNA:  x86asm_out(nat, JNA##L##r, 0);  return; \
    case p86_JNB:  x86asm_out(nat, JNB##L##r, 0);  return; \
    case p86_JNBE: x86asm_out(nat, JNBE##L##r, 0); return; \
    case p86_JNC:  x86asm_out(nat, JNC##L##r, 0);  return; \
    case p86_JNE:  x86asm_out(nat, JNE##L##r, 0);  return; \
    case p86_JNG:  x86asm_out(nat, JNG##L##r, 0);  return; \
    case p86_JNGE: x86asm_out(nat, JNGE##L##r, 0); return; \
    case p86_JNL:  x86asm_out(nat, JNL##L##r, 0);  return; \
    case p86_JNLE: x86asm_out(nat, JNLE##L##r, 0); return; \
    case p86_JNO:  x86asm_out(nat, JNO##L##r, 0);  return; \
    case p86_JNP:  x86asm_out(nat, JNP##L##r, 0);  return; \
    case p86_JNS:  x86asm_out(nat, JNS##L##r, 0);  return; \
    case p86_JNZ:  x86asm_out(nat, JNZ##L##r, 0);  return; \
    case p86_JO:   x86asm_out(nat, JO##L##r, 0);   return; \
    case p86_JP:   x86asm_out(nat, JP##L##r, 0);   return; \
    case p86_JPE:  x86asm_out(nat, JPE##L##r, 0);  return; \
    case p86_JPO:  x86asm_out(nat, JPO##L##r, 0);  return; \
    case p86_JS:   x86asm_out(nat, JS##L##r, 0);   return; \
    case p86_JZ:   x86asm_out(nat, JZ##L##r, 0);   return; \
  }
  
  switch (len)
  {
    case p86l_byte:
    BRSWITCH(s);
    break;
    
    case p86l_long:
    BRSWITCH(n);
    break;
  }
}

#define expri(x,X) \
  void p86_##x##_ri(nativeblockinfo* nat, uint5 len, uint5 imm, uint5 rm) \
  { \
    p86_gen_ri(nat, p86_##X, len, imm, rm); \
  }

#define exprc(x,X) \
  void p86_##x##_rx(nativeblockinfo* nat, uint5 len, uint5 rm) \
  { \
    p86_gen_rc(nat, p86_##X, len, rm); \
  }

#define exprr(x,X) \
  void p86_##x##_rr(nativeblockinfo* nat, uint5 len, uint5 reg, uint5 rm) \
  { \
    p86_gen_rr(nat, p86_##X, len, reg, rm); \
  }

#define expmi(x,X) \
  void p86_##x##_mi(nativeblockinfo* nat, uint5 len, uint5 imm, p86_mtype* dest) \
  { \
    p86_gen_mi(nat, p86_##X, len, imm, dest); \
  }

#define expmr(x,X) \
  void p86_##x##_mr(nativeblockinfo* nat, uint5 len, uint5 reg, p86_mtype* dest) \
  { \
    p86_gen_mr(nat, p86_##X, len, reg, dest); \
  }

#define exprm(x,X) \
  void p86_##x##_rm(nativeblockinfo* nat, uint5 len, p86_mtype* dest, uint5 reg) \
  { \
    p86_gen_rm(nat, p86_##X, len, dest, reg); \
  }

#define expbranch(x,X) \
  void p86_##x##_xi(nativeblockinfo* nat, uint5 len, uint5 rel) \
  { \
    p86_gen_branch(nat, p86_##X, len, rel); \
  }

#define expxx(x,X) \
  void p86_##x##_xx(nativeblockinfo* nat, uint5 len) \
  { \
    p86_gen_xx(nat, p86_##X, len); \
  }

#define expxr(x,X) \
  void p86_##x##_xr(nativeblockinfo* nat, uint5 len, uint5 reg) \
  { \
    p86_gen_xr(nat, p86_##X, len, reg); \
  }

#define expxm(x,X) \
  void p86_##x##_xm(nativeblockinfo* nat, uint5 len, p86_mtype* mem) \
  { \
    p86_gen_xm(nat, p86_##X, len, mem); \
  }

expri(adc,ADC);
expri(add,ADD);
expri(and,AND);
expri(bt,BT);
expri(cmp,CMP);
expri(imul,IMUL);
expri(mov,MOV);
expri(or,OR);
expri(rcl,RCL);
expri(rcr,RCR);
expri(rol,ROL);
expri(ror,ROR);
expri(shl,SHL);
expri(sar,SAR);
expri(sbb,SBB);
expri(shr,SHR);
expri(sub,SUB);
expri(test,TEST);
expri(xor,XOR);

exprc(rcl,RCL);
exprc(rcr,RCR);
exprc(rol,ROL);
exprc(ror,ROR);
exprc(shl,SHL);
exprc(sar,SAR);
exprc(shr,SHR);

exprr(adc,ADC);
exprr(add,ADD);
exprr(and,AND);
exprr(bt,BT);
exprr(cmp,CMP);
exprr(imul,IMUL);
exprr(mov,MOV);
exprr(or,OR);
exprr(sbb,SBB);
exprr(sub,SUB);
exprr(test,TEST);
exprr(xadd,XADD);
exprr(xchg,XCHG);
exprr(xor,XOR);

expmi(adc,ADC);
expmi(add,ADD);
expmi(and,AND);
expmi(bt,BT);
expmi(cmp,CMP);
expmi(imul,IMUL);
expmi(mov,MOV);
expmi(or,OR);
expmi(sbb,SBB);
expmi(sub,SUB);
expmi(test,TEST);
expmi(xor,XOR);

expmr(adc,ADC);
expmr(add,ADD);
expmr(and,AND);
expmr(bt,BT);
expmr(cmp,CMP);
expmr(imul,IMUL);
expmr(mov,MOV);
expmr(or,OR);
expmr(sbb,SBB);
expmr(sub,SUB);
expmr(test,TEST);
expmr(xadd,XADD);
expmr(xchg,XCHG);
expmr(xor,XOR);

exprm(adc,ADC);
exprm(add,ADD);
exprm(and,AND);
exprm(bt,BT);
exprm(cmp,CMP);
exprm(imul,IMUL);
exprm(mov,MOV);
exprm(or,OR);
exprm(sbb,SBB);
exprm(sub,SUB);
exprm(test,TEST);
exprm(xadd,XADD);
exprm(xchg,XCHG);
exprm(xor,XOR);

expbranch(ja,JA);
expbranch(jae,JAE);
expbranch(jb,JB);
expbranch(jbe,JBE);
expbranch(jc,JC);
expbranch(je,JE);
expbranch(jg,JG);
expbranch(jge,JGE);
expbranch(jl,JL);
expbranch(jle,JLE);
expbranch(jna,JNA);
expbranch(jnb,JNB);
expbranch(jnbe,JNBE);
expbranch(jnc,JNC);
expbranch(jne,JNE);
expbranch(jng,JNG);
expbranch(jnge,JNGE);
expbranch(jnl,JNL);
expbranch(jnle,JNLE);
expbranch(jno,JNO);
expbranch(jnp,JNP);
expbranch(jns,JNS);
expbranch(jnz,JNZ);
expbranch(jo,JO);
expbranch(jp,JP);
expbranch(jpe,JPE);
expbranch(jpo,JPO);
expbranch(js,JS);
expbranch(jz,JZ);

expxx(clc,CLC);
expxx(cmc,CMC);
expxx(nop,NOP);
expxx(popf,POPF);
expxx(pushf,PUSHF);
expxx(ret,RET);
expxx(stc,STC);

expxr(call,CALL);
expxr(dec,DEC);
expxr(inc,INC);
expxr(jmp,JMP);
expxr(neg,NEG);
expxr(not,NOT);
expxr(pop,POP);
expxr(push,PUSH);
expxr(seta,SETA);
expxr(setae,SETAE);
expxr(setb,SETB);
expxr(setbe,SETBE);
expxr(setc,SETC);
expxr(sete,SETE);
expxr(setg,SETG);
expxr(setge,SETGE);
expxr(setl,SETL);
expxr(setle,SETLE);
expxr(setna,SETNA);
expxr(setnb,SETNB);
expxr(setnbe,SETNBE);
expxr(setnc,SETNC);
expxr(setne,SETNE);
expxr(setng,SETNG);
expxr(setnge,SETNGE);
expxr(setnl,SETNL);
expxr(setnle,SETNLE);
expxr(setno,SETNO);
expxr(setnp,SETNP);
expxr(setns,SETNS);
expxr(setnz,SETNZ);
expxr(seto,SETO);
expxr(setp,SETP);
expxr(setpe,SETPE);
expxr(setpo,SETPO);
expxr(sets,SETS);
expxr(setz,SETZ);

expxm(call,CALL);
expxm(dec,DEC);
expxm(inc,INC);
expxm(jmp,JMP);
expxm(neg,NEG);
expxm(not,NOT);
expxm(pop,POP);
expxm(push,PUSH);
expxm(seta,SETA);
expxm(setae,SETAE);
expxm(setb,SETB);
expxm(setbe,SETBE);
expxm(setc,SETC);
expxm(sete,SETE);
expxm(setg,SETG);
expxm(setge,SETGE);
expxm(setl,SETL);
expxm(setle,SETLE);
expxm(setna,SETNA);
expxm(setnb,SETNB);
expxm(setnbe,SETNBE);
expxm(setnc,SETNC);
expxm(setne,SETNE);
expxm(setng,SETNG);
expxm(setnge,SETNGE);
expxm(setnl,SETNL);
expxm(setnle,SETNLE);
expxm(setno,SETNO);
expxm(setnp,SETNP);
expxm(setns,SETNS);
expxm(setnz,SETNZ);
expxm(seto,SETO);
expxm(setp,SETP);
expxm(setpe,SETPE);
expxm(setpo,SETPO);
expxm(sets,SETS);
expxm(setz,SETZ);
