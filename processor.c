#include <stdio.h>

#include "cnew.h"
#include "registers.h"
#include "processor.h"
#include "machine.h"
#include "decode.h"

void processor_initialise(void)
{
  /* doesn't do anything */
}

const char* modename_st[] =
{
  "USR26", "FIQ26", "IRQ26", "SVC26", "?", "?", "?", "?",
  "?", "?", "?", "?", "?", "?", "?", "?",
  "USR32", "FIQ32", "IRQ32", "SVC32", "ABT32", "UND32"
};

// Switch the current processor mode
void processor_mode(machineinfo* machine, uint5 newmode)
{
  registerinfo* reg = machine->reg;
  uint5 omode = reg->cpsr.flag.mode&15, i;

  fprintf(stderr, "Switching mode from %s to %s\n",
    modename_st[reg->cpsr.flag.mode], modename_st[newmode]);

  switch (omode)
  {
    case pm_USR26:
    case pm_USR32:
    for (i=8; i<=14; i++) reg->usr[i-8] = reg->r[i];
    break;

    case pm_FIQ26:
    case pm_FIQ32:
    for (i=8; i<=14; i++) reg->fiq[i-8] = reg->r[i];
    break;

    case pm_IRQ26:
    case pm_IRQ32:
    for (i=8; i<=12; i++) reg->usr[i-8] = reg->r[i];
    for (i=13; i<=14; i++) reg->irq[i-13] = reg->r[i];
    break;

    case pm_SVC26:
    case pm_SVC32:
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

  switch (newmode & 15)
  {
    case pm_USR26:
    case pm_USR32:
    for (i=8; i<=14; i++) reg->r[i] = reg->usr[i-8];
    reg->pcmask = 0xf0000000;
    break;
  
    case pm_FIQ26:
    case pm_FIQ32:
    for (i=8; i<=14; i++) reg->r[i] = reg->irq[i-8];
    reg->pcmask = 0xfc000003;
    break;
  
    case pm_IRQ26:
    case pm_IRQ32:
    for (i=8; i<=12; i++) reg->r[i] = reg->usr[i-8];
    for (i=13; i<=14; i++) reg->r[i] = reg->irq[i-13];
    reg->pcmask = 0xfc000003;
    break;

    case pm_SVC26:
    case pm_SVC32:
    for (i=8; i<=12; i++) reg->r[i] = reg->usr[i-8];
    for (i=13; i<=14; i++) reg->r[i] = reg->svc[i-13];
    reg->pcmask = 0xfc000003;
    break;

    case pm_ABT32:
    for (i=8; i<=12; i++) reg->r[i] = reg->usr[i-8];
    for (i=13; i<=14; i++) reg->r[i] = reg->abt[i-13];
    reg->pcmask = 0xfc000003;
    break;

    case pm_UND32:
    for (i=8; i<=12; i++) reg->r[i] = reg->usr[i-8];
    for (i=13; i<=14; i++) reg->r[i] = reg->und[i-13];
    reg->pcmask = 0xfc000003;
    break;
  }

  reg->cpsr.flag.mode = newmode;
  // keep a copy of current mode for use by memory subsystem
  machine->mem->currentmode = newmode & 15;
  
  machine->exectab = newmode<16 ? &exec26 : &exec32;

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
  processor_mode(machine, 0x11);
  reg->r[14] = reg->r[15]-4;
  reg->cpsr.flag.interrupt = 3;  // disable fiq, irq
  reg->r[15] = 0x1C+8;
}

void processor_irq(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  processor_mode(machine, 0x12);
  reg->r[14] = reg->r[15]-4;
  reg->cpsr.flag.interrupt = 2;  // disable irq
  reg->r[15] = 0x18+8;
}

void processor_prefetchabort(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  processor_mode(machine, 0x17);
  reg->r[14] = reg->r[15]-4;
  reg->cpsr.flag.interrupt = 2;  // disable irq
  reg->r[15] = 0x0C+8;
}

void processor_dataabort(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  processor_mode(machine, 0x17);
  reg->r[14] = reg->r[15];
  reg->cpsr.flag.interrupt = 2;  // disable irq
  reg->r[15] = 0x10+8;
}

void processor_swi(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  processor_mode(machine, 0x13);
  reg->r[14] = reg->r[15]-4;
  reg->cpsr.flag.interrupt = 2;  // disable irq
  reg->r[15] = 0x08+8;
}

void processor_und(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  processor_mode(machine, 0x1b);
  reg->r[14] = reg->r[15]-4;
  reg->cpsr.flag.interrupt = 2;  // disable irq
  reg->r[15] = 0x04+8;
}

void processor_reset(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  processor_mode(machine, 0x13);
  reg->r[14] = reg->r[15];
  reg->cpsr.flag.interrupt = 3;  // disable fiq+irq
  reg->r[15] = 0x00+8;
}
