#ifndef MACHINE_H
#define MACHINE_H 1

#include "defs.h"
#include "processor.h"
#include "registers.h"
#include "memory.h"
#include "hash.h"

typedef struct {
	meminfo* mem;
	registerinfo* reg;
//	x86flaginfo x86flags;
	hashtable* blocks;
	hashentry* live;
	hashtable* translaterules;
} machineinfo;

// dependency problems otherwise
/*
#include "block.h"
#include "analyse.h"
*/

extern machineinfo* machine_create(uint5 memory);

#endif
