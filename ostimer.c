#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "defs.h"
#include "iomd.h"
#include "keyboard.h"
#include "mouse.h"
#include "machine.h"
#include "cnew.h"
#include "ostimer.h"

#ifdef EMULART

ostimer_registers* ostimer_new(void)
{
  ostimer_registers* reg = cnew(ostimer_registers);
  reg->osmr[0] = reg->osmr[1] = reg->osmr[2] = reg->osmr[3] = 0;
  reg->ower = 0;
  reg->ossr = 0;
  reg->oier = 0;
  reg->clock = 0;
  
  return reg;
}

void ostimer_write(meminfo* mem, uint5 address, uint5 data)
{
  switch (address & 0x1f)
  {
    case 0x00:
    case 0x04:
    case 0x08:
    case 0x0c:
    mem->ostimer->osmr[(address & 0x1f)>>2] = data;
    break;
    
    case 0x10:
    mem->ostimer->clock = data;
    break;
    
    case 0x14:
    mem->ostimer->ossr = data;
    break;
    
    case 0x18:
    mem->ostimer->ower = data;
    break;
    
    case 0x1c:
    mem->ostimer->oier = data;
    break;
  }
}


uint5 ostimer_read(meminfo* mem, uint5 address)
{ 
  switch (address & 0x1f)
  {
    case 0x00:
    case 0x04:
    case 0x08:
    case 0x0c:
    return mem->ostimer->osmr[(address & 0x1f)>>2];
    break;
    
    case 0x10:
    return mem->ostimer->clock;
    break;
    
    case 0x14:
    return mem->ostimer->ossr;
    break;
    
    case 0x18:
    return mem->ostimer->ower;
    break;
    
    case 0x1c:
    return mem->ostimer->oier;
    break;
  }
  return 0xdeadbeef;
}

void ostimer_clock(machineinfo* machine)
{
  machine->mem->ostimer->clock++;
}

#endif
