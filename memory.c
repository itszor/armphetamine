#include <stdio.h>

#include "defs.h"
#include "cnew.h"
#include "memory.h"
#include "processor.h"
#include "vidc20.h"
#include "iomd.h"

meminfo* memory_initialise(uint5 bytes)
{
  meminfo* mem = cnew(meminfo);

  mem->memory = cnewarray(int, bytes/4);
  mem->bank0 = cnewarray(int, BANK0RAM/4);
  mem->bank1 = cnewarray(int, BANK1RAM/4);
  mem->bank2 = cnewarray(int, BANK2RAM/4);
  mem->bank3 = cnewarray(int, BANK3RAM/4);
  mem->rom0 = cnewarray(int, 4*1024*1024/4);
  mem->rom1 = 0; // cnewarray(int, 16*1024*1024/4);
  mem->vram = cnewarray(int, VRAM/4);
  mem->writetag = 0;
  mem->currentmode = 0;
  mem->memoryfault = 0;
  memory_invalidatetlb(&mem->insttlb);
  memory_invalidatetlb(&mem->datatlb);

  // start off with MMU disabled
  mem->mmuactive = 0;
	
//	fprintf(stderr, "Initialised memory, MMU inactive\n");

  return mem;
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
}
#endif

void memory_writebank0(meminfo* mem, uint5 physaddress, uint5 data)
{
  if ((physaddress &= 0xffffff) < BANK0RAM)
    mem->bank0[physaddress >> 2] = data;
}

void memory_writebytebank0(meminfo* mem, uint5 physaddress, uint5 data)
{
  if ((physaddress &= 0xffffff) < BANK0RAM)
    ((uint3*)mem->bank0)[physaddress] = data;
}

uint5 memory_readbank0(meminfo* mem, uint5 physaddress)
{
  if ((physaddress &= 0xffffff) < BANK0RAM)
    return mem->bank0[physaddress >> 2];
  else
    return 0;
}

uint5 memory_readbytebank0(meminfo* mem, uint5 physaddress)
{
  if ((physaddress &= 0xffffff) < BANK0RAM)
    return ((uint3*)mem->bank0)[physaddress];
  else
    return 0;
}

void memory_writebank1(meminfo* mem, uint5 physaddress, uint5 data)
{
  if ((physaddress &= 0xffffff) < BANK1RAM)
    mem->bank1[physaddress >> 2] = data;
}

void memory_writebytebank1(meminfo* mem, uint5 physaddress, uint5 data)
{
  if ((physaddress &= 0xffffff) < BANK1RAM)
    ((uint3*)mem->bank1)[physaddress] = data;
}

uint5 memory_readbank1(meminfo* mem, uint5 physaddress)
{
  if ((physaddress &= 0xffffff) < BANK1RAM)
    return mem->bank1[physaddress >> 2];
  else
    return 0;
}

uint5 memory_readbytebank1(meminfo* mem, uint5 physaddress)
{
  if ((physaddress &= 0xffffff) < BANK1RAM)
    return ((uint3*)mem->bank1)[physaddress];
  else
    return 0;
}

void memory_writebank2(meminfo* mem, uint5 physaddress, uint5 data)
{
  if ((physaddress &= 0xffffff) < BANK2RAM)
    mem->bank2[physaddress >> 2] = data;
}

void memory_writebytebank2(meminfo* mem, uint5 physaddress, uint5 data)
{
  if ((physaddress &= 0xffffff) < BANK2RAM)
    ((uint3*)mem->bank2)[physaddress] = data;
}

uint5 memory_readbank2(meminfo* mem, uint5 physaddress)
{
  if ((physaddress &= 0xffffff) < BANK2RAM)
    return mem->bank2[physaddress >> 2];
  else
    return 0;
}

uint5 memory_readbytebank2(meminfo* mem, uint5 physaddress)
{
  if ((physaddress &= 0xffffff) < BANK2RAM)
    return ((uint3*)mem->bank2)[physaddress];
  else
    return 0;
}

void memory_writebank3(meminfo* mem, uint5 physaddress, uint5 data)
{
  if ((physaddress &= 0xffffff) < BANK3RAM)
    mem->bank3[physaddress >> 2] = data;
}

void memory_writebytebank3(meminfo* mem, uint5 physaddress, uint5 data)
{
  if ((physaddress &= 0xffffff) < BANK3RAM)
    ((uint3*)mem->bank3)[physaddress] = data;
}

uint5 memory_readbank3(meminfo* mem, uint5 physaddress)
{
  if ((physaddress &= 0xffffff) < BANK3RAM)
    return mem->bank3[physaddress >> 2];
  else
    return 0;
}

uint5 memory_readbytebank3(meminfo* mem, uint5 physaddress)
{
  if ((physaddress &= 0xffffff) < BANK3RAM)
    return ((uint3*)mem->bank3)[physaddress];
  else
    return 0;
}

// !!! wraparound at 4mb?
uint5 memory_readrom0(meminfo* mem, uint5 physaddress)
{
  return mem->rom0[(physaddress & 0x3fffff) >> 2];
}

uint5 memory_readbyterom0(meminfo* mem, uint5 physaddress)
{
  return ((uint3*)mem->rom0)[physaddress & 0xffffff];
}

uint5 memory_readrom1(meminfo* mem, uint5 physaddress)
{
  return mem->rom1[(physaddress & 0xffffff) >> 2];
}

uint5 memory_readbyterom1(meminfo* mem, uint5 physaddress)
{
  return ((uint3*)mem->rom1)[physaddress & 0xffffff];
}

void memory_writebytevram(meminfo* mem, uint5 physaddress, uint5 data)
{
  if ((physaddress &= 0xffffff) < VRAM)
    ((uint3*)mem->vram)[physaddress] = data;
}

void memory_writevram(meminfo* mem, uint5 physaddress, uint5 data)
{
  if ((physaddress &= 0xffffff) < VRAM)
    mem->vram[physaddress >> 2] = data;
}

uint5 memory_readbytevram(meminfo* mem, uint5 physaddress)
{
  if ((physaddress &= 0xffffff) < VRAM)
    return ((uint3*)mem->vram)[physaddress];
  else
    return 0;
}

uint5 memory_readvram(meminfo* mem, uint5 physaddress)
{
  if ((physaddress &= 0xffffff) < VRAM)
    return mem->vram[physaddress >> 2];
  else
    return 0;
}

void memory_writefault(meminfo* mem, uint5 physaddress, uint5 data)
{
  extern const char* modename_st[];
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
 /* exit(1);*/
  return 0;
}

void memory_nullwrite(meminfo* mem, uint5 physaddress, uint5 data)
{
}

uint5 memory_nullread(meminfo* mem, uint5 physaddress)
{
  return 0x0c0ffee0;
}

void memory_physicalmap(tlbentry* tlb, uint5 physaddress, uint3 writeperm,
                        uint3 readperm)
{
  switch ((physaddress >> 24) & 0x1f)
  {
    case 0x00: // ROM bank 0
    tlb->readbyte = readperm ? memory_readbyterom0 : memory_readfault;
    tlb->readword = readperm ? memory_readrom0 : memory_readfault;
    tlb->writebyte = tlb->writeword =
      writeperm ? memory_nullwrite : memory_writefault;
    break;

    case 0x01: // ROM bank 1
    tlb->readbyte = readperm ? memory_readbyterom1 : memory_readfault;
    tlb->readword = readperm ? memory_readrom1 : memory_readfault;
    tlb->writebyte = tlb->writeword =
      writeperm ? memory_nullwrite : memory_writefault;
    break;

    case 0x02: // VRAM
    tlb->readbyte = readperm ? memory_readbytevram : memory_readfault;
    tlb->readword = readperm ? memory_readvram : memory_readfault;
    tlb->writebyte = writeperm ? memory_writebytevram : memory_writefault;
    tlb->writeword = writeperm ? memory_writevram : memory_writefault;
    break;

    case 0x03: // module I/O space
    switch ((physaddress >> 20) & 0xf)
    {
      case 0x2:  // iomd registers
      tlb->readbyte = readperm ? iomd_readword : memory_readfault;
      tlb->readword = readperm ? iomd_readword : memory_readfault;
      tlb->writebyte = writeperm ? iomd_writeword : memory_writefault;
      tlb->writeword = writeperm ? iomd_writeword : memory_writefault;
      break;
      
      case 0x4:  // video registers
      tlb->readbyte = tlb->readword = memory_nullread;
      tlb->writebyte = writeperm ? vidc20_writebyte : memory_writefault;
      tlb->writeword = writeperm ? vidc20_writeword : memory_writefault;
      break;
      
      default:
      tlb->readbyte = tlb->readword = memory_nullread;
      tlb->writebyte = tlb->writeword = memory_nullwrite;
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
    tlb->readbyte = tlb->readword = memory_nullread;
    tlb->writebyte = tlb->writeword = memory_nullwrite;
    break;

    case 0x10: // DRAM bank 0 (64Mb)
    case 0x11:
    case 0x12:
    case 0x13:
    tlb->readbyte = readperm ? memory_readbytebank0 : memory_readfault;
    tlb->readword = readperm ? memory_readbank0 : memory_readfault;
    tlb->writebyte = writeperm ? memory_writebytebank0 : memory_writefault;
    tlb->writeword = writeperm ? memory_writebank0 : memory_writefault;
    break;

    case 0x14: // DRAM bank 1 (64Mb)
    case 0x15:
    case 0x16:
    case 0x17:
    tlb->readbyte = readperm ? memory_readbytebank1 : memory_readfault;
    tlb->readword = readperm ? memory_readbank1 : memory_readfault;
    tlb->writebyte = writeperm ? memory_writebytebank1 : memory_writefault;
    tlb->writeword = writeperm ? memory_writebank1 : memory_writefault;
    break;

    case 0x18: // DRAM bank 2 (64Mb)
    case 0x19:
    case 0x1a:
    case 0x1b:
    tlb->readbyte = readperm ? memory_readbytebank2 : memory_readfault;
    tlb->readword = readperm ? memory_readbank2 : memory_readfault;
    tlb->writebyte = writeperm ? memory_writebytebank2 : memory_writefault;
    tlb->writeword = writeperm ? memory_writebank2 : memory_writefault;
    break;

    case 0x1c: // DRAM bank 3 (64Mb)
    case 0x1d:
    case 0x1e:
    case 0x1f:
    tlb->readbyte = readperm ? memory_readbytebank3 : memory_readfault;
    tlb->readword = readperm ? memory_readbank3 : memory_readfault;
    tlb->writebyte = writeperm ? memory_writebytebank3 : memory_writefault;
    tlb->writeword = writeperm ? memory_writebank3 : memory_writefault;
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

uint5 memory_virtualtophysical(meminfo* mem, uint5 virtualaddress,
                               tlbentry* tlb)
{
  uint5 tableindex, firstleveldescriptor, domain;
  uint3 isuser = (mem->currentmode==0);
  // bits go: aprsvd
  const static uint3 apfault[] = {
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
  return mem->datatlb.readword(mem, physaddress);
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
  return mem->insttlb.readword(mem, physaddress);
}

void memory_writeword(meminfo* mem, uint5 virtualaddress, uint5 data)
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
  mem->datatlb.writeword(mem, physaddress, data);
}

uint3 memory_readbyte(meminfo* mem, uint5 virtualaddress)
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
  return mem->datatlb.readbyte(mem, physaddress);
}

void memory_writebyte(meminfo* mem, uint5 virtualaddress, uint3 data)
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
  mem->datatlb.writebyte(mem, physaddress, data);
}

