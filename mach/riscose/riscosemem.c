#include "defs.h"
#include "cnew.h"
#include "riscosemem.h"

const mem_readbank mem_rrom0 =
{
  memory_readbyterom0,
  memory_readhalfrom0,
  memory_readrom0
};

const mem_writebank mem_wbank0 =
{
  memory_writebytebank0,
  memory_writehalfbank0,
  memory_writewordbank0
};

const mem_readbank mem_rbank0 =
{
  memory_readbytebank0,
  memory_readhalfbank0,
  memory_readwordbank0
};

void memory_subinitialise(meminfo* mem)
{
  fprintf(stderr, "Initialising RISCOSE memory subsystem\n");
  /* let's have 16mb for our application */
  mem->bank0 = jt_newarray(uint5, 16384/4);
  /* and 1 mb of ROM */
  mem->rom0 = jt_newarray(uint5, 1024/4);
}

void memory_physicalmap(tlbentry* tlb, uint5 physaddress, uint3 readperm,
                        uint3 writeperm)
{
  if (physaddress < 0x8000)
  {
    tlb->read = mem_rrom0;
    tlb->write = mem_wnull;
  }
  else
  {
    tlb->read = mem_rbank0;
    tlb->write = mem_wbank0;
  }
}

uint5 memory_readphysicalword(meminfo* mem, uint5 physaddress)
{
  switch (physaddress >> 24)
  {
    case 0xc0: return mem->bank0[(physaddress & 0xffffff) >> 2];
    case 0xc1: return mem->bank1[(physaddress & 0xffffff) >> 2];
    case 0xc8: return mem->bank2[(physaddress & 0xffffff) >> 2];
    case 0xc9: return mem->bank3[(physaddress & 0xffffff) >> 2];
  }
  fprintf(stderr, "Bad physical word read by MMU at %.8x\n", physaddress);
  abort();

  return 0;
}

uint5 memory_readrom0(meminfo* mem, uint5 physaddress)
{
  return mem->rom0[(physaddress & 0xffffff) >> 2];
}

uint5 memory_readhalfrom0(meminfo* mem, uint5 physaddress)
{
  return ((uint4*)mem->rom0)[(physaddress & 0xffffff) >> 1];
}

uint5 memory_readbyterom0(meminfo* mem, uint5 physaddress)
{
  return ((uint3*)mem->rom0)[physaddress & 0xffffff];
}

#define BANKEDWRITER(W,B,T,S) \
  void memory_write##W##bank##B(meminfo* mem, uint5 physaddress, uint5 data) \
  { \
    if ((physaddress &= 0xffffff) < BANK##B##RAM) \
    ((T*)mem->bank##B)[physaddress >> (S)] = data; \
  }

BANKEDWRITER(word,0,uint5,2)
BANKEDWRITER(half,0,uint4,1)
BANKEDWRITER(byte,0,uint3,0)

#define BANKEDREADER(W,B,T,S) \
  uint5 memory_read##W##bank##B(meminfo* mem, uint5 physaddress) \
  { \
    if ((physaddress &= 0xffffff) < BANK##B##RAM) \
      return ((T*)mem->bank##B)[physaddress >> (S)]; \
    else \
      return 0; \
  }

BANKEDREADER(word,0,uint5,2)
BANKEDREADER(half,0,uint4,1)
BANKEDREADER(byte,0,uint3,0)
