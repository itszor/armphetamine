#ifndef MEMORY_H
#define MEMORY_H 1

#include "defs.h"
#ifdef VIRTUALFRAMEBUFFER
#include "SDL.h"
#endif

struct meminfo;

typedef void (*writefn)(struct meminfo* mem, uint5 physaddr, uint5 data);
typedef uint5 (*readfn)(struct meminfo* mem, uint5 physaddr);

typedef struct {
  uint5 virtual;
  uint5 physical;
  uint5 mask;
  uint5 modestamp;
  writefn writebyte;
  readfn readbyte;
  writefn writeword;
  readfn readword;
} tlbentry;

struct meminfo {
  uint5* memory;
  uint5* rom0;
  uint5* rom1;
  uint5* bank0;
  uint5* bank1;
  uint5* bank2;
  uint5* bank3;
  tlbentry insttlb, datatlb;
//  level1pt* pagetable;
  uint5 mmucontrol;          // register 1 (write)
  uint5 translationbase;     // register 2 (write)
  uint5 domainaccesscontrol; // register 3 (write)
  uint5 faultstatus;         // register 5 (read)
  uint5 faultaddress;        // register 6 (read)
  int mmuactive;
  #ifdef VIRTUALFRAMEBUFFER
  SDL_Surface* screen;
  #endif
  int writetag;
  uint3 currentmode;
  uint3 memoryfault;
};

typedef struct meminfo meminfo;

#define BANK0RAM (4096*1024)
#define BANK1RAM (4096*1024)
#define BANK2RAM (4096*1024)
#define BANK3RAM (4096*1024)

extern meminfo* memory_initialise(uint5 bytes);
extern void memory_invalidatetlb(tlbentry* tlb);
extern void memory_setmapping(meminfo* mem, uint5 from, uint5 to);
extern __inline__ uint5* memory_lookup(meminfo* mem, uint5* virtualaddress);
extern void memory_postwrite(meminfo* mem, uint5* virtualaddress);
extern uint5 memory_virtualtophysical(meminfo* mem, uint5 virtualaddress,
                                      tlbentry* tlb);
extern uint5 memory_readinstword(meminfo* mem, uint5 address);
extern uint5 memory_readdataword(meminfo* mem, uint5 address);
extern void memory_writeword(meminfo* mem, uint5 address, uint5 data);
extern uint3 memory_readbyte(meminfo* mem, uint5 address);
extern void memory_writebyte(meminfo* mem, uint5 address, uint3 data);

#define memory_lookup(M,V) ((uint5*)((uint5)(M)->memory+(uint5)(V)))

#endif
