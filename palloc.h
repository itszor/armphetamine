#ifndef PALLOC_H
#define PALLOC_H 1

#include "defs.h"
#include "pheta.h"
#include "list.h"

typedef enum {
  pal_UNSET,
  pal_CONSTB,
  pal_CONST,
  pal_RFILE,
  pal_IREG
} palloc_type;

typedef struct {
  pheta_basicblock* startblk;
  uint5 startline;
  pheta_basicblock* endblk;
  uint5 endline;
} palloc_edge;

typedef struct {
  palloc_type type;
  union {
    struct {
      uint5 num;
/*      uint5 arm_affiliation;*/
    } ireg;
    uint5 value;
  } info;
  hash* referenced_by;
  palloc_edge edge;
} palloc_info;

#endif
