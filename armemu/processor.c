#include <stdio.h>

#include "cnew.h"
#include "registers.h"
#include "processor.h"

void processor_initialise(void)
{
  /* doesn't do anything */
}

// Switch the current processor mode
void processor_mode(registerinfo* reg, int newmode)
{
  const int rmodebase[] = {7, 7, 2, 2, 2, 2};
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
	}
}

void processor_fiq(registerinfo* reg)
{
  processor_mode(reg, 0x11);
  reg->r[14] = reg->r[15]-4;
  reg->cpsr.flag.interrupt = 3;  // disable fiq, irq
  reg->r[15] = 0x1C+8;
}

void processor_irq(registerinfo* reg)
{
  processor_mode(reg, 0x12);
  reg->r[14] = reg->r[15]-4;
  reg->cpsr.flag.interrupt = 2;  // disable irq
  reg->r[15] = 0x18+8;
}

void processor_prefetchabort(registerinfo* reg)
{
  processor_mode(reg, 0x17);
  reg->r[14] = reg->r[15]-4;
  reg->cpsr.flag.interrupt = 2;  // disable irq
  reg->r[15] = 0x0C+8;
}

void processor_dataabort(registerinfo* reg)
{
  processor_mode(reg, 0x17);
  reg->r[14] = reg->r[15];
  reg->cpsr.flag.interrupt = 2;  // disable irq
  reg->r[15] = 0x10+8;
}

void processor_swi(registerinfo* reg)
{
  processor_mode(reg, 0x13);
  reg->r[14] = reg->r[15]-4;
  reg->cpsr.flag.interrupt = 2;  // disable irq
  reg->r[15] = 0x08+8;
}

void processor_und(registerinfo* reg)
{
  processor_mode(reg, 0x1b);
  reg->r[14] = reg->r[15]-4;
  reg->cpsr.flag.interrupt = 2;  // disable irq
  reg->r[15] = 0x04+8;
}

void processor_reset(registerinfo* reg)
{
  processor_mode(reg, 0x13);
  reg->r[14] = reg->r[15];
  reg->cpsr.flag.interrupt = 3;  // disable fiq+irq
  reg->r[15] = 0x00+8;
}
