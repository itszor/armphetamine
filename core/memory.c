#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "libjtype/defs.h"
#include "libjtype/cnew.h"
#include "core/memory.h"
#include "core/processor.h"
#include "core/machine.h"

#ifdef RISCPCEMU
#include "mach/riscpc/vidc20.h"
#include "mach/riscpc/iomd.h"
#endif

#define USETLB 1
/*#undef USETLB*/

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

meminfo* memory_initialise(uint5 bytes)
{
  meminfo* mem = jt_new(meminfo);
  uint5 i;

  IGNORE(bytes);

/*  mem->memory = cnewarray(uint5, bytes/4);*/
  mem->vram = VRAM ? jt_newarray(uint5, VRAM/4) : 0;
  mem->currentmode = 0;
  mem->memoryfault = 0;
  memory_inittlb(&mem->insttlb);
  memory_inittlb(&mem->datatlb);

  // start off with MMU disabled
  mem->mmuactive = 0;

  // full 4Gb address-space worth of hashes
  fprintf(stderr, "Creating empty transmap\n");
  mem->transmap = jt_newarray(jt_hash*, 1<<20);
  for (i=0; i<1<<20; i++)
  {
    mem->transmap[i] = 0;
  }
	
  memory_subinitialise(mem);
  
//	fprintf(stderr, "Initialised memory, MMU inactive\n");

  mem->trace = fopen("trace.out", "w");

  mem->mmucontrol = 0;

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

void memory_inittlb(tlbentry* tlb)
{
  tlb->modestamp = -1;
  tlb->virtual = 0;
  tlb->physical = 0;
  tlb->mask = 0;
  tlb->write = mem_wnull;
  tlb->read = mem_rnull;
}

void memory_invalidatetlb(tlbentry* tlb)
{
  tlb->modestamp = -1;  // there isn't a mode -1
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
  IGNORE(mem);
  IGNORE(virtualaddress);
}
#endif

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
  
  fprintf(stderr, "Write fault when trying to write %.8x to %.8x. Mode=%s\n", 
    data, physaddress, modename_st[mem->currentmode]);
 /* ((machineinfo*)mem->parent)->trace = 1;*/
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
  exit(1);
  return 0xdeadbeef;
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
  fprintf(stderr, "Null read at %x.\n", physaddress);
  exit(1);
  return 0x0;
}

uint5 memory_virtualtophysical(meminfo* mem, uint5 virtualaddress,
                               tlbentry* tlb, uint5 alignment)
{
  uint5 tableindex, firstleveldescriptor, domain;
  uint3 isuser = (mem->currentmode&15==0);
  // bits go: aprsvd
  static const uint3 apfault[] = {
//  user  super
//  r  w  r  w
    0, 0, 0, 0,  // ap = 00, r = 0, s = 0
    0, 0, 1, 0,  // ap = 00, r = 0, s = 1
    1, 0, 1, 0,  // ap = 00, r = 1, s = 0
    0, 0, 0, 0,  // ap = 00, r = 1, s = 1
    
    0, 0, 1, 1,  // ap = 01, r = 0, s = 0
    0, 0, 1, 1,  // ap = 01, r = 0, s = 1
    0, 0, 1, 1,  // ap = 01, r = 1, s = 0
    0, 0, 1, 1,  // ap = 01, r = 1, s = 1
    
    1, 0, 1, 1,  // ap = 10, r = 0, s = 0
    1, 0, 1, 1,  // ap = 10, r = 0, s = 1
    1, 0, 1, 1,  // ap = 10, r = 1, s = 0
    1, 0, 1, 1,  // ap = 10, r = 1, s = 1
    
    1, 1, 1, 1,  // ap = 11, r = 0, s = 0
    1, 1, 1, 1,  // ap = 11, r = 0, s = 1
    1, 1, 1, 1,  // ap = 11, r = 1, s = 0
    1, 1, 1, 1   // ap = 11, r = 1, s = 1
  };

  if (!(mem->mmucontrol & 1)) {
    tlb->virtual = tlb->physical = virtualaddress & 0xff000000;
    tlb->mask = 0xff000000;  // can't use whole memory (I/O etc.)
    memory_physicalmap(tlb, virtualaddress, 1, 1);
    tlb->modestamp = mem->currentmode;
    return virtualaddress;
  }

  if ((mem->mmucontrol & 2) && (virtualaddress & (alignment-1))) {
  /* !!! ok, this is disabled 'cos it triggers on bytes, which is stupid */
    fprintf(stderr, "Alignment fault!\n");
    mem->memoryfault = 1;
  }

  tableindex = virtualaddress >> 20;
  firstleveldescriptor = memory_readphysicalword(mem,
    mem->translationbase | (tableindex << 2));
  domain = (firstleveldescriptor >> 5) & 0xf;

  switch (firstleveldescriptor & 3)
  {
    case 0:  // first level descriptor: fault
    {      
      fprintf(stderr, "First level descriptor fault at %.8x\n", virtualaddress);
      fprintf(stderr, "First-level descriptor=%.8x\n", firstleveldescriptor);
      fprintf(stderr, "Translation base=%.8x\n", mem->translationbase);
      fprintf(stderr, "Table index=%.8x\n", tableindex);
      
  /*  diss_around_pc(mem->parent);*/
      
      mem->memoryfault = 1;
    }
    break;

    case 1:  // first level descriptor: coarse second-level table
    {
      uint5 l2tableindex = (virtualaddress >> 12) & 0xff;
      uint5 pagetablebaseaddress = firstleveldescriptor >> 10;  
      uint5 secondleveldescriptor = memory_readphysicalword(mem, 
        (pagetablebaseaddress << 10) | (l2tableindex << 2));
      uint5 domainaccess = (mem->domainaccesscontrol >> (domain << 1)) & 0x3;

      if (domainaccess==0 || domainaccess==2)
      {
        fprintf(stderr, "Page domain fault!\n");
        memory_invalidatetlb(tlb);
        mem->memoryfault = 1;
        return 0xdeadbeef;
      }

      switch (secondleveldescriptor & 3)  
      {  
        case 0:  // page translation fault
        {
          fprintf(stderr, "Page translation fault!\n");
          mem->memoryfault = 1;
        }
        break;  

        case 1:  // large page (64kb)
        {
          uint5 pageindex = 0, physaddress;
          uint3 fullpage = 0;

          pageindex = virtualaddress & 0xffff;
          physaddress = (secondleveldescriptor & 0xffff0000) | pageindex;

          if (domainaccess==1)  // client
          {
            /* r bit (9) & s bit (8) */
            uint5 rsbits = (mem->mmucontrol >> 8) & 3;
            uint5 subpage = (pageindex >> 14) & 3;
            uint3 ap = (secondleveldescriptor >> (2*subpage+4)) & 3;
            uint3 ap0 = (secondleveldescriptor >> 4) & 3;
            uint3 ap1 = (secondleveldescriptor >> 6) & 3;
            uint3 ap2 = (secondleveldescriptor >> 8) & 3;
            uint3 ap3 = (secondleveldescriptor >> 10) & 3;
            uint5 fltbase = (ap<<4) | (rsbits<<2);

            if (ap0==ap1 && ap1==ap2 && ap2==ap3) fullpage = 1;

            if (!isuser) fltbase += 2;

            memory_physicalmap(tlb, physaddress,
              apfault[fltbase+0], apfault[fltbase+1]);
          }
          else  // manager, no permission checking
          {
            memory_physicalmap(tlb, physaddress, 1, 1);
            fullpage = 1;
          }

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

        case 2:  // small page (4kb)
        {  
          uint5 pageindex = 0, physaddress;
          uint3 fullpage = 0;
          
          pageindex = virtualaddress & 0xfff;  
          physaddress = (secondleveldescriptor & 0xfffff000) | pageindex;

          if (domainaccess==1)  // client
          {
            /* r bit (9) & s bit (8) */
            uint5 rsbits = (mem->mmucontrol >> 8) & 3;
            uint5 subpage = (pageindex >> 10) & 3;
            uint5 ap = (secondleveldescriptor >> (2*subpage+4)) & 3;
            uint3 ap0 = (secondleveldescriptor >> 4) & 3;
            uint3 ap1 = (secondleveldescriptor >> 6) & 3;
            uint3 ap2 = (secondleveldescriptor >> 8) & 3;
            uint3 ap3 = (secondleveldescriptor >> 10) & 3;
            uint5 fltbase = (ap<<4) | (rsbits<<2);
                        
            if (ap0==ap1 && ap1==ap2 && ap2==ap3) fullpage = 1;

            if (!isuser) fltbase += 2;

/*            fprintf(stderr, "fltbase=%d, subpage=%d, "
              "S bit=%d, R bit=%d\n", fltbase, subpage,
                (mem->mmucontrol >> 8)&1, (mem->mmucontrol >> 9)&1);*/

            memory_physicalmap(tlb, physaddress,
              apfault[fltbase+0], apfault[fltbase+1]);
          }
          else  // manager, no permission checking
          {
            memory_physicalmap(tlb, physaddress, 1, 1);
            fullpage = 1;
          }

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
        
        case 3:  // tiny page
        {
          fprintf(stderr, "Tiny page found in coarse page table!!\n");
          abort();
        }
        break;
      }
    }
    break;

    case 2:  // first-level descriptor: section
    {
      uint5 sectionindex, physaddress;
      uint5 domainaccess = (mem->domainaccesscontrol >> (domain << 1)) & 0x3;

      if (domainaccess==0 || domainaccess==2)
      {
        fprintf(stderr, "Section domain fault\n");
        memory_invalidatetlb(tlb);
        mem->memoryfault = 1;
        return 0xdeadbeef;
      }
      
      sectionindex = virtualaddress & 0x000fffff;
      physaddress = (firstleveldescriptor & 0xfff00000) | sectionindex;

      if (domainaccess==1)
      {
        /* r bit (9) & s bit (8) */
        uint5 rsbits = (mem->mmucontrol >> 8) & 3;
        uint5 ap = (firstleveldescriptor >> 10) & 3;
        uint5 fltbase = (ap<<4) | (rsbits<<2);

        if (!isuser) fltbase += 2;

        memory_physicalmap(tlb, physaddress,
          apfault[fltbase+0], apfault[fltbase+1]);
      }
      else
      {
        memory_physicalmap(tlb, physaddress, 1, 1);
      }

      tlb->mask = 0xfff00000;
      tlb->physical = physaddress & 0xfff00000;
      tlb->virtual = virtualaddress & 0xfff00000;
      tlb->modestamp = mem->currentmode;
      return physaddress;
    }
    break;
    
    case 3:  // first-level descriptor: fine second-level table
    {
      fprintf(stderr, "Trying to translate fine second-level table\n");
      exit(1);
    }
    break;
  }
  assert(!"Page translation failed");
  return -1;  // shouldn't happen!
}

uint5 memory_readdataword(meminfo* mem, uint5 virtualaddress)
{
  uint5 physaddress, data;
#ifdef USETLB
  if (mem->datatlb.modestamp != mem->currentmode ||
      (virtualaddress & mem->datatlb.mask) != mem->datatlb.virtual)
  {
#endif
    physaddress = memory_virtualtophysical(mem, virtualaddress,
      &mem->datatlb, 4);
#ifdef USETLB
  }
  else
  {
    physaddress = mem->datatlb.physical |
                  (virtualaddress & ~mem->datatlb.mask);
  }
#endif
  data = mem->datatlb.read.word(mem, physaddress);
/*  fprintf(stderr, "Read data word %.8x from virtual addr %.8x\n", 
    data, virtualaddress);*/
  return data;
}

uint5 memory_readinstword(meminfo* mem, uint5 virtualaddress)
{
  uint5 physaddress;
#ifdef USETLB
  if (mem->insttlb.modestamp != mem->currentmode ||
      (virtualaddress & mem->insttlb.mask) != mem->insttlb.virtual)
  {
#endif
    physaddress = memory_virtualtophysical(mem, virtualaddress,
      &mem->insttlb, 4);
#ifdef USETLB
  }
  else
  {
    physaddress = mem->insttlb.physical |
                  (virtualaddress & ~mem->insttlb.mask);
  }
#endif
  return mem->insttlb.read.word(mem, physaddress);
}

void memory_writeword(meminfo* mem, uint5 virtualaddress, uint5 data)
{
  uint5 physaddress;
  static int dec = 9;
/*  fprintf(stderr, "Write data word %.8x to virtual addr %.8x\n", data, 
          virtualaddress);*/
#ifdef USETLB
  if (mem->datatlb.modestamp != mem->currentmode ||
      (virtualaddress & mem->datatlb.mask) != mem->datatlb.virtual)
  {
#endif  
    physaddress = memory_virtualtophysical(mem, virtualaddress,
      &mem->datatlb, 4);
#ifdef USETLB
  }
  else
  {
    physaddress = mem->datatlb.physical |
                  (virtualaddress & ~mem->datatlb.mask);
  }
  if (physaddress==0xc0004000) {
    fprintf(stderr, "Writing %.8x to %.8x\n", data, physaddress);
    if (!--dec) ((machineinfo*)mem->parent)->trace = 1;
  }
#endif
  mem->datatlb.write.word(mem, physaddress, data);
}

uint5 memory_readbyte(meminfo* mem, uint5 virtualaddress)
{
  uint5 physaddress;
#ifdef USETLB
  if (mem->datatlb.modestamp != mem->currentmode ||
      (virtualaddress & mem->datatlb.mask) != mem->datatlb.virtual)
  {
#endif
    physaddress = memory_virtualtophysical(mem, virtualaddress,
      &mem->datatlb, 1);
#ifdef USETLB
  }
  else
  {
    physaddress = mem->datatlb.physical |
                  (virtualaddress & ~mem->datatlb.mask);
  }
#endif
  return mem->datatlb.read.byte(mem, physaddress);
}

uint5 memory_readhalf(meminfo* mem, uint5 virtualaddress)
{
  uint5 physaddress;
#ifdef USETLB
  if (mem->datatlb.modestamp != mem->currentmode ||
      (virtualaddress & mem->datatlb.mask) != mem->datatlb.virtual)
  {
#endif
    physaddress = memory_virtualtophysical(mem, virtualaddress,
      &mem->datatlb, 2);
#ifdef USETLB
  }
  else
  {
    physaddress = mem->datatlb.physical |
                  (virtualaddress & ~mem->datatlb.mask);
  }
#endif
  return mem->datatlb.read.half(mem, physaddress);
}

uint5 memory_readsbyte(meminfo* mem, uint5 virtualaddress)
{
  uint5 physaddress;
  uint5 data;
#ifdef USETLB
  if (mem->datatlb.modestamp != mem->currentmode ||
      (virtualaddress & mem->datatlb.mask) != mem->datatlb.virtual)
  {
#endif
    physaddress = memory_virtualtophysical(mem, virtualaddress,
      &mem->datatlb, 1);
#ifdef USETLB
  }
  else
  {
    physaddress = mem->datatlb.physical |
                  (virtualaddress & ~mem->datatlb.mask);
  }
#endif
  data = mem->datatlb.read.byte(mem, physaddress);
  if (data & 0x80) data |= 0xffffff00;
  return data;
}

uint5 memory_readshalf(meminfo* mem, uint5 virtualaddress)
{
  uint5 physaddress;
  uint5 data;
#ifdef USETLB
  if (mem->datatlb.modestamp != mem->currentmode ||
      (virtualaddress & mem->datatlb.mask) != mem->datatlb.virtual)
  {
#endif
    physaddress = memory_virtualtophysical(mem, virtualaddress,
      &mem->datatlb, 2);
#ifdef USETLB
  }
  else
  {
    physaddress = mem->datatlb.physical |
                  (virtualaddress & ~mem->datatlb.mask);
  }
#endif
  data = mem->datatlb.read.half(mem, physaddress);
  if (data & 0x8000) data |= 0xffff0000;
  return data;
}

void memory_writebyte(meminfo* mem, uint5 virtualaddress, uint5 data)
{
  uint5 physaddress;
#ifdef USETLB
  if (mem->datatlb.modestamp != mem->currentmode ||
      (virtualaddress & mem->datatlb.mask) != mem->datatlb.virtual)
  {
#endif
    physaddress = memory_virtualtophysical(mem, virtualaddress,
      &mem->datatlb, 1);
#ifdef USETLB
  }
  else
  {
    physaddress = mem->datatlb.physical |
                  (virtualaddress & ~mem->datatlb.mask);
  }
#endif
  mem->datatlb.write.byte(mem, physaddress, data);
}

void memory_writehalf(meminfo* mem, uint5 virtualaddress, uint5 data)
{
  uint5 physaddress;
#ifdef USETLB
  if (mem->datatlb.modestamp != mem->currentmode ||
      (virtualaddress & mem->datatlb.mask) != mem->datatlb.virtual)
  {
#endif
    physaddress = memory_virtualtophysical(mem, virtualaddress,
      &mem->datatlb, 2);
#ifdef USETLB
  }
  else
  {
    physaddress = mem->datatlb.physical |
                  (virtualaddress & ~mem->datatlb.mask);
  }
#endif
  mem->datatlb.write.half(mem, physaddress, data);
}

