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
  ab_POP
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

typedef struct {
  genx86_r32_rm32 r32_rm32;
  genx86_rm32_r32 rm32_r32;
  genx86_rm32_i8 rm32_i8;
  genx86_rm32_i32 rm32_i32;
  genx86_rm32_c rm32_c;
  genx86_r32_rm32_i8 r32_rm32_i8;
  genx86_r32_rm32_i32 r32_rm32_i32;
  genx86_rm32 rm32;
  uint5 flagset;
  uint5 flagcorrupt;
} genx86_variant;

extern void genx86_test(void);

#endif
