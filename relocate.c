#include <stdio.h>

#include "relocate.h"
#include "list.h"
#include "cnew.h"

void relocate_add(list** to, uint5 value, uint5 offset, reloc_size size,
  reloc_type type)
{
  reloc_record* rec;
  list_add(to);
  rec = (*to)->data = cnew(reloc_record);
  rec->value = value;
  rec->offset = offset;
  rec->size = size;
  rec->type = type;
}

void relocate_fix(list** to, void* base)
{
  while (*to)
  {
    reloc_record* rec = (*to)->data;
    
    fprintf(stderr, "Patching offset %p with %x\n", rec->offset, rec->value);
    
    switch (rec->size)
    {
      case relsize_BYTE:
      if (rec->type==reloc_ABSOLUTE)
        *(uint3*)(base+rec->offset) = rec->value;
      else
        *(uint3*)(base+rec->offset) += rec->value;
      break;
      
      case relsize_HALFWORD:
      if (rec->type==reloc_ABSOLUTE)
        *(uint4*)(base+rec->offset) = rec->value;
      else
        *(uint4*)(base+rec->offset) += rec->value;
      break;
      
      case relsize_WORD:
      if (rec->type==reloc_ABSOLUTE)
        *(uint5*)(base+rec->offset) = rec->value;
      else
        *(uint5*)(base+rec->offset) += rec->value;
      break;
    }
    
    free(rec);
    
    list_removehead(to);
  }
}
