#ifndef MACHINE_H
#define MACHINE_H 1

#include "defs.h"
#include "processor.h"
#include "registers.h"
#include "memory.h"
#include "hash.h"

struct insttab;
struct profile_state;

struct machineinfo {
  meminfo* mem;
  registerinfo* reg;
  // x86flaginfo x86flags;
  hashtable* blocks;
  hashentry* live;
  hashtable* translaterules;
  hashtable* breakpoints;
  struct profile_state* pstate;
  int trace;
  struct insttab* exectab;
  sint5 cycle;
#ifdef EMULART
  uint5 serialclock;
#endif
};

typedef struct machineinfo machineinfo;

// dependency problems otherwise
/*
#include "block.h"
#include "analyse.h"
*/

extern machineinfo* machine_create(uint5 memory);
extern void machine_start(machineinfo* machine);

#endif
