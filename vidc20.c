#include <stdio.h>

#include "defs.h"
#include "vidc20.h"
#include "memory.h"

void vidc20_writebyte(meminfo* mem, uint5 address, uint5 data)
{
  vidc20_writeword(mem, address, data);
}

void vidc20_writeword(meminfo* mem, uint5 address, uint5 data)
{
  fprintf(stderr, "Writing to VIDC20 register\n");
  switch (data>>28)
  {
    case 0x0:  // video palette
    break;
    
    case 0x1:  // video palette index register
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
        break;
        
        case 0x4:  // horizontal display end register
        break;
        
        case 0x5:  // horizontal border end register
        break;
        
        case 0x6:  // horizontal cursor start register
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
        break;
        
        case 0x4:  // vertical display end register
        break;
        
        case 0x5:  // vertical border end register
        break;
        
        case 0x6:  // vertical cursor start register
        break;
        
        case 0x7:  // vertical cursor end register
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
    break;
    
    case 0xf:  // data control register
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
