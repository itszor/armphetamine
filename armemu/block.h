#ifndef BLOCK_H
#define BLOCK_H 1

#include "defs.h"
#include "machine.h"
#include "pseudo.h"
#include "hash.h"
#include "x86asm.h"

typedef struct {
  uint5 length;  // extent of this block
	uint5 usecount;
	psblock* psb;
	hashtable* lastset;     // this and...
	hashtable* nextused;    // ...this form an instruction dependency graph
  nativeblockinfo* native;
} blockinfo;

#define EXECTHRESHOLD 5

// these must be included after blockinfo is defined; they incorporate
// stuff to ensure this file is included first
#include "codegen.h"
#include "analyse.h"

extern blockinfo* block_new(void);
extern void block_clean(blockinfo*);
extern void block_delete(blockinfo*);
extern uint5 block_gather(machineinfo*, uint5);

extern void pseudo_translateblock(machineinfo*, blockinfo*, uint5);

#endif
