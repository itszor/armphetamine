#ifndef MEMORY_H
#define MEMORY_H 1

#include "defs.h"
#include "keyboard.h"
#include "mouse.h"

#ifdef VIDCSUPPORT
#include <SDL/SDL.h>
#endif

struct meminfo;

typedef void (*writefn)(struct meminfo* mem, uint5 physaddr, uint5 data);
typedef uint5 (*readfn)(struct meminfo* mem, uint5 physaddr);

typedef struct {
  writefn byte;
  writefn half;
  writefn word;
} mem_writebank;

typedef struct {
  readfn byte;
  readfn sbyte;
  readfn half;
  readfn shalf;
  readfn word;
} mem_readbank;

typedef struct {
  uint5 virtual;
  uint5 physical;
  uint5 mask;
  uint5 modestamp;
  mem_writebank write;
  mem_readbank read;
} tlbentry;

typedef struct {
  uint3 status;
  uint3 request;
  uint3 mask;
} irq_info;

struct meminfo {
  uint5* memory;
  uint5* rom0;
  uint5* rom1;
  uint5* vram;
  uint5* bank0;
  uint5* bank1;
  uint5* bank2;
  uint5* bank3;
  tlbentry insttlb, datatlb;
  uint5 mmucontrol;          // register 1 (write)
  uint5 translationbase;     // register 2 (write)
  uint5 domainaccesscontrol; // register 3 (write)
  uint5 faultstatus;         // register 5 (read)
  uint5 faultaddress;        // register 6 (read)
  int mmuactive;

#ifdef VIDCSUPPORT
  struct {
    SDL_Surface* surf;
    uint5 hstart;
    uint5 hend;
    uint5 xsize;
    uint5 vstart;
    uint5 vend;
    uint5 ysize;
    uint5 cursx;
    uint5 cursys;
    uint5 cursye;
    uint5 cursheight;
    uint5 bpp;
    uint5 paladdr;
  } video;
#endif

#ifdef IOMDSUPPORT
  struct {
    uint3 t0lolat;
    uint3 t0hilat;
    uint3 t1lolat;
    uint3 t1hilat;
    uint4 timer0;
    uint4 timer1;
    irq_info irqa;
    irq_info irqb;
    irq_info irqc;
    irq_info irqd;
    irq_info fiq;
    irq_info sounddma;
    uint3 clkctl;
    keyboard_info kbd;
    mouse_info mouse;
  } io;
#endif

  int writetag;
  uint3 currentmode;
  uint3 memoryfault;
  void* parent;
};

typedef struct meminfo meminfo;

#define BANK0RAM (4096*1024)
#define BANK1RAM (4096*1024)
#define BANK2RAM (4096*1024)
#define BANK3RAM (4096*1024)
#define VRAM (2048*1024)

extern meminfo* memory_initialise(uint5 bytes);
extern void memory_invalidatetlb(tlbentry* tlb);
extern void memory_setmapping(meminfo* mem, uint5 from, uint5 to);
extern __inline__ uint5* memory_lookup(meminfo* mem, uint5* virtualaddress);

void memory_nullwrite(meminfo* mem, uint5 addr, uint5 data);
void memory_writefault(meminfo* mem, uint5 addr, uint5 data);
void memory_writevram(meminfo* mem, uint5 addr, uint5 data);
void memory_writebytevram(meminfo* mem, uint5 addr, uint5 data);
void iomd_writeword(meminfo* mem, uint5 addr, uint5 data);
void vidc20_writebyte(meminfo* mem, uint5 addr, uint5 data);
void vidc20_writeword(meminfo* mem, uint5 addr, uint5 data);
void memory_writebytebank0(meminfo* mem, uint5 addr, uint5 data);
void memory_writebank0(meminfo* mem, uint5 addr, uint5 data);
void memory_writebytebank1(meminfo* mem, uint5 addr, uint5 data);
void memory_writebank1(meminfo* mem, uint5 addr, uint5 data);
void memory_writebytebank2(meminfo* mem, uint5 addr, uint5 data);
void memory_writebank2(meminfo* mem, uint5 addr, uint5 data);
void memory_writebytebank3(meminfo* mem, uint5 addr, uint5 data);
void memory_writebank3(meminfo* mem, uint5 addr, uint5 data);

uint5 memory_nullread(meminfo* mem, uint5 addr);
uint5 memory_readfault(meminfo* mem, uint5 addr);
uint5 memory_readvram(meminfo* mem, uint5 addr);
uint5 memory_readbytevram(meminfo* mem, uint5 addr);
uint5 memory_readrom0(meminfo* mem, uint5 addr);
uint5 memory_readbyterom0(meminfo* mem, uint5 addr);
uint5 memory_readrom1(meminfo* mem, uint5 addr);
uint5 memory_readbyterom1(meminfo* mem, uint5 addr);
uint5 iomd_readword(meminfo* mem, uint5 addr);
uint5 memory_readbytebank0(meminfo* mem, uint5 addr);
uint5 memory_readbank0(meminfo* mem, uint5 addr);
uint5 memory_readbytebank1(meminfo* mem, uint5 addr);
uint5 memory_readbank1(meminfo* mem, uint5 addr);
uint5 memory_readbytebank2(meminfo* mem, uint5 addr);
uint5 memory_readbank2(meminfo* mem, uint5 addr);
uint5 memory_readbytebank3(meminfo* mem, uint5 addr);
uint5 memory_readbank3(meminfo* mem, uint5 addr);

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
