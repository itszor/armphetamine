#ifndef PROFILE_H
#define PROFILE_H 1

#include "defs.h"
#include "transmap.h"

struct profile_state {
  uint5 context;
  uint5 ctxtmask;
  uint5 start;
  uint5 length;
  transmap_entry* currenttrans;
};

#define RECOMPILE_THRESHOLD 16

typedef struct profile_state profile_state;

#endif
