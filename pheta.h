#ifndef PHETA_H
#define PHETA_H 1

#include "defs.h"
#include "list.h"
#include "decode.h"
#include "machine.h"
#include "bset.h"
#include "pqueue.h"

typedef enum {
  ph_CONST,
  ph_CONSTB,
  ph_FETCH,
  ph_COMMIT,
  ph_SPILL,
  ph_RELOAD,
  ph_FEXPECT,
  ph_FCOMMIT,
  ph_FENSURE,
  ph_SETPRED,
  ph_NFEXPECT,
  ph_NFCOMMIT,
  ph_NFENSURE,
  ph_NSETPRED,
  ph_FWRITE,
  ph_XJMP,
  ph_LSL,
  ph_LSR,
  ph_ASR,
  ph_ROR,
  ph_ROL,  // for completeness
  ph_RRX,
  ph_RLX,  // for completeness
  ph_MOV,
  ph_NOT,
  ph_AND,
  ph_OR,
  ph_EOR,
  ph_TEQ,
  ph_TST,
  ph_ADD,
  ph_ADC,
  ph_SUB,
  ph_SBC,
  ph_CMP,
  ph_CMN,
  ph_MUL,
  ph_LDW,
  ph_LDB,
  ph_STW,
  ph_STB,
  ph_SWI,
  ph_UNDEF,
  ph_SYNC,
  ph_END
} pheta_opcode;

typedef enum {
  ph_R0,
  ph_R1,
  ph_R2,
  ph_R3,
  ph_R4,
  ph_R5,
  ph_R6,
  ph_R7,
  ph_R8,
  ph_R9,
  ph_R10,
  ph_R11,
  ph_R12,
  ph_R13,
  ph_R14,
  ph_R15_FULL,
  ph_R15_ADDR,
  ph_R15_FLAG,
  ph_CPSR_FLAG,
  ph_CPSR_ALL,
  ph_SPSR,
  ph_CLOCK,
  ph_NUMREG
} pheta_armregisters;

typedef struct pheta_basicblock_t {
  uint3* base;
  uint5 length;
  uint5 size;
  uint5 cycles;
  uint5 srcstart;
  uint3 predicate;
  uint3 marker;
  uint5 required;
  uint5 set;
  struct pheta_basicblock_t* trueblk;
  struct pheta_basicblock_t* falseblk;
  struct pheta_basicblock_t* parent;
  char* comment;
  bset_info* active;
  sint5 lbuf[ph_NUMREG];
  uint3 dirtybuf[ph_NUMREG];
  pqueue* live;
} pheta_basicblock;

struct palloc_info;
struct palloc_liverange;

typedef struct {
  list* blocks;
  int tempno;
  int predno;
  int force;
  int virtualaddress;  // this is a horrible place for this
  uint5 start;
  uint5 length;
  uint5 stacksize;
  hashtable* leaders;
  pheta_basicblock* root;
  pheta_basicblock* currentblock;
  struct palloc_info* alloc;
  struct palloc_liverange** reversetable;
  uint3 reguse[8];
  uint5 regno;
  uint5* rename;
  pqueue* active;
} pheta_chunk;

/*
typedef struct {
  sint5 start;
  sint5 end;
} pheta_range;
*/

typedef enum {
  ph_EQ,
  ph_NE,
  ph_CS,
  ph_CC,
  ph_MI,
  ph_PL,
  ph_VS,
  ph_VC,
  ph_HI,
  ph_LS,
  ph_GE,
  ph_LT,
  ph_GT,
  ph_LE,
  ph_AL,
  ph_NV
} pheta_condition_code;

#define ph_EQ 0x0001
#define ph_NE 0x0002
#define ph_CS 0x0004
#define ph_CC 0x0008
#define ph_MI 0x0010
#define ph_PL 0x0020
#define ph_VS 0x0040
#define ph_VC 0x0080
#define ph_HI 0x0100
#define ph_LS 0x0200
#define ph_GE 0x0400
#define ph_LT 0x0800
#define ph_GT 0x1000
#define ph_LE 0x2000
#define ph_AL 0x4000
#define ph_NV 0x8000

#define ph_Z  ph_EQ
#define ph_C  ph_CS
#define ph_IC ph_CC
#define ph_N  ph_MI
#define ph_V  ph_VS

#define ph_ALWAYS 0

#define ph_MAXDEST 1
#define ph_MAXSRC 2

#define col_WHITE 0
#define col_GREY 1
#define col_BLACK 2

extern const uint3 pheta_instlength[];

extern pheta_chunk* pheta_newchunk(uint5 start, uint5 length);

extern pheta_basicblock* pheta_newbasicblock(pheta_chunk* c, uint5 startaddr);

extern pheta_chunk* pheta_translatechunk(machineinfo* machine, uint5 base,
                                         uint5 length);
extern void pheta_link(pheta_basicblock* from, uint5 code,
                       pheta_basicblock* condtrue, pheta_basicblock* condfalse);
extern void pheta_getused(uint3* base, int index, uint5* numdest, uint5 dest[],
                          uint5* numsrc, uint5 src[]);
extern void pheta_dfs_visit(pheta_basicblock* blk);
extern void pheta_dfs(pheta_chunk* chunk);

extern uint5 pheta_lfetch(pheta_chunk* chunk, uint5 regno);

extern void pheta_lcommit(pheta_chunk* chunk, uint5 regno, uint5 tempno);

extern void pheta_lsync(pheta_chunk* chunk);

extern void pheta_dp(machineinfo* machine, instructionformat inst,
                     void* chunk);
extern void pheta_dp_imm(machineinfo* machine, instructionformat inst,
                         void* chunk);
extern void pheta_dp_guts(machineinfo* machine, instructionformat inst,
                          pheta_chunk* chunk, uint5 op2);
extern void pheta_mul(machineinfo* machine, instructionformat inst,
                      void* chunk);
extern void pheta_sdt(machineinfo* machine, instructionformat inst,
                      void* chunk);
extern void pheta_bdt(machineinfo* machine, instructionformat inst,
                      void* chunk);
extern void pheta_bra(machineinfo* machine, instructionformat inst,
                      void* chunk);
extern void pheta_swi(machineinfo* machine, instructionformat inst,
                      void* chunk);
extern void pheta_cdt(machineinfo* machine, instructionformat inst,
                      void* chunk);
extern void pheta_cdo(machineinfo* machine, instructionformat inst,
                      void* chunk);
extern void pheta_crt(machineinfo* machine, instructionformat inst,
                      void* chunk);
extern void pheta_sds(machineinfo* machine, instructionformat inst,
                      void* chunk);
extern void pheta_und(machineinfo* machine, instructionformat inst,
                      void* chunk);
  
#endif
