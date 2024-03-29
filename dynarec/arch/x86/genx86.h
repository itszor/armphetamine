#ifndef GENX86_H
#define GENX86_H 1

#include "libjtype/defs.h"
#include "dynarec/arch/x86/x86asm.h"
#include "dynarec/arch/x86/rtasm.h"
#include "dynarec/arch/x86/rtasm_fns.h"
#include "dynarec/palloc.h"
#include "libjtype/list.h"
#include "core/memory.h"

typedef enum {
  ab_SHL,
  ab_SHR,
  ab_SAR,
  ab_ROR,
  ab_ROL,
  ab_RCR,
  ab_RCL,
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
  ab_BT,
  ab_CMC,
  ab_STC,
  ab_CLC,
  ab_NOP
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
  gowidth_QWORD,
  gowidth_NUMWIDTHS
} genx86_operand_width;

typedef enum {
  gotype_EMPTY,
  gotype_PARTIAL,
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
    palloc_info* src;
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
  union {
    struct {
      uint5 op0born : 1;  /* might be useful for peephole optimisation */
      uint5 op0kill : 1;
      uint5 op1born : 1;
      uint5 op1kill : 1;
      uint5 op2born : 1;
      uint5 op2kill : 1;
      uint5 unused  : 26;
    } flags;
    uint5 all;
  } data;
} genx86_op;

typedef struct {
  uint5 src;
  palloc_info* var;
  jt_clist* loc;
} genx86_delayedfetchcommit;

struct genx86_buffer {
  jt_clist* buffer;
  jt_list* fetch;
  jt_list* commit;
  uint3 expecting;
  uint3 beenset;
  jt_hash* reloc;
};

typedef struct genx86_buffer genx86_buffer;

typedef void (*genx86_rm8)(nativeblockinfo* nat, rtasm_mtype);
typedef void (*genx86_i8)(nativeblockinfo* nat, uint3);
typedef void (*genx86_r8_rm8)(nativeblockinfo* nat, uint3, rtasm_mtype);
typedef void (*genx86_rm8_r8)(nativeblockinfo* nat, rtasm_mtype, uint3);
typedef void (*genx86_r8_i8)(nativeblockinfo* nat, uint3, uint3);
typedef void (*genx86_rm8_i8)(nativeblockinfo*, rtasm_mtype, uint3);
typedef void (*genx86_rm32)(nativeblockinfo* nat, rtasm_mtype);
typedef void (*genx86_r32)(nativeblockinfo* nat, uint3);
typedef void (*genx86_i32)(nativeblockinfo* nat, uint5);
typedef void (*genx86_r32_rm32)(nativeblockinfo*, uint3, rtasm_mtype);
typedef void (*genx86_rm32_r32)(nativeblockinfo*, rtasm_mtype, uint3);
typedef void (*genx86_r32_i32)(nativeblockinfo*, uint3, uint5);
typedef void (*genx86_rm32_i8)(nativeblockinfo*, rtasm_mtype, uint3);
typedef void (*genx86_rm32_i32)(nativeblockinfo*, rtasm_mtype, uint5);
typedef void (*genx86_rm32_c)(nativeblockinfo*, rtasm_mtype);
typedef void (*genx86_r32_rm32_i8)(nativeblockinfo*, uint3, rtasm_mtype, uint3);
typedef void (*genx86_r32_rm32_i32)(nativeblockinfo*, uint3, rtasm_mtype, 
                                    uint5);
typedef void (*genx86_narg)(nativeblockinfo* nat);

typedef struct {
  genx86_rm8          rm8;             // m
  genx86_i8           i8;              // i
  genx86_r8_rm8       r8_rm8;          // rm
  genx86_rm8_r8       rm8_r8;          // mr
  genx86_r8_i8        r8_i8;           // ri
  genx86_rm8_i8       rm8_i8;          // mi
  genx86_rm32         rm32;            // M
  genx86_i32          i32;             // I
  genx86_r32          r32;             // R
  genx86_r32_rm32     r32_rm32;        // RM
  genx86_rm32_r32     rm32_r32;        // MR
  genx86_r32_i32      r32_i32;         // RI
  genx86_rm32_i8      rm32_i8;         // Mi
  genx86_rm32_i32     rm32_i32;        // MI
  genx86_rm32_c       rm32_c;          // Mc
  genx86_r32_rm32_i8  r32_rm32_i8;     // RMi
  genx86_r32_rm32_i32 r32_rm32_i32;    // RMI
  genx86_narg         narg;            // n
  uint5 flagset;
  uint5 flagcorrupt;
} genx86_variant;

extern void genx86_append(pheta_chunk* chunk, genx86_buffer*, uint5 opcode, 
  genx86_operand* dest, genx86_operand* src1, genx86_operand* src2);
  
extern void genx86_move(pheta_chunk* chunk, genx86_buffer*, palloc_info* dest, 
  palloc_info* src);
  
extern void genx86_out(nativeblockinfo* nat, uint5 opcode, palloc_info* dest,
  palloc_info* src1, palloc_info* src2, jt_list* x);

extern genx86_buffer* genx86_newbuffer(void);

extern nativeblockinfo* genx86_translate(pheta_chunk* chunk);

extern uint5 genx86_translate_opcode(genx86_buffer* buf,
  pheta_chunk* chunk, pheta_basicblock* blk, pheta_instr* instr,
  meminfo* mem);

extern nativeblockinfo* genx86_flatten_code(pheta_chunk*);

extern void genx86_insert_spill_code(pheta_chunk* chunk);

extern void genx86_complete_alloc(pheta_chunk* chunk);

#endif
