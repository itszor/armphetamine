#ifndef GENX86_H
#define GENX86_H 1

#include "defs.h"
#include "x86asm.h"
#include "rtasm.h"
#include "rtasm_fns.h"
#include "palloc.h"

typedef enum {
  ab_SHL,
  ab_SHR,
  ab_SAR,
  ab_ROR,
  ab_ROL,
  ab_AND,
  ab_OR,
  ab_XOR,
  ab_ADD,
  ab_ADC,
  ab_SUB,
  ab_SBB,
  ab_IMUL,
  ab_LEA,
  ab_MOV,
  ab_NOT,
  ab_PUSH,
  ab_POP,
  ab_RCR,
  ab_RCL,
  ab_TEST,
  ab_CMP,
  ab_RET,
  ab_SETO,
  ab_SETNO,
  ab_SETB,
  ab_SETAE,
  ab_SETE,
  ab_SETNE,
  ab_SETBE,
  ab_SETA,
  ab_SETS,
  ab_SETNS,
  ab_SETL,
  ab_SETGE,
  ab_SETLE,
  ab_SETG,
  ab_CALL,
  ab_JECXZ
} genx86_ab86;

typedef struct {
  genx86_ab86 type;
  palloc_type* op1;
  palloc_type* op2;
} genx86_op;

typedef void (*genx86_r32_rm32)(nativeblockinfo*, uint3, rtasm_mtype);
typedef void (*genx86_rm32_r32)(nativeblockinfo*, rtasm_mtype, uint3);
typedef void (*genx86_rm32_i8)(nativeblockinfo*, rtasm_mtype, uint3);
typedef void (*genx86_rm32_i32)(nativeblockinfo*, rtasm_mtype, uint5);
typedef void (*genx86_rm32_c)(nativeblockinfo*, rtasm_mtype);
typedef void (*genx86_r32_rm32_i8)(nativeblockinfo*, uint3, rtasm_mtype, uint3);
typedef void (*genx86_r32_rm32_i32)(nativeblockinfo*, uint3, rtasm_mtype, 
                                    uint5);
typedef void (*genx86_rm32)(nativeblockinfo* nat, rtasm_mtype);
typedef void (*genx86_i8)(nativeblockinfo* nat, uint3);
typedef void (*genx86_i32)(nativeblockinfo* nat, uint5);
typedef void (*genx86_narg)(nativeblockinfo* nat);

typedef struct {
  genx86_r32_rm32 r32_rm32;
  genx86_rm32_r32 rm32_r32;
  genx86_rm32_i8 rm32_i8;
  genx86_rm32_i32 rm32_i32;
  genx86_rm32_c rm32_c;
  genx86_r32_rm32_i8 r32_rm32_i8;
  genx86_r32_rm32_i32 r32_rm32_i32;
  genx86_rm32 rm32;
  genx86_i8 i8;
  genx86_i32 i32;
  genx86_narg narg;
  uint5 flagset;
  uint5 flagcorrupt;
} genx86_variant;

extern void genx86_test(void);
extern void genx86_out(nativeblockinfo* nat, uint5 opcode, palloc_info* dest,
                       palloc_info* src1, palloc_info* src2, uint5 line);

#endif
