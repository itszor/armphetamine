#include "libjtype/defs.h"
#include "libjtype/cnew.h"
#include "core/memory.h"
#include "core/processor.h"
#include "core/machine.h"

#include "sapcm.h"
#include "ostimer.h"
#include "intctrl.h"
#include "mexreg.h"
#include "lartmem.h"
#include "lartflash.h"

const mem_readbank mem_rrom0 =
{
  memory_readbyterom0,
  memory_readhalfrom0,
  memory_readrom0
};

const mem_writebank mem_wrom0 =
{
  memory_writerom0,
  memory_writerom0,
  memory_writerom0
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

const mem_readbank mem_mexreg_read =
{
  sa1100_mexreg_read,
  sa1100_mexreg_read,
  sa1100_mexreg_read
};

const mem_writebank mem_mexreg_write =
{
  sa1100_mexreg_write,
  sa1100_mexreg_write,
  sa1100_mexreg_write
};

void memory_subinitialise(meminfo* mem)
{
  fprintf(stderr, "Initialising LART memory subsystem\n");
  mem->bank0 = jt_newarray(uint5, BANK0RAM/4);
  mem->bank1 = jt_newarray(uint5, BANK1RAM/4);
  mem->bank2 = jt_newarray(uint5, BANK2RAM/4);
  mem->bank3 = jt_newarray(uint5, BANK3RAM/4);
  mem->rom0 = jt_newarray(uint5, 16*1024*1024/4);
  mem->rom1 = jt_newarray(uint5, 16*1024*1024/4); // hello, I'm flash
  mem->sapcm.dma = jt_newarray(sapcm_dma_channel, 6);
  mem->sapcm.serial_fifo = jt_new(sapcm_serial_fifo);
  mem->sapcm.serial_fifo->in = fifo_create(12);
  mem->sapcm.serial_fifo->out = fifo_create(8);
  mem->ostimer = ostimer_new();
  mem->intctrl = intctrl_new();
  mem->mex = mexreg_new();
  mem->flashmode = READ_ARRAY;
}

void memory_physicalmap(tlbentry* tlb, uint5 physaddress, uint3 readperm,
                        uint3 writeperm)
{
  switch (physaddress >> 24)
  {
    case 0xe8: // alt location ? (physical or virtual??)
    case 0xec:
    fprintf(stderr, "Urgle burgle, accessing flash as "
                    "virtual or physical addr?\n");
    abort();
    break;
    
    case 0x08:
    fprintf(stderr, "Access alt. ROM location %.8x\n", physaddress);
    case 0x00: // ROM bank 0
    tlb->read = readperm ? mem_rrom0 : mem_rfault;
    tlb->write = writeperm ? mem_wrom0 : mem_wfault;
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
    tlb->read = readperm ? mem_mexreg_read : mem_rfault;
    tlb->write = writeperm ? mem_mexreg_write : mem_wfault;
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

    case 0xc2 ... 0xc7:
    break;

    case 0xc8: // DRAM bank 2 (8Mb)
    tlb->read = readperm ? mem_rbank2 : mem_rfault;
    tlb->write = writeperm ? mem_wbank2 : mem_wfault;
    break;

    case 0xc9: // DRAM bank 3 (8Mb)
    tlb->read = readperm ? mem_rbank3 : mem_rfault;
    tlb->write = writeperm ? mem_wbank3 : mem_wfault;
    break;

    case 0xca ... 0xe0:
    break;

    default:
    fprintf(stderr, "Bad physical address for LART at %.8x\n", physaddress);
    abort();
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
  switch (mem->flashmode)
  {
    case READ_ARRAY:
    return mem->rom0[(physaddress & 0xffffff) >> 2];
    break;

    case READ_ID_CODES:
    {
      switch (FLASH_U2_TO_ADDR(physaddress))
      {
        case 0x00000000:
        return DATA_TO_FLASH(0x00890089);  /* manufacturer code */
        break;

        case 0x00000001:
        return DATA_TO_FLASH(0x88f388f3);  /* device code -- 16mbit_top */
        break;

        default:
        fprintf(stderr, "Unknown flash ID code, aborting\n");
        abort();
      }
    }
    break;
    
    default:
    fprintf(stderr, "Unknown flash mode, aborting\n");
    abort();
  }
}

uint5 memory_readhalfrom0(meminfo* mem, uint5 physaddress)
{
  if (mem->flashmode==READ_ARRAY)
    return ((uint4*)mem->rom0)[(physaddress & 0xffffff) >> 1];
  else
    abort();
}

uint5 memory_readbyterom0(meminfo* mem, uint5 physaddress)
{
  fprintf(stderr, "Read flash byte at %.8x\n", physaddress);
  if (mem->flashmode==READ_ARRAY)
    return ((uint3*)mem->rom0)[physaddress & 0xffffff];
  else
    abort();
}

void memory_writerom0(meminfo* mem, uint5 addr, uint5 data)
{
  if (addr==0x0 && data==DATA_TO_FLASH(READ_ID_CODES))
  {
    fprintf(stderr, "FLASH: Read ID codes\n");
    mem->flashmode = READ_ID_CODES;
  }
  else if (addr==0x0 && data==DATA_TO_FLASH(READ_ARRAY))
  {
    fprintf(stderr, "FLASH: Array read mode\n");
    mem->flashmode = READ_ARRAY;
  }
 /* else if (data==0x0)
  {
    fprintf(stderr, "FLASH: Ignoring write\n");
  }*/
  else
  {
    fprintf(stderr, "FLASH: Don't know what to do (addr: %.8x, data: %.8x)\n",
      addr, data);
    abort();
  }
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
