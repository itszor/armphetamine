#include <assert.h>

#include "cnew.h"
#include "transmap.h"

transmap_entry* transmap_new(void)
{
  transmap_entry* entry = jt_new(transmap_entry);
  
  entry->code = 0;
  entry->length = 0;
  entry->usecount = 0;
  entry->context = 0;
  
  return entry;
}

void transmap_addentry(meminfo* mem, uint5 physaddr, transmap_entry* entry)
{
  uint5 page = physaddr >> 12;
  jt_hashentry* e;
  
  if (!mem->transmap[page])
  {
    mem->transmap[page] = jt_hash_new(32);
  }
  
  e = jt_hash_insert(mem->transmap[page], physaddr);
  e->data = entry;
}

transmap_entry* transmap_getentry(meminfo* mem, uint5 physaddr)
{
  uint5 page = physaddr >> 12;
  jt_hashentry* e = mem->transmap[page]
                   ? jt_hash_lookup(mem->transmap[page], physaddr)
                   : 0;
  
  if (!e) return 0;
  
  return (transmap_entry*) e->data;
}

void transmap_invalidatepage(meminfo* mem, uint5 page)
{
  IGNORE(mem);
  IGNORE(page);
}

void transmap_invalidateall(meminfo* mem)
{
  IGNORE(mem);
}

