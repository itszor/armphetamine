#include <stdio.h>

#include "cnew.h"
#include "machine.h"
#include "hash.h"
#include "block.h"
#include "execute.h"

machineinfo* machine_create(uint5 memory)
{
	machineinfo* machine = cnew(machineinfo);
	
	machine->mem = memory_initialise(memory);
	machine->reg = registers_new();
//	machine->x86flags.value = 0;
	machine->blocks = hash_new(32768);  // pick a number...
	machine->live = 0;
	machine->translaterules = 0;
  machine->breakpoints = hash_new(16);
  machine->trace = 0;
	
	return machine;
}

// emulation loop, debugging version
void machine_start(machineinfo* machine)
{
  registerinfo* reg = machine->reg;
  meminfo* mem = machine->mem;
  const uint5 cycperio = 100;

  machine->cycle = cycperio;
  
  for (;;)
  {
    uint5 instaddr = PCADDR-8;
    instructionformat inst;
    inst.instruction = memory_readinstword(mem, instaddr);
    
    if (machine->trace)
    {
      fprintf(stderr, "%.8x : %.8x : ", instaddr, inst.instruction);
      dispatch(machine, inst, &diss, 0);
      fprintf(stderr, "\n");
    }
    
    if (hash_lookup(machine->breakpoints, instaddr))
    {
      fprintf(stderr, "Breakpoint at %.8x\n", instaddr);
      return;
    }
    else
    {
      dispatch(machine, inst, machine->exectab, 0);
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
    }
  }
}
