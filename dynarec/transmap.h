#ifndef TRANSMAP_H
#define TRANSMAP_H 1

#include "libjtype/defs.h"
#include "libjtype/hash.h"
#include "dynarec/pheta.h"

typedef struct {
  uint3* code;
  uint5 length;
  uint5 usecount;
  uint5 context;
} transmap_entry;

extern transmap_entry* transmap_new(void);

extern void transmap_addentry(meminfo* mem, uint5 physaddr,
  transmap_entry* entry);
  
extern transmap_entry* transmap_getentry(meminfo* mem, uint5 physaddr);

extern void transmap_invalidatepage(meminfo* mem, uint5 page);

extern void transmap_invalidateall(meminfo* mem);

#endif
