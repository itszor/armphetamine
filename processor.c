#include <stdio.h>

#include "cnew.h"
#include "registers.h"
#include "memory.h"
#include "processor.h"
#include "machine.h"
#include "decode.h"
#include "execute.h"

void processor_initialise(void)
{
  /* doesn't do anything */
}

const char* modename_st[] =
{
  "USR26", "FIQ26", "IRQ26", "SVC26",
  "XXX4", "XXX5", "XXX6", "XXX7", "XXX8", "XXX9",  "XXX10", "XXX11",
  "XXX12", "XXX13", "XXX14", "XXX15",
  "USR32", "FIQ32", "IRQ32", "SVC32", "XXX20", "XXX21", "XXX22",
  "ABT32", "XXX24", "XXX25", "XXX26",
  "UND32", "XXX28", "XXX29", "XXX30", "XXX31"
};

// Switch the current processor mode
void processor_mode(machineinfo* machine, uint5 newmode)
{
  registerinfo* reg = machine->reg;
  meminfo* mem = machine->mem;
  uint5 omode = reg->cpsr.flag.mode, i;
  uint5 reducemode = omode>=16 && newmode<16;
  uint5 increasemode = omode<16 && newmode>=16;
  uint5 instaddr = PCADDR-8;
  instructionformat inst;
  psrinfo spsr = reg->spsr[reg->spsr_current];

  if (omode!=newmode)
  {
    fprintf(stderr, "Switching mode from %s to %s\n",
      modename_st[omode], modename_st[newmode]);

    if (instaddr<0xffffff00)
    {
      int i;
      if (omode<16) instaddr = instaddr & ~0xfc000003;

      fprintf(stderr, "Disassembling around %.8x\n", instaddr);

      for (i=-32; i<32; i+=4)
      {
        inst.instruction = memory_readinstword(mem, instaddr+i);

      /*
        if (newmode==pm_USR26)
        {
          machine->trace = 1;
          machine->detracecounter = 6;
        }*/

/*        machine->trace = 1;
        machine->detracecounter = 0;*/

        fprintf(stderr, "+ %.8x : %.8x : ", instaddr+i, inst.instruction);
        dispatch(machine, inst, &diss, (void*)(instaddr+i));
        fprintf(stderr, "\n");
      }
    }
  }

  if ((newmode&15)>0)
  {
    fprintf(stderr, "Storing cpsr %.8x to spsr[%d]\n", reg->cpsr,
      newmode&15);
    if (newmode != omode) reg->spsr[newmode&15] = reg->cpsr;
  }

  switch (omode)
  {
    case pm_USR32:
    case pm_USR26:
    for (i=8; i<=14; i++) reg->usr[i-8] = reg->r[i];
    break;

    case pm_FIQ32:
    case pm_FIQ26:
    for (i=8; i<=14; i++) reg->fiq[i-8] = reg->r[i];
    break;

    case pm_IRQ32:
    case pm_IRQ26:
    for (i=8; i<=12; i++) reg->usr[i-8] = reg->r[i];
    for (i=13; i<=14; i++) reg->irq[i-13] = reg->r[i];
    break;

    case pm_SVC32:
    case pm_SVC26:
    for (i=8; i<=12; i++) reg->usr[i-8] = reg->r[i];
    for (i=13; i<=14; i++) reg->svc[i-13] = reg->r[i];
    break;

    case pm_ABT32:
    for (i=8; i<=12; i++) reg->usr[i-8] = reg->r[i];
    for (i=13; i<=14; i++) reg->abt[i-13] = reg->r[i];
    break;

    case pm_UND32:
    for (i=8; i<=12; i++) reg->usr[i-8] = reg->r[i];
    for (i=13; i<=14; i++) reg->und[i-13] = reg->r[i];
    break;
  }

  switch (newmode)
  {
    case pm_USR32:
    case pm_USR26:
    for (i=8; i<=14; i++) reg->r[i] = reg->usr[i-8];
    // ignore bits 0,1,26,27 on LDM {...pc...}^
    reg->pcmask = 0x0c000003;
    reg->spsr_current = 0;
    break;
  
    case pm_FIQ32:
    case pm_FIQ26:
    for (i=8; i<=14; i++) reg->r[i] = reg->fiq[i-8];
    reg->pcmask = 0x0;
    reg->spsr_current = 1;
    break;
  
    case pm_IRQ32:
    case pm_IRQ26:
    for (i=8; i<=12; i++) reg->r[i] = reg->usr[i-8];
    for (i=13; i<=14; i++) reg->r[i] = reg->irq[i-13];
    reg->pcmask = 0x0;
    reg->spsr_current = 2;
    break;

    case pm_SVC32:
    case pm_SVC26:
    for (i=8; i<=12; i++) reg->r[i] = reg->usr[i-8];
    for (i=13; i<=14; i++) reg->r[i] = reg->svc[i-13];
    reg->pcmask = 0x0;
    reg->spsr_current = 3;
    break;

    case pm_ABT32:
    for (i=8; i<=12; i++) reg->r[i] = reg->usr[i-8];
    for (i=13; i<=14; i++) reg->r[i] = reg->abt[i-13];
    reg->pcmask = 0x0;
    reg->spsr_current = 4;
    break;

    case pm_UND32:
    for (i=8; i<=12; i++) reg->r[i] = reg->usr[i-8];
    for (i=13; i<=14; i++) reg->r[i] = reg->und[i-13];
    reg->pcmask = 0x0;
    reg->spsr_current = 5;
    break;
  }

  reg->cpsr.flag.mode = newmode;
  // keep a copy of current mode for use by memory subsystem
  machine->mem->currentmode = newmode; // & 15;
  
  machine->exectab = newmode<16 ? &exec26 : &exec32;

/*  if (newmode==24) machine->trace = 1;*/

#ifdef PROBABLY_BROKEN
  if (reducemode)
  {
    // absolutely no idea if this is right
    reg->r[15] = (reg->r[15] & ~0xfc000003) | (reg->cpsr.value & 0xf0000003) |
                 ((reg->cpsr.value & 0xc0)<<20);
  }

  if (increasemode)
  {
    /* address */
    reg->r[15] = (reg->r[15] & ~0xfc000003);
    /* flags */
    reg->cpsr.value = (reg->cpsr.value & ~0xf0000000)
                    | (reg->r[15]      &  0xf0000000);
    /* interrupt */
    reg->cpsr.value = (reg->cpsr.value & ~0x000000c0)
                    | ((reg->r[15]>>20) & 0xc0);
  }
#endif
  
/*  if (increasemode)
  {
    reg->cpsr.value = 
  }*/

/*  const int rmodebase[] = {7, 7, 2, 2, 2, 2};
  uint5 omode = reg->cpsr.flag.mode&15;
  int i;
  
  if (reg->cpsr.flag.mode != newmode)
  {
    uint5 nmode = newmode & 0xf;
    int sw = MIN(rmodebase[omode], rmodebase[nmode]);

    // store CPSR in SPSR_<oldmode>
    if (omode>0) reg->spsr[omode-1] = reg->cpsr;
    
    // switch out current mode's register bank
    for (i=0; i<sw; i++)
      reg->rbank[omode][i] = reg->r[i+(15-rmodebase[omode])];

    // retrieve CPSR from SPSR_<newmode>
    if (nmode>0) reg->cpsr = reg->spsr[nmode-1];

    // switch in new mode's register bank
    for (i=0; i<sw; i++)
      reg->r[i+(15-rmodebase[nmode])] = reg->rbank[nmode][i];

    // do the mode switch
    reg->cpsr.flag.mode = newmode;
  }*/
}

void processor_fiq(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  processor_mode(machine, pm_FIQ32);
  reg->r[14] = reg->r[15];
  reg->cpsr.flag.interrupt = 3;  // disable fiq, irq
  reg->r[15] = 0x1C+8;
}

void processor_irq(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  processor_mode(machine, pm_IRQ32);
  reg->r[14] = reg->r[15];
  reg->cpsr.flag.interrupt |= 2;  // disable irq
  reg->r[15] = 0x18+8;
}

void processor_prefetchabort(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  processor_mode(machine, pm_ABT32);
  reg->r[14] = reg->r[15]-4;    /* +/- 4 */
  reg->cpsr.flag.interrupt |= 2;  // disable irq
  reg->r[15] = 0x0C+8;
}

void processor_dataabort(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  processor_mode(machine, pm_ABT32);
  reg->r[14] = reg->r[15];  /* +/- 4 */
  reg->cpsr.flag.interrupt |= 2;  // disable irq
  reg->r[15] = 0x10+8;
}

void processor_swi(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  processor_mode(machine, pm_SVC32);
  reg->r[14] = reg->r[15]-4;
  reg->cpsr.flag.interrupt |= 2;  // disable irq
  reg->r[15] = 0x08+8;
}

void processor_und(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  processor_mode(machine, pm_UND32);
  reg->r[14] = reg->r[15]-4;
  reg->cpsr.flag.interrupt |= 2;  // disable irq
  reg->r[15] = 0x04+8;
}

void processor_reset(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  processor_mode(machine, pm_SVC32);
  reg->r[14] = reg->r[15];
  reg->cpsr.flag.interrupt = 3;  // disable fiq+irq
  reg->r[15] = 0x00+8;
}
