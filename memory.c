#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "cnew.h"
#include "memory.h"
#include "processor.h"
#include "vidc20.h"
#include "iomd.h"
#include "machine.h"

#ifdef EMULART
#  include "sapcm.h"
#  include "ostimer.h"
#endif

const mem_writebank mem_wfault =
{
  memory_writefault,
  memory_writefault,
  memory_writefault
};

const mem_readbank mem_rfault =
{
  memory_readfault,
  memory_readfault,
  memory_readfault
};

const mem_writebank mem_wnull =
{
  memory_nullwrite,
  memory_nullwrite,
  memory_nullwrite
};

const mem_readbank mem_rnull =
{
  memory_nullread,
  memory_nullread,
  memory_nullread
};

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

#ifdef IOMDSUPPORT
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
#endif

#ifdef VIDCSUPPORT
const mem_writebank mem_wvidc20 =
{
  vidc20_writebyte,
  vidc20_writeword,
  vidc20_writeword
};
#endif

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

#ifdef EMULART

const mem_readbank mem_sapcm_serial_read =
{
  sa1100_serial_read,
  sa1100_serial_read,
  sa1100_serial_read
};

const mem_writebank mem_sapcm_serial_write =
{
  sa1100_serial_write,
  sa1100_serial_write,
  sa1100_serial_write
};

const mem_readbank mem_sapcm_lcd_read =
{
  sa1100_lcd_read,
  sa1100_lcd_read,
  sa1100_lcd_read
};

const mem_writebank mem_sapcm_lcd_write =
{
  sa1100_lcd_write,
  sa1100_lcd_write,
  sa1100_lcd_write
};

const mem_readbank mem_ostimer_read =
{
  ostimer_read,
  ostimer_read,
  ostimer_read
};

const mem_writebank mem_ostimer_write =
{
  ostimer_write,
  ostimer_write,
  ostimer_write
};

#endif  /* EMULART */

meminfo* memory_initialise(uint5 bytes)
{
  meminfo* mem = cnew(meminfo);
  uint5 i;

  IGNORE(bytes);

/*  mem->memory = cnewarray(uint5, bytes/4);*/
  mem->bank0 = cnewarray(uint5, BANK0RAM/4);
  mem->bank1 = cnewarray(uint5, BANK1RAM/4);
  mem->bank2 = cnewarray(uint5, BANK2RAM/4);
  mem->bank3 = cnewarray(uint5, BANK3RAM/4);
  mem->rom0 = cnewarray(uint5, 4*1024*1024/4);
  mem->rom1 = cnewarray(uint5, 16*1024*1024/4); // hello, I'm flash
  mem->vram = VRAM ? cnewarray(uint5, VRAM/4) : 0;
  mem->currentmode = 0;
  mem->memoryfault = 0;
  memory_invalidatetlb(&mem->insttlb);
  memory_invalidatetlb(&mem->datatlb);

  // start off with MMU disabled
  mem->mmuactive = 0;

  // full 4Gb address-space worth of hashes
  fprintf(stderr, "Creating empty transmap\n");
  mem->transmap = cnewarray(hashtable*, 1<<20);
  for (i=0; i<1<<20; i++)
  {
    mem->transmap[i] = 0;
  }
	
#ifdef EMULART
  mem->sapcm.dma = cnewarray(sapcm_dma_channel, 6);
  mem->sapcm.serial_fifo = cnew(sapcm_serial_fifo);
  mem->sapcm.serial_fifo->in = fifo_create(12);
  mem->sapcm.serial_fifo->out = fifo_create(8);
  mem->ostimer = ostimer_new();
  mem->intctrl = intctrl_new();
#endif
  
//	fprintf(stderr, "Initialised memory, MMU inactive\n");

  mem->trace = fopen("trace.out", "w");

  return mem;
}

/* This DOES NOT fully copy the memory structure - complicated stuff like the
 * transmap is left alone!
 */
void memory_clone(meminfo* dest, meminfo* src)
{
  memcpy(dest->bank0, src->bank0, BANK0RAM);
  memcpy(dest->bank1, src->bank1, BANK1RAM);
  memcpy(dest->bank2, src->bank2, BANK2RAM);
  memcpy(dest->bank3, src->bank3, BANK3RAM);
  dest->currentmode = src->currentmode;
  dest->memoryfault = src->memoryfault;
  dest->mmuactive = src->mmuactive;
  memcpy(&dest->insttlb, &src->insttlb, sizeof(tlbentry));
  memcpy(&dest->datatlb, &src->datatlb, sizeof(tlbentry));
  dest->mmucontrol = src->mmucontrol;
  dest->translationbase = src->translationbase;
  dest->domainaccesscontrol = src->domainaccesscontrol;
  dest->faultstatus = src->faultstatus;
  dest->faultaddress = src->faultaddress;
}

static void bankchecker(uint5* a, uint5* b, uint5 size, uint5 bank)
{
  uint5 i;
  for (i=0; i<size; i++)
  {
    if (a[i] != b[i])
    {
      fprintf(stderr,
        "Memories differ at bank %d, offset %.8x. A=%.8x, B=%.8x\n",
        bank, i, a[i], b[i]);
      abort();
    }
  }
}

#define BOMB(A,B) if ((A) != (B)) { \
    fprintf(stderr, "%s != %s!\n", #A, #B); \
    abort(); \
  }

void memory_diff(meminfo* a, meminfo* b)
{
  bankchecker(a->bank0, b->bank0, BANK0RAM/4, 0);
  bankchecker(a->bank1, b->bank1, BANK1RAM/4, 1);
  bankchecker(a->bank2, b->bank2, BANK2RAM/4, 2);
  bankchecker(a->bank3, b->bank3, BANK3RAM/4, 3);
  BOMB(a->currentmode, b->currentmode);
  BOMB(a->memoryfault, b->memoryfault);
  BOMB(a->mmuactive, b->mmuactive);
/*  memcpy(&dest->insttlb, &src->insttlb, sizeof(tlbentry));
  memcpy(&dest->datatlb, &src->datatlb, sizeof(tlbentry));*/
  BOMB(a->mmucontrol, b->mmucontrol);
  BOMB(a->translationbase, b->translationbase);
  BOMB(a->domainaccesscontrol, b->domainaccesscontrol);
  BOMB(a->faultstatus, b->faultstatus);
  BOMB(a->faultaddress, b->faultaddress);
}

void memory_invalidatetlb(tlbentry* tlb)
{
  tlb->modestamp = -1;  // there isn't a mode -1
}

/* This wasn't sensible
void memory_setmapping(meminfo* mem, uint5 from, uint5 to)
{
  // blah
}
*/

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
  IGNORE(mem);
  IGNORE(virtualaddress);
}
#endif

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

// !!! wraparound at 4mb?
uint5 memory_readrom0(meminfo* mem, uint5 physaddress)
{
  return mem->rom0[(physaddress & 0x3fffff) >> 2];
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

void memory_writebytevram(meminfo* mem, uint5 physaddress, uint5 data)
{
#if VRAM>0
  if ((physaddress &= 0xffffff) < VRAM)
    ((uint3*)mem->vram)[physaddress] = data;
#else
  IGNORE(mem);
  IGNORE(physaddress);
  IGNORE(data);
#endif
}

void memory_writehalfvram(meminfo* mem, uint5 physaddress, uint5 data)
{
#if VRAM>0
  if ((physaddress &= 0xffffff) < VRAM)
    ((uint4*)mem->vram)[physaddress >> 1] = data;
#else
  IGNORE(mem);
  IGNORE(physaddress);
  IGNORE(data);
#endif
}

void memory_writevram(meminfo* mem, uint5 physaddress, uint5 data)
{
#if VRAM>0
  if ((physaddress &= 0xffffff) < VRAM)
    mem->vram[physaddress >> 2] = data;
#else
  IGNORE(mem);
  IGNORE(physaddress);
  IGNORE(data);
#endif
}

uint5 memory_readbytevram(meminfo* mem, uint5 physaddress)
{
#if VRAM>0
  if ((physaddress &= 0xffffff) < VRAM)
    return ((uint3*)mem->vram)[physaddress];
  else
    return 0;
#else
  IGNORE(mem);
  IGNORE(physaddress);
  return 0;
#endif
}

uint5 memory_readhalfvram(meminfo* mem, uint5 physaddress)
{
#if VRAM>0
  if ((physaddress &= 0xffffff) < VRAM)
    return ((uint4*)mem->vram)[physaddress >> 1];
  else
    return 0;
#else
  IGNORE(mem);
  IGNORE(physaddress);
  return 0;
#endif
}

uint5 memory_readvram(meminfo* mem, uint5 physaddress)
{
#if VRAM>0
  if ((physaddress &= 0xffffff) < VRAM)
    return mem->vram[physaddress >> 2];
  else
    return 0;
#else
  IGNORE(mem);
  IGNORE(physaddress);
  return 0;
#endif
}

void memory_writefault(meminfo* mem, uint5 physaddress, uint5 data)
{
  extern const char* modename_st[];
  
  IGNORE(data);
  
  fprintf(stderr, "Write fault at %x. Mode=%s\n", physaddress, 
    modename_st[mem->currentmode]);
  mem->memoryfault = 1;
 /* exit(1);*/
}

uint5 memory_readfault(meminfo* mem, uint5 physaddress)
{
  extern const char* modename_st[];
  fprintf(stderr, "Read fault at %x. Mode=%s\n", physaddress, 
    modename_st[mem->currentmode]);
  mem->memoryfault = 1;
/*  ((machineinfo*)mem->parent)->trace = 1;*/
 /* exit(1);*/
  return 0x0c0ffee0;
}

void memory_nullwrite(meminfo* mem, uint5 physaddress, uint5 data)
{
  IGNORE(mem);
  IGNORE(physaddress);
  IGNORE(data);
}

uint5 memory_nullread(meminfo* mem, uint5 physaddress)
{
  IGNORE(mem);
  IGNORE(physaddress);
  return 0x0c0ffee0;
}

#ifdef EMULART

void memory_physicalmap(tlbentry* tlb, uint5 physaddress, uint3 readperm,
                        uint3 writeperm)
{
  switch (physaddress >> 24)
  {
    case 0x00: // ROM bank 0
    tlb->read = readperm ? mem_rrom0 : mem_rfault;
    tlb->write = writeperm ? mem_wnull : mem_wfault;
    break;
    
    case 0x10: // ROM bank 1
    tlb->read = readperm ? mem_rrom1 : mem_rfault;
    tlb->write = writeperm ? mem_wnull : mem_wfault;
    break;

    case 0x80: // SA1100 serial registers
    tlb->read = readperm ? mem_sapcm_serial_read : mem_rfault;
    tlb->write = writeperm ? mem_sapcm_serial_write : mem_wfault;
    break;
    
    case 0x90: // OS timer registers
    tlb->read = readperm ? mem_ostimer_read : mem_rfault;
    tlb->write = writeperm ? mem_ostimer_write : mem_wfault;
    break;
    
    case 0xa0: // memory control registers
    tlb->read = readperm ? mem_rnull : mem_rfault;
    tlb->write = writeperm ? mem_wnull : mem_wfault;
    break;

    case 0xb0: // SA1100 LCD registers
    tlb->read = readperm ? mem_sapcm_lcd_read : mem_rfault;
    tlb->write = writeperm ? mem_sapcm_lcd_write : mem_wfault;
    break;
  
    case 0xc0: // DRAM bank 0 (8Mb)
    tlb->read = readperm ? mem_rbank0 : mem_rfault;
    tlb->write = writeperm ? mem_wbank0 : mem_wfault;
    break;

    case 0xc1: // DRAM bank 1 (8Mb)
    tlb->read = readperm ? mem_rbank1 : mem_rfault;
    tlb->write = writeperm ? mem_wbank1 : mem_wfault;
    break;

    case 0xc8: // DRAM bank 2 (8Mb)
    tlb->read = readperm ? mem_rbank2 : mem_rfault;
    tlb->write = writeperm ? mem_wbank2 : mem_wfault;
    break;

    case 0xc9: // DRAM bank 3 (8Mb)
    tlb->read = readperm ? mem_rbank3 : mem_rfault;
    tlb->write = writeperm ? mem_wbank3 : mem_wfault;
    break;

    default:
//    fprintf(stderr, "Bad physical address %.8x\n", physaddress);
    break;
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

#else

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

#endif  /* EMULART */

uint5 memory_virtualtophysical(meminfo* mem, uint5 virtualaddress,
                               tlbentry* tlb)
{
  uint5 tableindex, firstleveldescriptor, domain;
  uint3 isuser = (mem->currentmode==0);
  // bits go: aprsvd
  static const uint3 apfault[] = {
    0, 0, 0, 0,
    1, 0, 1, 0,
    0, 0, 1, 0,
    0, 0, 0, 0,
    
    0, 0, 1, 1,
    0, 0, 1, 1,
    0, 0, 1, 1,
    0, 0, 0, 0,
    
    1, 0, 1, 1,
    1, 0, 1, 1,
    1, 0, 1, 1,
    0, 0, 0, 0,
    
    1, 1, 1, 1,
    1, 1, 1, 1,
    1, 1, 1, 1,
    0, 0, 0, 0
  };

  if (!(mem->mmucontrol & 1)) {
    tlb->virtual = tlb->physical = virtualaddress & 0xff000000;
    tlb->mask = 0xff000000;  // can't use whole memory (I/O etc.)
    memory_physicalmap(tlb, virtualaddress, 1, 1);
    tlb->modestamp = mem->currentmode;
    return virtualaddress;
  }

  if ((mem->mmucontrol & 2) && (virtualaddress & 3)) {
//    fprintf(stderr, "Alignment fault!\n");
    mem->memoryfault = 1;
  }

  tableindex = virtualaddress >> 20;
  firstleveldescriptor = memory_readphysicalword(mem,
    mem->translationbase | (tableindex << 2));
  domain = (firstleveldescriptor >> 5) & 0xf;

  switch (firstleveldescriptor & 3)
  {
    case 0:  // fault
    case 3:
    {
//      fprintf(stderr, "Section translation fault\n");
      mem->memoryfault = 1;
    }
    break;

    case 1:  // page
    {
      uint5 l2tableindex = (virtualaddress >> 12) & 0xff;
      uint5 pagetablebaseaddress = firstleveldescriptor >> 10;  
      uint5 secondleveldescriptor = memory_readphysicalword(mem, 
        (pagetablebaseaddress << 10) | (l2tableindex << 2));
      uint5 domainaccess = (mem->domainaccesscontrol >> (domain << 1)) & 0x3;

      if (domainaccess==0 || domainaccess==2)
      {
//        fprintf(stderr, "Page domain fault!\n");
        mem->memoryfault = 1;
      }

      switch (secondleveldescriptor & 3)  
      {  
        case 0:  // page translation fault
        case 3:
        {
//          fprintf(stderr, "Page translation fault!\n");
          mem->memoryfault = 1;
        }
        break;  

        case 1:  // large page
        {
          uint5 pageindex = 0, physaddress;
          uint3 fullpage = 0;

          if (domainaccess==1)  // client
          {
            uint5 rsbits = (mem->mmucontrol >> 8) & 3;
            uint5 subpage = (pageindex >> 10) & 3;
            uint3 ap = (secondleveldescriptor >> (2*subpage+4)) & 3;
            uint3 ap0 = (secondleveldescriptor >> 4) & 3;
            uint3 ap1 = (secondleveldescriptor >> 6) & 3;
            uint3 ap2 = (secondleveldescriptor >> 8) & 3;
            uint3 ap3 = (secondleveldescriptor >> 10) & 3;
            uint5 fltbase = (ap<<4) | (rsbits<<2);

            if (ap0==ap1 && ap1==ap2 && ap2==ap3) fullpage = 1;

            if (!isuser) fltbase += 2;

            memory_physicalmap(tlb, secondleveldescriptor,
              apfault[fltbase+0], apfault[fltbase+1]);
          }
          else  // manager, no permission checking
          {
            memory_physicalmap(tlb, secondleveldescriptor, 1, 1);
            fullpage = 1;
          }
          pageindex = virtualaddress & 0xffff;
          physaddress = (secondleveldescriptor & 0xffff0000) | pageindex;
          if (fullpage)  // valid for entire 64k
          {
            tlb->mask = 0xffff0000;
            tlb->physical = physaddress & 0xffff0000;
            tlb->virtual = virtualaddress & 0xffff0000;
          }
          else  // valid for 16k only
          {
            tlb->mask = 0xffffc000;
            tlb->physical = physaddress & 0xffffc000;
            tlb->virtual = virtualaddress & 0xffffc000;
          }
          tlb->modestamp = mem->currentmode;
          return physaddress;
        }
        break;

        case 2:  // small page
        {  
          uint5 pageindex = 0, physaddress;
          uint3 fullpage = 0;
          
          if (domainaccess==1)  // client
          {
            uint5 rsbits = (mem->mmucontrol >> 8) & 3;
            uint5 subpage = (pageindex >> 14) & 3;
            uint5 ap = (secondleveldescriptor >> (2*subpage+4)) & 3;
            uint3 ap0 = (secondleveldescriptor >> 4) & 3;
            uint3 ap1 = (secondleveldescriptor >> 6) & 3;
            uint3 ap2 = (secondleveldescriptor >> 8) & 3;
            uint3 ap3 = (secondleveldescriptor >> 10) & 3;
            uint5 fltbase = (ap<<4) | (rsbits<<2);

            if (ap0==ap1 && ap1==ap2 && ap2==ap3) fullpage = 1;

            if (!isuser) fltbase += 2;

            memory_physicalmap(tlb, secondleveldescriptor,
              apfault[fltbase+0], apfault[fltbase+1]);
          }
          else  // manager, no permission checking
          {
            memory_physicalmap(tlb, secondleveldescriptor, 1, 1);
            fullpage = 1;
          }
          pageindex = virtualaddress & 0xfff;  
          physaddress = (secondleveldescriptor & 0xfffff000) | pageindex;

          if (fullpage)  // valid for full 4k
          {
            tlb->mask = 0xfffff000;
            tlb->physical = physaddress & 0xfffff000;
            tlb->virtual = virtualaddress & 0xfffff000;
          }
          else  // valid for 1k only
          {
            tlb->mask = 0xfffffc00;
            tlb->physical = physaddress & 0xfffffc00;
            tlb->virtual = virtualaddress & 0xfffffc00;
          }
          tlb->modestamp = mem->currentmode;
          return physaddress;
        }
        break;
      }
    }
    break;

    case 2:  // section
    {
      uint5 sectionindex, physaddress;
      uint5 domainaccess = (mem->domainaccesscontrol >> (domain << 1)) & 0x3;

      if (domainaccess==0 || domainaccess==2)
      {
        // fprintf(stderr, "Section domain fault\n");
        mem->memoryfault = 1;
      }
      
      if (domainaccess==1)
      {
        uint5 rsbits = (mem->mmucontrol >> 8) & 3;
        uint5 ap = (firstleveldescriptor >> 10) & 3;
        uint5 fltbase = (ap<<4) | (rsbits<<2);

        if (!isuser) fltbase += 2;

        memory_physicalmap(tlb, firstleveldescriptor,
          apfault[fltbase+0], apfault[fltbase+1]);
      }
      else
      {
        memory_physicalmap(tlb, firstleveldescriptor, 1, 1);
      }

      sectionindex = virtualaddress & 0x000fffff;
      physaddress = (firstleveldescriptor & 0xfff00000) | sectionindex;
      tlb->mask = 0xfff00000;
      tlb->physical = physaddress & 0xfff00000;
      tlb->virtual = virtualaddress & 0xfff00000;
      tlb->modestamp = mem->currentmode;
      return physaddress;
    }
    break;
  }
  return -1;  // shouldn't happen!
}

uint5 memory_readdataword(meminfo* mem, uint5 virtualaddress)
{
  uint5 physaddress, data;
  if (mem->datatlb.modestamp != mem->currentmode ||
      (virtualaddress & mem->datatlb.mask) != mem->datatlb.virtual)
  {
    physaddress = memory_virtualtophysical(mem, virtualaddress, &mem->datatlb);
  }
  else
  {
    physaddress = mem->datatlb.physical |
                  (virtualaddress & ~mem->datatlb.mask);
  }
  data = mem->datatlb.read.word(mem, physaddress);
/*  fprintf(stderr, "Read data word %.8x from virtual addr %.8x\n", 
    data, virtualaddress);*/
  return data;
}

uint5 memory_readinstword(meminfo* mem, uint5 virtualaddress)
{
  uint5 physaddress;
  if (mem->insttlb.modestamp != mem->currentmode ||
      (virtualaddress & mem->insttlb.mask) != mem->insttlb.virtual)
  {
    physaddress = memory_virtualtophysical(mem, virtualaddress, &mem->insttlb);
  }
  else
  {
    physaddress = mem->insttlb.physical |
                  (virtualaddress & ~mem->insttlb.mask);
  }
  return mem->insttlb.read.word(mem, physaddress);
}

void memory_writeword(meminfo* mem, uint5 virtualaddress, uint5 data)
{
  uint5 physaddress;
/*  fprintf(stderr, "Write data word %.8x to virtual addr %.8x\n", data, 
          virtualaddress);*/
  if (mem->datatlb.modestamp != mem->currentmode ||
      (virtualaddress & mem->datatlb.mask) != mem->datatlb.virtual)
  {
    physaddress = memory_virtualtophysical(mem, virtualaddress, &mem->datatlb);
  }
  else
  {
    physaddress = mem->datatlb.physical |
                  (virtualaddress & ~mem->datatlb.mask);
  }
  mem->datatlb.write.word(mem, physaddress, data);
}

uint5 memory_readbyte(meminfo* mem, uint5 virtualaddress)
{
  uint5 physaddress;
  if (mem->datatlb.modestamp != mem->currentmode ||
      (virtualaddress & mem->datatlb.mask) != mem->datatlb.virtual)
  {
    physaddress = memory_virtualtophysical(mem, virtualaddress, &mem->datatlb);
  }
  else
  {
    physaddress = mem->datatlb.physical |
                  (virtualaddress & ~mem->datatlb.mask);
  }
  return mem->datatlb.read.byte(mem, physaddress);
}

uint5 memory_readhalf(meminfo* mem, uint5 virtualaddress)
{
  uint5 physaddress;
  if (mem->datatlb.modestamp != mem->currentmode ||
      (virtualaddress & mem->datatlb.mask) != mem->datatlb.virtual)
  {
    physaddress = memory_virtualtophysical(mem, virtualaddress, &mem->datatlb);
  }
  else
  {
    physaddress = mem->datatlb.physical |
                  (virtualaddress & ~mem->datatlb.mask);
  }
  return mem->datatlb.read.half(mem, physaddress);
}

uint5 memory_readsbyte(meminfo* mem, uint5 virtualaddress)
{
  uint5 physaddress;
  uint5 data;
  if (mem->datatlb.modestamp != mem->currentmode ||
      (virtualaddress & mem->datatlb.mask) != mem->datatlb.virtual)
  {
    physaddress = memory_virtualtophysical(mem, virtualaddress, &mem->datatlb);
  }
  else
  {
    physaddress = mem->datatlb.physical |
                  (virtualaddress & ~mem->datatlb.mask);
  }
  data = mem->datatlb.read.byte(mem, physaddress);
  if (data & 0x80) data |= 0xffffff00;
  return data;
}

uint5 memory_readshalf(meminfo* mem, uint5 virtualaddress)
{
  uint5 physaddress;
  uint5 data;
  if (mem->datatlb.modestamp != mem->currentmode ||
      (virtualaddress & mem->datatlb.mask) != mem->datatlb.virtual)
  {
    physaddress = memory_virtualtophysical(mem, virtualaddress, &mem->datatlb);
  }
  else
  {
    physaddress = mem->datatlb.physical |
                  (virtualaddress & ~mem->datatlb.mask);
  }
  data = mem->datatlb.read.half(mem, physaddress);
  if (data & 0x8000) data |= 0xffff0000;
  return data;
}

void memory_writebyte(meminfo* mem, uint5 virtualaddress, uint5 data)
{
  uint5 physaddress;
  if (mem->datatlb.modestamp != mem->currentmode ||
      (virtualaddress & mem->datatlb.mask) != mem->datatlb.virtual)
  {
    physaddress = memory_virtualtophysical(mem, virtualaddress, &mem->datatlb);
  }
  else
  {
    physaddress = mem->datatlb.physical |
                  (virtualaddress & ~mem->datatlb.mask);
  }
  mem->datatlb.write.byte(mem, physaddress, data);
}

void memory_writehalf(meminfo* mem, uint5 virtualaddress, uint5 data)
{
  uint5 physaddress;
  if (mem->datatlb.modestamp != mem->currentmode ||
      (virtualaddress & mem->datatlb.mask) != mem->datatlb.virtual)
  {
    physaddress = memory_virtualtophysical(mem, virtualaddress, &mem->datatlb);
  }
  else
  {
    physaddress = mem->datatlb.physical |
                  (virtualaddress & ~mem->datatlb.mask);
  }
  mem->datatlb.write.half(mem, physaddress, data);
}

