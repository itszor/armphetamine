#ifndef PHETA_H
#define PHETA_H 1

#include "libjtype/defs.h"
#include "libjtype/list.h"
#include "core/decode.h"
#include "core/machine.h"
#include "libjtype/bset.h"
#include "libjtype/pqueue.h"
#include "libjtype/clist.h"

typedef enum {
  ph_CONST,
  ph_CONSTB,
  ph_FETCH,
  ph_COMMIT,
  ph_ASSOC,
  ph_SPILL,
  ph_RELOAD,
  ph_PHI,
  ph_FEXPECT,
  ph_FCOMMIT,
  ph_FENSURE,
  ph_NFEXPECT,
  ph_NFCOMMIT,
  ph_NFENSURE,
  ph_FWRITE,
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
  ph_STATE,
  ph_SYNC,
  ph_XJMP,
  ph_UKJMP,
  ph_CAJMP,
  ph_CAJMP26F,
  ph_RTS,
  ph_ADDRESS
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

#define ph_IREG 8

typedef struct {
  pheta_opcode opcode;
  union {
    struct {
      uint4 dest;
      uint4 src1;
      uint4 src2;
    } op;
    uint5 imm;
    struct {
      uint5 pred;
      uint3 need;
      uint3 have;
    } flag;
    struct {
      uint5 imm;
      uint4 dest;
    } con;
    void* ptr;
  } data;
} pheta_instr;

struct genx86_buffer;

typedef struct pheta_basicblock_t {
  jt_clist* base;
  struct genx86_buffer* gxbuffer;
  uint5 length;
  uint5 size;
  uint5 cycles;
  uint5 srcstart;
  uint5 marker;
  uint5 required;
  uint5 set;
  uint5 discovertime;
  uint5 finishtime;
  struct pheta_basicblock_t* trueblk;
  struct pheta_basicblock_t* falseblk;
  struct pheta_basicblock_t* parent;
  jt_clist* predecessor;  // flag resolution needs all predecessors for a block
  struct pheta_basicblock_t* scsubgraph;
  char* comment;
  jt_bset* active;
  sint5 lbuf[ph_NUMREG];
  uint3 dirtybuf[ph_NUMREG];
  jt_pqueue* live;
  uint3 predicate;
  jt_list* reloc;
  uint5 natoffset;
} pheta_basicblock;

typedef struct {
  uint5 ph, arm;
} pheta_rpair;

struct palloc_info;
struct palloc_liverange;

typedef struct {
  jt_list* blocks;
  uint5 tempno;
  uint5 predno;
  uint5 virtualaddress;  // this is a horrible place for this
  uint5 start;
  uint5 length;
  jt_hash* leaders;
  pheta_basicblock* root;
  pheta_basicblock* currentblock;
  struct palloc_info* alloc;
  struct palloc_liverange** reversetable;
  uint3 reguse[ph_IREG];
  uint5 regno;
  jt_pqueue* active;
  jt_list* assoc;
  jt_hash* constantpool;
  jt_hash* registerpool;
  jt_hash* baseoffsetpool;
  jt_list* miscoperand;
  machineinfo* parentmachine;
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

#define ph_NAT 0x10

#define bitcc_EQ 0x0001
#define bitcc_NE 0x0002
#define bitcc_CS 0x0004
#define bitcc_CC 0x0008
#define bitcc_MI 0x0010
#define bitcc_PL 0x0020
#define bitcc_VS 0x0040
#define bitcc_VC 0x0080
#define bitcc_HI 0x0100
#define bitcc_LS 0x0200
#define bitcc_GE 0x0400
#define bitcc_LT 0x0800
#define bitcc_GT 0x1000
#define bitcc_LE 0x2000
#define bitcc_AL 0x4000
#define bitcc_NV 0x8000

#define ph_Z  0x01
#define ph_C  0x02
#define ph_N  0x04
#define ph_V  0x08
#define ph_IC 0x10

#define ph_ALWAYS 0

#define ph_MAXDEST 1
#define ph_MAXSRC 2

#define col_WHITE 0
#define col_GREY 1
#define col_BLACK 2

extern const uint3 pheta_instlength[];

extern pheta_chunk* pheta_newchunk(uint5 start, uint5 length);

extern pheta_basicblock* pheta_newbasicblock(pheta_chunk* c, uint5 startaddr);

extern void pheta_destroybasicblock(pheta_basicblock* blk);

extern pheta_chunk* pheta_translatechunk(machineinfo* machine, uint5 base,
  uint5 length);
                                         
extern void pheta_link(pheta_basicblock* from, uint5 code,
  pheta_basicblock* condtrue, pheta_basicblock* condfalse);
  
extern uint5 pheta_emit(pheta_chunk* chunk, pheta_opcode opcode, ...);

extern void pheta_getused(pheta_instr* instr, uint5* numdest,
  uint5 dest[], uint5* numsrc, uint5 src[]);
  
extern void pheta_dfs_visit(pheta_basicblock* blk, uint5* time);

extern void pheta_dfs(pheta_chunk* chunk);

extern void pheta_scc(pheta_chunk* chunk);

extern void pheta_scc_visit(pheta_basicblock* blk);

extern void pheta_predecessor(pheta_chunk* chunk);

extern void pheta_gdlprint(pheta_chunk* chunk, char* outfile);

extern void pheta_davinciprint(pheta_chunk* chunk, char* outfile);

extern uint5 pheta_fixup_flags_inner(pheta_basicblock* blk, uint5 blktag,
  uint5 needpred, uint5 needflag);
  
extern void pheta_fixup_flags(pheta_chunk* chunk);

extern void pheta_optimise_transitive_branch(pheta_chunk* chunk);

extern void pheta_cull_unused_nodes(pheta_chunk* chunk);

extern uint5 pheta_lfetch(pheta_chunk* chunk, uint5 regno);

extern void pheta_lcommit(pheta_chunk* chunk, uint5 regno, uint5 tempno);

extern void pheta_lsync(pheta_chunk* chunk);

extern pheta_basicblock* pheta_getbasicblock(pheta_chunk* chunk, uint5 line);

extern int pheta_dp(machineinfo* machine, instructionformat inst,
  void* chunk);
  
extern int pheta_dp_imm(machineinfo* machine, instructionformat inst,
  void* chunk);
  
extern int pheta_dp_guts(machineinfo* machine, instructionformat inst,
  pheta_chunk* chunk, uint5 op2);
  
extern int pheta_mul(machineinfo* machine, instructionformat inst,
  void* chunk);

extern int pheta_mull(machineinfo* machine, instructionformat inst,
  void* chunk);
  
extern int pheta_sdt(machineinfo* machine, instructionformat inst,
  void* chunk);

extern int pheta_sdth(machineinfo* machine, instructionformat inst,
  void* chunk);
  
extern int pheta_bdt(machineinfo* machine, instructionformat inst,
  void* chunk);
  
extern int pheta_bra(machineinfo* machine, instructionformat inst,
  void* chunk);
  
extern int pheta_swi(machineinfo* machine, instructionformat inst,
  void* chunk);
  
extern int pheta_cdt(machineinfo* machine, instructionformat inst,
  void* chunk);
  
extern int pheta_cdo(machineinfo* machine, instructionformat inst,
  void* chunk);
  
extern int pheta_crt(machineinfo* machine, instructionformat inst,
  void* chunk);
  
extern int pheta_sds(machineinfo* machine, instructionformat inst,
  void* chunk);
  
extern int pheta_und(machineinfo* machine, instructionformat inst,
  void* chunk);
  
#endif
