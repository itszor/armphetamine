#include <stdio.h>

#include "defs.h"
#include "cnew.h"
#include "memory.h"

meminfo* memory_initialise(uint5 bytes)
{
  meminfo* mem = cnew(meminfo);
  int i;

  mem->memory = cnewarray(int, bytes/4);
  mem->pagetable = cnew(level1pt);
  mem->writetag = 0;

  for (i=0; i<256; mem->pagetable->ptr[i++]=0);

  // start off with MMU disabled
  mem->mmuactive = 0;
	
//	fprintf(stderr, "Initialised memory, MMU inactive\n");

  return mem;
}

// I don't know if this is sensible.
void memory_setmapping(meminfo* mem, uint5 from, uint5 to)
{
  // blah
}

#undef memory_lookup
uint5* memory_lookup(meminfo* mem, uint5* virtualaddress)
{

  if (mem->mmuactive)   // non-flat address space
  {
    addressparts vaddr;
    level2pt* l2;
    level3pt* l3;
    uint5 page;
    vaddr.address = (uint5)virtualaddress;
    l2 = mem->pagetable->ptr[vaddr.part.l3];
    l3 = l2 ? l2->ptr[vaddr.part.l2] : 0;
    page = l3 ? l3->base[vaddr.part.l1] : -1;
    if (page==-1)
    {
      fprintf(stderr, "Page fault (unimplemented)\n");
      exit(1);
    }
    return (uint5*)((uint5)mem->memory+4096*page);
  }
  else  // flat address space
  {
    if ((uint5)virtualaddress>4096*1024)
    {
      mem->writetag = 1;
//      return (uint5*)((uint5)mem->screen->pixels+(uint5)virtualaddress
//                    -4096*1024);
      fprintf(stderr, "Bad memory access at %x\n", virtualaddress);
      abort();
    }
    if ((uint5)virtualaddress<0/* || (uint5)virtualaddress>4096*1024*/)
    {
      fprintf(stderr, "Bad memory access at %x\n", virtualaddress);
      abort();
    }
    return (uint5*)((uint5)mem->memory+(uint5)virtualaddress);
  }
}

#ifdef VIRTUALFRAMEBUFFER
// screen might need updating (efficiency, efficiency...)
void memory_postwrite(meminfo* mem, uint5* virtualaddress)
{
  uint5 v, x, y;
  v = (uint5)virtualaddress-4096*1024;
  x = v % 320, y = v / 320;
  if (mem->writetag)
  {
    SDL_UpdateRect(mem->screen, x, y, 4, 1);
    mem->writetag = 0;
  }
}
#else
void memory_postwrite(meminfo* mem, uint5* virtualaddress)
{
}
#endif
