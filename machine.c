#include <stdio.h>
#include <assert.h>

#include "cnew.h"
#include "machine.h"
#include "hash.h"
#include "block.h"
#include "execute.h"
#include "profile.h"

#ifdef EMULART
#  include "sapcm.h"
#  include "ostimer.h"
#endif

machineinfo* machine_create(uint5 memory)
{
	machineinfo* machine = cnew(machineinfo);
	
	machine->mem = memory_initialise(memory);
	machine->reg = registers_new();
//	machine->x86flags.value = 0;
/*	machine->blocks = hash_new(32768);  // pick a number...
	machine->live = 0;
	machine->translaterules = 0;*/
  machine->breakpoints = hash_new(16);
  machine->trace = 0;
  machine->pstate = profile_initialise();
    
	return machine;
}

// emulation loop, debugging version
void machine_start(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  meminfo* mem = machine->mem;
  const uint5 cycperio = 100;
#ifdef RECOMPILE
  uint5 feednewpc = 1;
#endif
#ifdef EMULART
  sint5 serialclock = SERIALCLOCKPERIOD;
#endif

  machine->cycle = cycperio;
  
  for (;;)
  {
    uint5 instaddr;
    instructionformat inst;

#ifdef RECOMPILE
    do {
#endif
      instaddr = PCADDR-8;
      if (reg->cpsr.flag.mode<16) instaddr = instaddr & ~0xfc000003;

#ifdef RECOMPILE
      if (feednewpc)
        feednewpc = profile_feednseqaddr(machine, machine->pstate, instaddr);

    } while (feednewpc);
    
    assert(machine->pstate->start != -1);
#endif
    
    inst.instruction = memory_readinstword(mem, instaddr);
    
    if (machine->trace)
    {
      uint5 x;
      fprintf(stderr, "%.8x : %.8x : ", instaddr, inst.instruction);
      dispatch(machine, inst, &diss, (void*)instaddr);
      fprintf(stderr, "\n");
      for (x=0; x<15; x++)
      {
        fprintf(stderr, "r%d=%.8x ", x, reg->r[x]);
      }
      fprintf(stderr, "%c%c%c%c\n", FLAG(c)?'C':'c', FLAG(v)?'V':'v',
              FLAG(n)?'N':'n', FLAG(z)?'Z':'z');
    }
    
    if (hash_lookup(machine->breakpoints, instaddr))
    {
      fprintf(stderr, "Breakpoint at %.8x\n", instaddr);
      return;
    }
    else
    {
      uint5 retcode = dispatch(machine, inst, machine->exectab, 0);
      if (retcode==1)
      {
        /* Uses instruction ptr in state before execute has mangled it */
#ifdef RECOMPILE
        profile_feedseqaddr(machine->pstate, instaddr);
        /* make sure next instruction loaded (re-)enables a profile block */
        feednewpc = 1;
#endif
      }
    }
      
#ifdef IOMDSUPPORT
    if (--machine->cycle<0)
    {
      if (!--mem->io.timer0)
      {
        fprintf(stderr, "Timer0 triggered!\n");
      }
      if (!--mem->io.timer1)
      {
        fprintf(stderr, "Timer1 triggered!\n");
      }
      machine->cycle+=cycperio;
    }
#endif

#ifdef EMULART
    if (--serialclock<0)
    {
      sapcm_clock(machine);
      ostimer_clock(machine);
      serialclock = SERIALCLOCKPERIOD;
    }
#endif
  }
}

void machine_clone(machineinfo* dest, machineinfo* src)
{
  memory_clone(dest->mem, src->mem);
  registers_clone(dest->reg, src->reg);
  dest->cycle = src->cycle;
}
