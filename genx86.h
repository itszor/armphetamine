#ifndef GENX86_H
#define GENX86_H 1

#include "defs.h"
#include "x86asm.h"
#include "rtasm.h"
#include "rtasm_fns.h"
#include "palloc.h"
#include "list.h"

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
  ab_JO,
  ab_JNO,
  ab_JB,
  ab_JAE,
  ab_JE,
  ab_JNE,
  ab_JBE,
  ab_JA,
  ab_JS,
  ab_JNS,
  ab_JL,
  ab_JGE,
  ab_JLE,
  ab_JG,
  ab_JMP,
  ab_CALL,
  ab_JECXZ,
  ab_BT
} genx86_ab86;

/*

Allocation types could be:
  - immediate
  - register
  - [register+displacement]
  - [register+register*scale]
  - [register+register*scale+displacement]

*/

typedef enum {
  gowidth_BIT,
  gowidth_BYTE,
  gowidth_WORD,
  gowidth_DWORD,
  gowidth_QWORD
} genx86_operand_width;

typedef enum {
  gotype_EMPTY,
  gotype_IMMEDIATE,
  gotype_REGISTER,
  gotype_INDREG,
  gotype_INDREGPLUSDISP8,
  gotype_INDREGPLUSDISP32,
  gotype_INDREGPLUSSCALEDREG,
  gotype_INDREGPLUSSCALEDREGPLUSDISP8,
  gotype_INDREGPLUSSCALEDREGPLUSDISP32,
  gotype_ADDRESS,
  gotype_NUMTYPES
} genx86_operand_type;

struct genx86_operand {
  genx86_operand_type type;
  genx86_operand_width width;
  union {
    uint5 imm;
    uint5 reg;
    uint5 addr;
    struct {
      uint5 base;
      uint5 disp;
    } regdisp;
    struct {
      uint5 base;
      uint5 index;
      uint3 scale;
    } regscale;
    struct {
      uint5 base;
      uint5 index;
      uint5 offset;
      uint3 scale;
    } regscaledisp;
  } data;
};

typedef struct genx86_operand genx86_operand;

/*
typedef struct {
  genx86_operand* start;
  genx86_operand* end;
  uint5 reg;
} genx86_registerlifetime;
*/

/*
+++ Each op points to an entry in a table corresponding to each virtual register. List-form code might be generated at an earlyish stage of compilation, say after the source-dest aliasing phase. Then, register non-orthogonalities can be used to guide allocation decisions, especially function calling conventions etc.

Temporaries in x86 code can be handled by extending the virtual register mapping array thingy?
*/

typedef struct {
  genx86_ab86 operator;
  genx86_operand *op[3];
} genx86_op;

typedef struct {
  uint5 var;
  uint5 reg;
  clist* loc;
} genx86_delayedfetchcommit;

struct genx86_buffer {
  clist* buffer;
  list* fetch;
  list* commit;
  uint3 expecting;
  uint3 beenset;
};

typedef struct genx86_buffer genx86_buffer;

typedef void (*genx86_r32_rm32)(nativeblockinfo*, uint3, rtasm_mtype);
typedef void (*genx86_rm32_r32)(nativeblockinfo*, rtasm_mtype, uint3);
typedef void (*genx86_rm32_i8)(nativeblockinfo*, rtasm_mtype, uint3);
typedef void (*genx86_rm32_i32)(nativeblockinfo*, rtasm_mtype, uint5);
typedef void (*genx86_rm8_i8)(nativeblockinfo*, rtasm_mtype, uint3);
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
  genx86_rm8_i8 rm8_i8;
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
extern void genx86_append(clist* buffer, uint5 opcode, palloc_info* dest,
  palloc_info* src1, palloc_info* src2);
extern void genx86_move(clist* buffer, palloc_info* dest, palloc_info* src);
extern void genx86_out(nativeblockinfo* nat, uint5 opcode, palloc_info* dest,
                       palloc_info* src1, palloc_info* src2, list* x);
extern nativeblockinfo* genx86_translate(pheta_chunk* chunk);
extern uint5 genx86_translate_opcode(genx86_buffer* buf,
  pheta_chunk* chunk, pheta_instr* instr);

#endif
