#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "libjtype/defs.h"
#include "libjtype/cnew.h"
/*#include "iomd.h"*/
#include "core/machine.h"

#include "mach/lart/ostimer.h"
#include "core/decode.h"
#include "mach/lart/intctrl.h"

ostimer_registers* ostimer_new(void)
{
  ostimer_registers* reg = jt_new(ostimer_registers);
  reg->osmr[0] = reg->osmr[1] = reg->osmr[2] = reg->osmr[3] = 0;
  reg->ower = 0;
  reg->ossr = 0;
  reg->oier = 0;
  reg->clock = 0;
  
  return reg;
}

void ostimer_write(meminfo* mem, uint5 address, uint5 data)
{
  switch ((address>>16) & 0xf)
  {
    case 0x0:  // OS timer
    switch (address & 0x1f)
    {
      case 0x00:
      case 0x04:
      case 0x08:
      case 0x0c:
      fprintf(stderr, "Write match register %d to %x (cnt=%x)\n", 
        (address&0x1f)/4, data, mem->ostimer->clock);
      mem->ostimer->osmr[(address & 0x1f)>>2] = data;
      break;

      case 0x10:
      fprintf(stderr, "Writing %d to clock\n", data);
      mem->ostimer->clock = data;
      break;

      case 0x14:
      /* clears bits set to one in data */
      mem->ostimer->ossr &= ~data;
      fprintf(stderr, "Stripping int bits\n");
      mem->intctrl->icip &= ~(data<<26);
      mem->intctrl->icfp &= ~(data<<26);
      mem->intctrl->icpr &= ~(data<<26);
      break;

      case 0x18:
      mem->ostimer->ower = data;
      break;

      case 0x1c:
      mem->ostimer->oier = data;
      break;
    }
    break;
    
    case 0x5:  // interrupt controller
    fprintf(stderr, "Write interrupt controller @ %x, %.8x\n", address & 0xff,
      data);
    switch (address & 0xff)
    {
      case 0x00:
      mem->intctrl->icip &= ~data;
      fprintf(stderr, "Writing to icip? Aargh aargh panic, dying\n");
      abort();
      break;

      case 0x04:
      mem->intctrl->icmr = data;
      break;

      case 0x08:
      mem->intctrl->iclr = data;
      break;

      case 0x10:
      mem->intctrl->icfp &= ~data;
      fprintf(stderr, "Writing to icfp? Aargh aargh panic, dying\n");
      abort();
      break;

      case 0x20:
      mem->intctrl->icpr &= ~data;
      fprintf(stderr, "Writing to icpr? Aargh aargh panic, dying\n");
      abort();
      break;

      case 0x0c:
      mem->intctrl->iccr = data;
      break;
    }
    break;
  }
}


uint5 ostimer_read(meminfo* mem, uint5 address)
{ 
  switch ((address>>16) & 0xf)
  {
    case 0x0:  // OS timer
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
    break;

    case 0x5:  // interrupt controller
    fprintf(stderr, "Read interrupt controller @ %x, ", address & 0xff);
    switch (address & 0xff)
    {
      case 0x00:
      fprintf(stderr, "value=%x\n", mem->intctrl->icip);
      return mem->intctrl->icip;
      break;

      case 0x04:
      fprintf(stderr, "value=%x\n", mem->intctrl->icmr);
      return mem->intctrl->icmr;
      break;

      case 0x08:
      fprintf(stderr, "value=%x\n", mem->intctrl->iclr);
      return mem->intctrl->iclr;
      break;

      case 0x10:
      return mem->intctrl->icfp;
      break;

      case 0x20:
      return mem->intctrl->icpr;
      break;

      case 0x0c:
      return mem->intctrl->iccr;
      break;
    }
    break;
  }
  return 0xdeadbeef;
}

void ostimer_clock(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  meminfo* mem = machine->mem;
  uint5 pc = reg->cpsr.flag.mode<16 ? (reg->r[15] & ~0xfc000003)-8
                                    : reg->r[15]-8;
  instructionformat inst;
  ostimer_registers* oti = mem->ostimer;
  intctrl_registers* icr = mem->intctrl;
  uint5 i, clk;
  uint5 oldstate;

  clk = oti->clock++;

  /* watchdog timer */
  if ((oti->ower & 1)==1 && oti->osmr[3]==clk)
  {
 /*   fprintf(stderr, "Watchdog timer, whoops\n");
    processor_reset(machine);
    return;*/
  }

 /* intctrl_blank(machine, 26);
  intctrl_blank(machine, 27);
  intctrl_blank(machine, 28);
  intctrl_blank(machine, 29);*/

  oldstate = oti->ossr;
  
  for (i=0; i<4; i++)
  {
    if (clk==oti->osmr[i])
    {
      /* set corresponding bit in status register */
      fprintf(stderr, "clk matched %d at time %x\n", i, clk);
      fprintf(stderr, "OIER bit=%s\n", (1<<i) & oti->oier ? "true" : "false");
      oti->ossr |= (1<<i) & oti->oier;
    }
  }

  intctrl_add(machine, (oti->ossr&0xf)<<26);

  /* maybe only fire for 'new' state (rising edge) */
/*  if (oti->ossr & ~oldstate)*/
  {
  //fprintf(stderr, "icip=%.8x, int=%x\n", icr->icip, reg->cpsr.flag.interrupt);

    intctrl_fire(machine);
  }
}
