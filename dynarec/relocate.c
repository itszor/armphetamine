#include <stdio.h>

#include "relocate.h"
#include "list.h"
#include "cnew.h"

void relocate_add(jt_list** to, uint5 value, uint5 offset, reloc_size size,
  reloc_type type)
{
  reloc_record* rec;
  jt_list_add(to);
  rec = (*to)->data = jt_new(reloc_record);
  rec->value = value;
  rec->offset = offset;
  rec->size = size;
  rec->type = type;
}

void relocate_fix(jt_list** to, void* base)
{
  while (*to)
  {
    reloc_record* rec = (*to)->data;
    
    fprintf(stderr, "Patching offset %x with %x\n", rec->offset, rec->value);
    
    switch (rec->size)
    {
      case relsize_BYTE:
      {
        switch (rec->type)
        {
          case reloc_ABSOLUTE:
          *(uint3*)((uint3*)base+rec->offset) =  rec->value;
          break;

          case reloc_RELATIVE:
          *(uint3*)((uint3*)base+rec->offset) = rec->value -
            (uint5)base - rec->offset - 1;
          break;
          
          default:
          break;
        }
      }
      break;
      
      case relsize_HALFWORD:
      {
        switch (rec->type)
        {
          case reloc_ABSOLUTE:
          *(uint4*)((uint3*)base+rec->offset) = rec->value;
          break;

          case reloc_RELATIVE:
          *(uint4*)((uint3*)base+rec->offset) = rec->value -
            (uint5)base - rec->offset - 2;
          break;
          
          default:
          break;
        }
      }
      break;

      case relsize_WORD:
      {
        switch (rec->type)
        {
          case reloc_ABSOLUTE:
          *(uint5*)((uint3*)base+rec->offset) = rec->value;
          break;

          case reloc_RELATIVE:
          *(uint5*)((uint3*)base+rec->offset) = rec->value -
            (uint5)base - rec->offset - 4;
          break;
          
          default:
          break;
        }
      }
      break;
    }
    
    jt_delete(rec);
    
    jt_list_removehead(to);
  }
}
