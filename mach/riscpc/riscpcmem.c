#include "defs.h"
#include "cnew.h"
#include "memory.h"
#include "processor.h"
#include "machine.h"
#include "riscpcmem.h"
#include "vidc20.h"
#include "iomd.h"

const mem_readbank mem_rrom0 =
{
  memory_readbyterom0,
  memory_readhalfrom0,
  memory_readrom0
};

const mem_readbank mem_rrom1 =
{
  memory_readbyterom1,
  memory_readhalfrom1,
  memory_readrom1
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

const mem_writebank mem_wbank1 =
{
  memory_writebytebank1,
  memory_writehalfbank1,
  memory_writewordbank1
};

const mem_readbank mem_rbank1 =
{
  memory_readbytebank1,
  memory_readhalfbank1,
  memory_readwordbank1
};

const mem_writebank mem_wbank2 =
{
  memory_writebytebank2,
  memory_writehalfbank2,
  memory_writewordbank2
};

const mem_readbank mem_rbank2 =
{
  memory_readbytebank2,
  memory_readhalfbank2,
  memory_readwordbank2
};

const mem_writebank mem_wbank3 =
{
  memory_writebytebank3,
  memory_writehalfbank3,
  memory_writewordbank3
};

const mem_readbank mem_rbank3 =
{
  memory_readbytebank3,
  memory_readhalfbank3,
  memory_readwordbank3
};

const mem_writebank mem_wvram =
{
  memory_writebytevram,
  memory_writehalfvram,
  memory_writevram
};

const mem_readbank mem_rvram =
{
  memory_readbytevram,
  memory_readhalfvram,
  memory_readvram
};

const mem_writebank mem_wiomd =
{
  iomd_writeword,
  iomd_writeword,
  iomd_writeword
};

const mem_readbank mem_riomd =
{
  iomd_readword,
  iomd_readword,
  iomd_readword
};

const mem_writebank mem_wvidc20 =
{
  vidc20_writebyte,
  vidc20_writeword,
  vidc20_writeword
};

void memory_subinitialise(meminfo* mem)
{
  fprintf(stderr, "Initialising RISC PC memory subsystem\n");
  mem->bank0 = cnewarray(uint5, BANK0RAM/4);
  mem->bank1 = cnewarray(uint5, BANK1RAM/4);
  mem->bank2 = cnewarray(uint5, BANK2RAM/4);
  mem->bank3 = cnewarray(uint5, BANK3RAM/4);
  mem->rom0 = cnewarray(uint5, 16*1024*1024/4);
  mem->rom1 = cnewarray(uint5, 16*1024*1024/4); // hello, I'm flash
}

void memory_physicalmap(tlbentry* tlb, uint5 physaddress, uint3 readperm,
                        uint3 writeperm)
{
  switch ((physaddress >> 24) & 0x1f)
  {
    case 0x00: // ROM bank 0
    tlb->read = readperm ? mem_rrom0 : mem_rfault;
    tlb->write = writeperm ? mem_wnull : mem_wfault;
    break;

    case 0x01: // ROM bank 1
    tlb->read = readperm ? mem_rrom1 : mem_rfault;
    tlb->write = writeperm ? mem_wnull : mem_wfault;
    break;

    case 0x02: // VRAM
    tlb->read = readperm ? mem_rvram : mem_rfault;
    tlb->write = writeperm ? mem_wvram : mem_wfault;
    break;

    case 0x03: // module I/O space
    switch ((physaddress >> 20) & 0xf)
    {
      case 0x2:  // iomd registers
      tlb->read = readperm ? mem_riomd : mem_rfault;
      tlb->write = writeperm ? mem_wiomd : mem_wfault;
      break;
      
      case 0x4:  // video registers
      tlb->read = readperm ? mem_rnull : mem_rfault;
      tlb->write = writeperm ? mem_wvidc20 : mem_wfault;
      break;
      
      default:
      tlb->read = mem_rnull;
      tlb->write = mem_wnull;
    }
    break;

    case 0x08: // extended I/O space
    case 0x09:
    case 0x0a:
    case 0x0b:
    case 0x0c:
    case 0x0d:
    case 0x0e:
    case 0x0f:
    tlb->read = mem_rnull;
    tlb->write = mem_wnull;
    break;

    case 0x10: // DRAM bank 0 (64Mb)
    case 0x11:
    case 0x12:
    case 0x13:
    tlb->read = readperm ? mem_rbank0 : mem_rfault;
    tlb->write = writeperm ? mem_wbank0 : mem_wfault;
    break;

    case 0x14: // DRAM bank 1 (64Mb)
    case 0x15:
    case 0x16:
    case 0x17:
    tlb->read = readperm ? mem_rbank1 : mem_rfault;
    tlb->write = writeperm ? mem_wbank1 : mem_wfault;
    break;

    case 0x18: // DRAM bank 2 (64Mb)
    case 0x19:
    case 0x1a:
    case 0x1b:
    tlb->read = readperm ? mem_rbank2 : mem_rfault;
    tlb->write = writeperm ? mem_wbank2 : mem_wfault;
    break;

    case 0x1c: // DRAM bank 3 (64Mb)
    case 0x1d:
    case 0x1e:
    case 0x1f:
    tlb->read = readperm ? mem_rbank3 : mem_rfault;
    tlb->write = writeperm ? mem_wbank3 : mem_wfault;
    break;

    default:
    break;
  }
}

uint5 memory_readphysicalword(meminfo* mem, uint5 physaddress)
{
  switch (physaddress >> 26)
  {
    case 4: return mem->bank0[(physaddress & 0xffffff) >> 2];
    case 5: return mem->bank1[(physaddress & 0xffffff) >> 2];
    case 6: return mem->bank2[(physaddress & 0xffffff) >> 2];
    case 7: return mem->bank3[(physaddress & 0xffffff) >> 2];
  }
  return 0;
}

#define BANKEDWRITER(W,B,T,S) \
  void memory_write##W##bank##B(meminfo* mem, uint5 physaddress, uint5 data) \
  { \
    if ((physaddress &= 0xffffff) < BANK##B##RAM) \
    ((T*)mem->bank##B)[physaddress >> (S)] = data; \
  }

BANKEDWRITER(word,0,uint5,2)
BANKEDWRITER(word,1,uint5,2)
BANKEDWRITER(word,2,uint5,2)
BANKEDWRITER(word,3,uint5,2)

BANKEDWRITER(half,0,uint4,1)
BANKEDWRITER(half,1,uint4,1)
BANKEDWRITER(half,2,uint4,1)
BANKEDWRITER(half,3,uint4,1)

BANKEDWRITER(byte,0,uint3,0)
BANKEDWRITER(byte,1,uint3,0)
BANKEDWRITER(byte,2,uint3,0)
BANKEDWRITER(byte,3,uint3,0)

#define BANKEDREADER(W,B,T,S) \
  uint5 memory_read##W##bank##B(meminfo* mem, uint5 physaddress) \
  { \
    if ((physaddress &= 0xffffff) < BANK##B##RAM) \
      return ((T*)mem->bank##B)[physaddress >> (S)]; \
    else \
      return 0; \
  }

BANKEDREADER(word,0,uint5,2)
BANKEDREADER(word,1,uint5,2)
BANKEDREADER(word,2,uint5,2)
BANKEDREADER(word,3,uint5,2)

BANKEDREADER(half,0,uint4,1)
BANKEDREADER(half,1,uint4,1)
BANKEDREADER(half,2,uint4,1)
BANKEDREADER(half,3,uint4,1)

BANKEDREADER(byte,0,uint3,0)
BANKEDREADER(byte,1,uint3,0)
BANKEDREADER(byte,2,uint3,0)
BANKEDREADER(byte,3,uint3,0)

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

uint5 memory_readrom1(meminfo* mem, uint5 physaddress)
{
  return mem->rom1[(physaddress & 0xffffff) >> 2];
}

uint5 memory_readhalfrom1(meminfo* mem, uint5 physaddress)
{
  return ((uint4*)mem->rom1)[(physaddress & 0xffffff) >> 1];
}

uint5 memory_readbyterom1(meminfo* mem, uint5 physaddress)
{
  return ((uint3*)mem->rom1)[physaddress & 0xffffff];
}
