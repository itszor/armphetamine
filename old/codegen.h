#ifndef ALLOCATE_H
#include "allocate.h"
#else

#ifndef CODEGEN_H
#define CODEGEN_H 1

#include "defs.h"
#include "hash.h"
#include "machine.h"
#include "x86asm.h"

// the edge table is hashed on this (destination pointer holds another)
typedef union {
  struct {
	  sint5 address : 29;
		uint5 section : 3;
	} parts;
	sint5 value;
} endpoint;

/*typedef enum {
  in_UNUSED,
	in_REGBLK,   // currently spilled to register block
	in_IMM,
	in_FLAG,
  in_REG       // (+regfield) currently in a register
} argtype;*/
/*
typedef union {
  struct {
		uint5 rule   : 23;  // rule number
	  uint5 regx   : 3;
		uint5 regy   : 3;
		uint5 regz   : 3;
	} parts;
	uint5 value;
} rulepattern;
*/
// These are going to be so numerous the prefix is omitted for clarity
typedef enum {
	DONE,
	IGNORE,
  IMM,
	REGX,
	REGY,
	REGZ,
	REGT,
	REGU,
	REGV,
	CODEGEN
} regclass;

// arbitrary 'high' set bit, must be ORed with operands in pattern tree
#define OP (1U<<16)

typedef struct {
//	uint5 ptr;             // offset from start of x86 block
//	uint5 lastblockstart;  // the last place a block was started
	matchstate mstate;
} gencontext;

// I love the preprocessor, oh yes
#define GENFNARGS (machineinfo* machine, nativeblockinfo* native, \
                   hashtable* basic, void* info, allocationinfo* alloc, \
                   matchstate* mstate, uint5 lock, uint5 flagsset)

typedef void (*gen_fn) GENFNARGS;

#define GENFN(X) void r_##X GENFNARGS

// node is hashed on content type
/* DEPRECATED
typedef struct {
  hashtable* branch;
//  gen_fn rule;
} x86node;*/

// 'imapinfo' is defined by analyse.h, which needs gen_fn defined...
/*struct imapinfo;

// information about a block of x86 code (possibly garbage)
typedef struct {
  uint3* base;
	uint5 length;
	uint5 size;
	struct imapinfo* meta;
} iblock;*/

#endif
#endif
