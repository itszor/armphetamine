#ifndef PROFILE_H
#define PROFILE_H 1

#include "defs.h"
#include "transmap.h"

struct profile_state {
  uint5 context;
  uint5 ctxtmask;
  uint5 start;
  transmap_entry* currenttrans;
};

#define RECOMPILE_THRESHOLD 16

typedef struct profile_state profile_state;

extern void profile_feedseqaddr(meminfo* mem, profile_state* pstate,
  uint5 addr);

extern void profile_feednseqaddr(meminfo* mem, profile_state* pstate,
  uint5 addr);

#endif
