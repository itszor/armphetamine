#ifndef PALLOC_H
#define PALLOC_H 1

#include "defs.h"
#include "pheta.h"
#include "list.h"
#include "hash.h"
#include "memory.h"

typedef enum {
  pal_UNSET,
  pal_CONSTB,
  pal_CONST,
  pal_RFILE,
  pal_IREG,
  pal_STACK,
  pal_ALIAS,
  pal_SPILLED,
  pal_NUMTYPES
} palloc_type;

struct palloc_liverange {
  uint5 startline;
  uint5 length;
  uint5 reg;
};

struct genx86_operand;

struct palloc_info {
  palloc_type type;
  union {
    struct {
      uint5 num;
    } ireg;
    uint5 value;
    pqueue* extra;
  } info;
  struct genx86_operand* slot;
  sint5 arm_affiliation;
/*  hashtable* referenced_by;
  palloc_liverange edge;*/
};

typedef struct palloc_info palloc_info;
typedef struct palloc_liverange palloc_liverange;

typedef struct {
  palloc_info upper;
  palloc_info lower;
  uint5 splitline;
} palloc_splitalloc;

// mapping between 32-bit registers and 8-bit register numbers
#define LO8BITREG(X) (X)
#define HI8BITREG(X) ((X)+4)

extern void palloc_init(pheta_chunk* chunk);

extern void palloc_constant(pheta_chunk* chunk);

extern void palloc_shufflecommit(pheta_chunk* chunk);

extern void palloc_srcdestalias(pheta_chunk* chunk);

extern uint5 palloc_srcdestalias_inner(pheta_chunk* chunk, pheta_basicblock* 
  blk, uint5 startline);

extern void palloc_closealias(pheta_chunk* chunk);

extern void palloc_deletespans(pheta_chunk* chunk);

extern void palloc_clearmarkers(pheta_chunk* chunk);

extern uint5 palloc_close(pheta_chunk* chunk, uint5 reg);

extern uint5 palloc_linearscan_inner(pheta_chunk* chunk, pheta_basicblock* blk,
  uint5 startline, meminfo* mem);

extern void palloc_linearscan(pheta_chunk* chunk, meminfo* mem);

extern uint5 palloc_evict_ireg(pheta_chunk*, uint5 reg, uint5 except);

extern uint5 palloc_request_8bitsafe_ireg(pheta_chunk* chunk, uint5* reg,
  uint5 except);

extern void palloc_relinquish_ireg(pheta_chunk* chunk, uint5 reg);

extern uint5 palloc_findspans(pheta_chunk* chunk, pheta_basicblock* blk,
  uint5 startline);

extern void palloc_printspans(pheta_chunk* chunk);

extern void palloc_print(pheta_chunk* chunk);

#endif
