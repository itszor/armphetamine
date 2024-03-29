#include <stdio.h>
#include <assert.h>

#include "libjtype/cnew.h"
#include "core/registers.h"
#include "core/memory.h"
#include "core/processor.h"
#include "core/machine.h"
#include "core/decode.h"
#include "core/execute.h"

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

void processor_reg_savecurrent(machineinfo* machine, uint5 mode)
{
  uint5 i;
  registerinfo* reg = machine->reg;
  
  switch (mode)
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
}

void processor_reg_restorenew(machineinfo* machine, uint5 mode)
{
  uint5 i;
  registerinfo* reg = machine->reg;

  switch (mode)
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
}

// Switch the current processor mode
void processor_mode(machineinfo* machine, uint5 omode, uint5 newmode)
{
  registerinfo* reg = machine->reg;
  meminfo* mem = machine->mem;
  psrinfo spsr = reg->spsr[reg->spsr_current];

#if 0
  instructionformat inst;
  uint5 instaddr = PCADDR-8;

  if (newmode<16)
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
#endif

  assert(newmode>=16);
  
 /* reg->spsr[omode&15] = reg->cpsr;*/
  processor_reg_savecurrent(machine, omode);
  processor_reg_restorenew(machine, newmode);
/*  reg->cpsr = reg->spsr[newmode&15];*/
  
  reg->cpsr.flag.mode = newmode;
  // keep a copy of current mode for use by memory subsystem
  machine->mem->currentmode = newmode; // & 15;
  
  machine->exectab = newmode<16 ? &exec26 : &exec32;

/*  if (newmode==24) machine->trace = 1;*/

#ifdef PROBABLY_BROKEN
  /* I think this is up to the OS/interrupt handler, not us */
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
}

/* Next intruction to be executed here is pc-8 */
/* following instruction is pc-4 */
void processor_fiq(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  reg->spsr[pm_FIQ32&15] = reg->cpsr;
  fprintf(stderr, "FIQ\n");
  abort();
  processor_mode(machine, reg->cpsr.flag.mode, pm_FIQ32);
  reg->r[14] = reg->r[15]-4;
  reg->cpsr.flag.interrupt = 3;  // disable fiq, irq
  reg->r[15] = reg->vectorbase+0x1C+8;
}

void processor_irq(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  reg->spsr[pm_IRQ32&15] = reg->cpsr;
/*  fprintf(stderr, "IRQ\n");*/
  processor_mode(machine, reg->cpsr.flag.mode, pm_IRQ32);
  reg->r[14] = reg->r[15]-4;
  reg->cpsr.flag.interrupt |= 2;  // disable irq
  reg->r[15] = reg->vectorbase+0x18+8;
}

void processor_prefetchabort(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  reg->spsr[pm_ABT32&15] = reg->cpsr;
  fprintf(stderr, "Prefetch Abort\n");
  abort();
  processor_mode(machine, reg->cpsr.flag.mode, pm_ABT32);
  reg->r[14] = reg->r[15]-4;    /* +/- 4 */
  reg->cpsr.flag.interrupt |= 2;  // disable irq
  reg->r[15] = reg->vectorbase+0x0C+8;
}

void processor_dataabort(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  reg->spsr[pm_ABT32&15] = reg->cpsr;
  fprintf(stderr, "Oh cheese, a data abort\n");
  abort();
  processor_mode(machine, reg->cpsr.flag.mode, pm_ABT32);
  reg->r[14] = reg->r[15];  /* +/- 4 */
  reg->cpsr.flag.interrupt |= 2;  // disable irq
  reg->r[15] = reg->vectorbase+0x10+8;
}

void processor_swi(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  instructionformat inst;
  reg->spsr[pm_SVC32&15] = reg->cpsr;
  fprintf(stderr, "SWI! (vectorbase=%.8x)\n", reg->vectorbase);
  processor_mode(machine, reg->cpsr.flag.mode, pm_SVC32);
  reg->r[14] = reg->r[15]-4;
  fprintf(stderr, "insn: ");
  inst.instruction = memory_readinstword(machine->mem, reg->r[15]-8);
  dispatch(machine, inst, &diss, (void*)(reg->r[15]-8));
  fprintf(stderr, "\n");
  reg->cpsr.flag.interrupt |= 2;  // disable irq
  reg->r[15] = reg->vectorbase+0x08+8;
}

void processor_und(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  fprintf(stderr, "Teh undefined instruction trap!\n");
  reg->spsr[pm_UND32&15] = reg->cpsr;
  processor_mode(machine, reg->cpsr.flag.mode, pm_UND32);
  reg->r[14] = reg->r[15]-4;
  reg->cpsr.flag.interrupt |= 2;  // disable irq
  reg->r[15] = reg->vectorbase+0x04+8;
}

void processor_reset(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  reg->spsr[pm_SVC32&15] = reg->cpsr;
  fprintf(stderr, "Reset!\n");
  processor_mode(machine, reg->cpsr.flag.mode, pm_SVC32);
  reg->r[14] = reg->r[15];
  reg->cpsr.flag.interrupt = 3;  // disable fiq+irq
  reg->r[15] = reg->vectorbase+0x00+8;
}
