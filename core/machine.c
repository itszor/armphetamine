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
	machineinfo* machine = jt_new(machineinfo);
	
	machine->mem = memory_initialise(memory);
	machine->reg = registers_new();
//	machine->x86flags.value = 0;
/*	machine->blocks = hash_new(32768);  // pick a number...
	machine->live = 0;
	machine->translaterules = 0;*/
  machine->breakpoints = jt_hash_new(16);
  machine->trace = 0;
  machine->detracecounter = 0;
  machine->pstate = profile_initialise();
  machine->lastfew = jt_newarray(traceback, 1024);
  machine->posn = 0;
  #ifdef EMULART
  sa1100_serial_initialise(machine->mem);
  #endif

	return machine;
}

#define LOCALFLAG(X) \
  ((mem->currentmode<16) ? (((pcinfo*)&reg->r[15])->flag.X) \
                         : (reg->cpsr.flag.X))

// emulation loop, debugging version
void machine_start(machineinfo* machine, uint5 cont)
{
  registerinfo* reg = machine->reg;
  meminfo* mem = machine->mem;
  const uint5 cycperio = 100;  /* was 100 */
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

/*    memory_invalidatetlb(&mem->datatlb);
    memory_invalidatetlb(&mem->insttlb);*/

#ifdef RECOMPILE
    do {
#endif
      instaddr = PCADDR-8;
      if (reg->cpsr.flag.mode<16) instaddr = instaddr & ~0xfc000003;

#ifdef RECOMPILE
      if (feednewpc)
        feednewpc = profile_feednseqaddr(machine, machine->pstate, instaddr);

    } while (feednewpc);
    
    assert(machine->pstate->start != -1u);
#endif
    
    inst.instruction = memory_readinstword(mem, instaddr);
    
    machine->lastfew[machine->posn].inst = inst.instruction;
    machine->lastfew[machine->posn].virtualaddress = instaddr;

    machine->posn++;
    if (machine->posn>=1024) machine->posn = 0;
    
    if (machine->trace)
    {
      fprintf(stderr, "%.8x : %.8x : ", instaddr, inst.instruction);
      dispatch(machine, inst, &diss, (void*)instaddr);
      fprintf(stderr, "\n");
      if (machine->detracecounter)
      {
        if (!--machine->detracecounter) machine->trace = 0;
      }
    }
        
    if (jt_hash_lookup(machine->breakpoints, instaddr) && cont==0)
    {
      fprintf(stderr, "Breakpoint at %.8x\n", instaddr);
      return;
    }
    else
    {
      uint5 retcode = dispatch(machine, inst, machine->exectab, 0);
      if (machine->trace)
      {
        uint5 x;
        for (x=0; x<15; x++)
        {
          fprintf(stderr, "r%d=%.8x ", x, reg->r[x]);
        }
        fprintf(stderr, "cpsr=%.8x spsr[%d]=%.8x ",
          reg->cpsr.value, reg->spsr_current, 
          reg->spsr[reg->spsr_current].value);
        fprintf(stderr, "%c%c%c%c\n", LOCALFLAG(c)?'C':'c', 
                LOCALFLAG(v)?'V':'v', LOCALFLAG(n)?'N':'n', 
                LOCALFLAG(z)?'Z':'z');
      }
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
    
    // stop continuing from a breakpoint
    if (cont==1) cont = 0;
      
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

    if (cont==2)  // single-step
    {
      fprintf(stderr, "Stepped to %.8x\n", instaddr);
      break;
    }
  }
}

void machine_clone(machineinfo* dest, machineinfo* src)
{
  memory_clone(dest->mem, src->mem);
  registers_clone(dest->reg, src->reg);
  dest->cycle = src->cycle;
}
