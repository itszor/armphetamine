#ifndef BLOCK_H
// block.h must be included first; it re-includes this file
#include "block.h"
#else

#ifndef ANALYSE_H
#define ANALYSE_H 1

#include "defs.h"
#include "list.h"
#include "pseudo.h"
#include "codegen.h"
#include "allocate.h"

typedef enum {
  sec_RD,
	sec_RM,
	sec_RN,
	sec_RO,
	sec_C,
	sec_V,
	sec_N,
	sec_Z
} sectiontype;

// hashed by start
typedef struct {
  uint5 length;
  allocationinfo startalloc;
//	list* srcblks;  // places this can be called from
	list* destblks;   // places this can call
	sint5 offset;     // offset of generated inst from start of Intel code block
	list* patchback;  // list of x86 code offsets
} basicblockinfo;

#endif
#endif
