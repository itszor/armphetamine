#include <assert.h>

#include "transmap.h"

void transmap_addentry(meminfo* mem, uint5 physaddr, transmap_entry* entry)
{
  uint5 page = physaddr >> 12;
  hashentry* e;
  
  if (!mem->transmap[page])
  {
    mem->transmap[page] = hash_new(32);
  }
  
  e = hash_insert(mem->transmap[page], physaddr);
  e->data = entry;
}

transmap_entry* transmap_getentry(meminfo* mem, uint5 physaddr)
{
  uint5 page = physaddr >> 12;
  hashentry* e = hash_lookup(mem->transmap[page], physaddr);
  
  if (!e) return 0;
  
  return (transmap_entry*) e->data;
}

void transmap_invalidatepage(meminfo* mem, uint5 page)
{
}

void transmap_invalidateall(meminfo* mem)
{
}
