
#ifndef MEMORY_H
#define MEMORY_H 1

#include "defs.h"
#ifdef VIRTUALFRAMEBUFFER
#include "SDL.h"
#endif

// array of indices of pages
typedef struct {
  uint5 base[16];
} level3pt;

// array of pointers to level 3 page tables
typedef struct {
  level3pt* ptr[256];
} level2pt;

// array of pointers to level 2 page tables
typedef struct {
  level2pt* ptr[256];
} level1pt;

typedef union {
  struct {
    uint5 offset : 12;
    uint5 l1     : 4;
    uint5 l2     : 8;
    uint5 l3     : 8;
  } part;
  uint5 address;
} addressparts;

typedef struct {
  uint5* memory;
  level1pt* pagetable;
  int mmuactive;
  #ifdef VIRTUALFRAMEBUFFER
  SDL_Surface* screen;
  #endif
  int writetag;
} meminfo;

extern meminfo* memory_initialise(uint5 bytes);
extern void memory_setmapping(meminfo* mem, uint5 from, uint5 to);
extern __inline__ uint5* memory_lookup(meminfo* mem, uint5* virtualaddress);
extern void memory_postwrite(meminfo* mem, uint5* virtualaddress);

#define memory_lookup(M,V) ((uint5*)((uint5)(M)->memory+(uint5)(V)))

#endif
