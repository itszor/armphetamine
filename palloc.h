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
/*      uint5 arm_affiliation;*/
    } ireg;
    uint5 value;
  } info;
/*  hashtable* referenced_by;
  palloc_liverange edge;*/
};

typedef struct palloc_info palloc_info;
typedef struct palloc_liverange palloc_liverange;

extern void palloc_srcdestalias(pheta_chunk* chunk, pheta_basicblock* blk);
extern void palloc_srcdestalias_inner(pheta_chunk* chunk, pheta_basicblock* blk,
                                      uint5 startline);
extern void palloc_findspans(pheta_chunk* chunk, pheta_basicblock* blk,
                      uint5 startline);
extern void palloc_printspans(pheta_chunk* chunk);

#endif
