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
#include "decode.h"

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
    /* clears bits set to one in data */
    mem->ostimer->ossr &= ~data;
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
  registerinfo* reg = machine->reg;
  uint5 pc = reg->cpsr.flag.mode<16 ? (reg->r[15] & ~0xfc000003)-8
                                    : reg->r[15]-8;
  instructionformat inst;
  ostimer_registers* oti = machine->mem->ostimer;
  uint5 i, clk;

  clk = oti->clock++;

  /* watchdog timer */
  if ((oti->ower & 1)==1 && oti->osmr[3]==clk)
  {
    fprintf(stderr, "Watchdog timer, whoops\n");
   /* processor_reset(machine);*/
    return;
  }

  for (i=0; i<4; i++)
  {
    if (clk==oti->osmr[i])
    {
      /* set corresponding bit in status register */
      oti->ossr |= 1<<i;
    }
  }
  
  /* any enabled interrupts happen? */
  if (((oti->oier & oti->ossr) & 0xf)>0 && !(reg->cpsr.flag.interrupt&0x2))
  {
    fprintf(stderr, "Making ostimer interrupt!\n");
 /*   fprintf(stderr, "%.8x : %.8x : ", pc, inst.instruction);
    dispatch(machine, inst, &diss, (void*)pc);
    fprintf(stderr, "\n");
    machine->trace = 1;*/
    
    processor_irq(machine);
  }
}

#endif
