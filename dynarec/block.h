#ifndef BLOCK_H
#define BLOCK_H 1

#include "defs.h"
#include "machine.h"
#include "hash.h"
#include "x86asm.h"

typedef struct {
  uint5 length;  // extent of this block
	uint5 usecount;
/*	psblock* psb;*/
	jt_hash* lastset;     // this and...
	jt_hash* nextused;    // ...this form an instruction dependency graph
  nativeblockinfo* native;
} blockinfo;

#define EXECTHRESHOLD 5

extern blockinfo* block_new(void);
extern void block_clean(blockinfo*);
extern void block_delete(blockinfo*);
extern uint5 block_gather(machineinfo*, uint5);

#endif
