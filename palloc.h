#ifndef PALLOC_H
#define PALLOC_H 1

#include "defs.h"
#include "pheta.h"
#include "list.h"
#include "hash.h"

typedef enum {
  pal_UNSET,
  pal_CONSTB,
  pal_CONST,
  pal_RFILE,
  pal_IREG,
  pal_STACK,
  pal_ALIAS,
  pal_SPLIT,
  pal_NUMTYPES
} palloc_type;

struct palloc_liverange {
  uint5 startline;
  uint5 length;
  uint5 reg;
};

struct palloc_info {
  palloc_type type;
  union {
    struct {
      uint5 num;
    } ireg;
    uint5 value;
    pqueue* extra;
  } info;
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


extern void palloc_srcdestalias(pheta_chunk* chunk);
extern void palloc_srcdestalias_inner(pheta_chunk* chunk, pheta_basicblock* blk,
                                      uint5 startline);
extern void palloc_closealias(pheta_chunk* chunk);
extern void palloc_deletespans(pheta_chunk* chunk);
extern void palloc_clearmarkers(pheta_chunk* chunk);
extern uint5 palloc_close(pheta_chunk* chunk, uint5 reg);
extern void palloc_findspans(pheta_chunk* chunk, pheta_basicblock* blk,
                      uint5 startline);
extern void palloc_printspans(pheta_chunk* chunk);

#endif
