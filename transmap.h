#ifndef TRANSMAP_H
#define TRANSMAP_H 1

#include "defs.h"
#include "hash.h"
#include "pheta.h"

typedef struct {
  uint3* code;
  uint5 size;
  uint5 usecount;
  uint5 context;
} transmap_entry;

#endif
