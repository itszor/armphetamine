#ifndef TRANSMAP_H
#define TRANSMAP_H 1

#include "defs.h"
#include "hash.h"
#include "pheta.h"

typedef struct {
  uint3* code;
  uint5 size;
  uint5 refcount;
  uint5 context;
} transmap_entry;

typedef struct {
  uint5 context;
  uint5 ctxtmask;
  uint5 start;
  uint5 length;
} transmap_profilestate;

#endif
