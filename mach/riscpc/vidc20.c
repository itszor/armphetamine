#include <stdio.h>

#include "defs.h"
#include "vidc20.h"
#include "memory.h"

#ifdef VIDCSUPPORT
#include <SDL/SDL.h>
#endif

void vidc20_init(meminfo* mem)
{
/*  SDL_Init(SDL_INIT_VIDEO);*/
  mem->video.surf = 0;
  mem->video.paladdr = 0;
//  mem->video.surf = SDL_SetVideoMode(512, 384, 8, SDL_HWSURFACE);
}

void vidc20_writebyte(meminfo* mem, uint5 address, uint5 data)
{
  vidc20_writeword(mem, address, data);
}

void vidc20_writeword(meminfo* mem, uint5 address, uint5 data)
{
  fprintf(stderr, "Writing to VIDC20 register %x\n", data>>28);
  switch (data>>28)
  {
    case 0x0:  // video palette
    {
      SDL_Color col;
      col.r = data & 0xff;
      col.g = (data>>8) & 0xff;
      col.b = (data>>16) & 0xff;
      if (mem->video.surf)
      {
        SDL_SetColors(mem->video.surf, &col, mem->video.paladdr++, 1);
      }
      mem->video.paladdr &= 0xff;
    }
    break;
    
    case 0x1:  // video palette index register
    mem->video.paladdr = data & 0xff;
    break;
    
    case 0x3:  // LCD offset registers
    break;
    
    case 0x4:  // border colour register
    break;
    
    case 0x5:
    case 0x6:
    case 0x7:  // cursor palette
    break;
    
    case 0x8:
    {
      switch (data>>24)
      {
        case 0x0:  // horizontal cycle register
        break;
        
        case 0x1:  // horizontal sync width register
        break;
        
        case 0x2:  // horizontal border start register
        break;
        
        case 0x3:  // horizontal display start register
        mem->video.hstart = data & 0x3fff;
        mem->video.xsize = mem->video.hend - mem->video.hstart;
        break;
        
        case 0x4:  // horizontal display end register
        mem->video.hend = data & 0x3fff;
        mem->video.xsize = mem->video.hend - mem->video.hstart;
        break;
        
        case 0x5:  // horizontal border end register
        break;
        
        case 0x6:  // horizontal cursor start register
        mem->video.cursx = (data & 0x3fff) - mem->video.hstart;
        break;
        
        case 0x7:  // horizontal interlace register
        break;
        
        case 0x8:
        case 0xc:  // horizontal test registers
        break;
      }
    }
    break;
    
    case 0x9:
    {
      switch (data>>24)
      {
        case 0x0:  // vertical cycle register
        break;
        
        case 0x1:  // vertical sync width register
        break;
        
        case 0x2:  // vertical border start register
        break;
        
        case 0x3:  // vertical display start register
        mem->video.vstart = data & 0x1fff;
        mem->video.ysize = mem->video.vend - mem->video.vstart;
        break;
        
        case 0x4:  // vertical display end register
        mem->video.vend = data & 0x1fff;
        mem->video.ysize = mem->video.vend - mem->video.vstart;
        break;
        
        case 0x5:  // vertical border end register
        break;
        
        case 0x6:  // vertical cursor start register
        mem->video.cursys = (data & 0x7fff) - mem->video.vstart;
        mem->video.cursheight = mem->video.cursye - mem->video.cursys;
        break;
        
        case 0x7:  // vertical cursor end register
        mem->video.cursys = (data & 0x7fff) - mem->video.vstart;
        break;
        
        case 0x8:
        case 0xa:
        case 0xc:  // vertical test registers
        break;
      }
    }
    break;
    
    case 0xc:  // external register
    break;
    
    case 0xd:  // frequency synthesizer register
    break;
    
    case 0xe:  // control register
    {
      uint3 bitsperpixel[] = {1, 2, 4, 8, 16, 0, 32, 0};
      mem->video.bpp = bitsperpixel[(data>>8) & 7];
      // ok, arbitrarily we're going to say writes to the control register
      // initialise an SDL surface. Dunno if this'll work properly.

    /*  if (mem->video.surf) SDL_FreeSurface(mem->video.surf);*/
    }
    break;
    
    case 0xf:  // data control register
    {
      fprintf(stderr, "Horizontal start: %d, horizontal end: %d\n",
        mem->video.hstart, mem->video.hend);
      fprintf(stderr, "Vertical start: %d, vertical end: %d\n",
        mem->video.vstart, mem->video.vend);
      fprintf(stderr, "Setting video mode: %dx%d, %d bpp\n", mem->video.xsize,
        mem->video.ysize, mem->video.bpp);
      if (mem->video.xsize!=0 && mem->video.ysize!=0)
      {
        mem->video.surf = SDL_SetVideoMode(mem->video.xsize, mem->video.ysize,
          mem->video.bpp, SDL_HWSURFACE);
      }
    }
    break;
    
    case 0xa:  // stereo image register
    break;
    
    case 0xb:
    {
      switch (data>>24)
      {
        case 0x0:  // sound frequency register
        break;
        
        case 0x1:  // sound control register
        break;
      }
    }
    break;
  }
}


