#ifndef RELOCATE_H
#define RELOCATE_H 1

#include "defs.h"
#include "list.h"

typedef enum {
  reloc_ABSOLUTE,
  reloc_RELATIVE
} reloc_type;

typedef enum {
  relsize_BYTE,
  relsize_HALFWORD,
  relsize_WORD
} reloc_size;

typedef struct {
  uint5 value;
  uint5 offset;
  reloc_size size;
  reloc_type type;
} reloc_record;

extern void relocate_add(list** to, uint5 value, uint5 offset, reloc_size size,
  reloc_type type);
extern void relocate_fix(list** to, void* base);

#endif
