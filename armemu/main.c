#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "cnew.h"
#include "defs.h"
#include "machine.h"
#include "memory.h"
#include "decode.h"
#include "loadaout.h"
#include "block.h"
#include "codegen.h"
#include "generators.h"
#include "riscos.h"
#include "execute.h"
#include "pqueue.h"
#ifdef VIRTUALFRAMEBUFFER
#include "SDL.h"
#endif

//#define TRACE 1
#undef TRACE

//#define RECOMPILE 1
#undef RECOMPILE

//#define DIVISIONTEST 1
//#undef DIVISIONTEST

#define BASICTEST 1

//#define LOWER 0x233c
//#define LOWER 0x2370
//#define UPPER 0x23fc
//#define UPPER 0x236c

#define LOWER 0
#define UPPER (4096*1024)

//#define LOWER 0x119c
//#define UPPER 0x11fc

typedef struct {
  char** ps_argvstr;
	int ps_nargvstr;
	char** ps_envstr;
	int ps_nenvstr;
} netbsd_ps_strings;

// copy important fields from one machine to another
void dupstate(machineinfo* from, machineinfo* to)
{
  memcpy(to->reg, from->reg, sizeof(registerinfo));
  memcpy(to->mem->memory, from->mem->memory, 4*1024*1024);
}

void cmpstate(machineinfo* x, machineinfo* y)
{
  uint5 i;
  
  for (i=0; i<16; i++)
  {
    if (x->reg->r[i] != y->reg->r[i])
    {
      fprintf(stderr, "Register %d different (%x != %x)\n", i, x->reg->r[i],
        y->reg->r[i]);
    }
  }
  
  #define FLAGCHECK(q) if (x->reg->q##flag != y->reg->q##flag) { \
    fprintf(stderr, "Flag " #q " different (%d != %d)\n", x->reg->q##flag, \
    y->reg->q##flag); \
  }
  
  FLAGCHECK(c) FLAGCHECK(v) FLAGCHECK(n) FLAGCHECK(z)
  
  for (i=0; i<1024*1024; i++)
  {
    if (x->mem->memory[i] != y->mem->memory[i])
    {
      fprintf(stderr, "Memory contents different at %x (%x != %x)\n", i*4,
        x->mem->memory[i], y->mem->memory[i]);
    }
  }
}

machineinfo* mcopy;

void interpretblock(machineinfo* machine, uint5 base, blockinfo* blk)
{
  uint5 pc;
}

int main(void)
{
	machineinfo* machine;
	uint5 i;
	meminfo* mem;
	registerinfo* reg;
  uint5 cycle = 0, max = 1500;
  FILE* startaddr;

//  machine->mcopy = machine_create(4*1024*1024);

  fprintf(stderr, "ARMphetamine v" VERSION " alpha\n");
#ifdef RECOMPILE
  fprintf(stderr, "Recompilation ON\n");
#else
  fprintf(stderr, "Recompilation OFF\n");
#endif

#ifdef ASM_ALU
  fprintf(stderr, "Assembler ALU\n");
#else
  fprintf(stderr, "C ALU\n");
#endif

  machine = machine_create(4*1024*1024);
  mem = machine->mem;
  reg = machine->reg;

//  fprintf(stderr, "\nInitialising RISC OS API emulation...\n");
  riscosstate = riscos_initialise();
  
//	fprintf(stderr, "Tabulating translation rules...\n");
	machine->translaterules = generators_register();
	
//	fprintf(stderr, "mem->memory=%x\n", (int)mem->memory);

#ifdef VIRTUALFRAMEBUFFER
  fprintf(stderr, "Initialising SDL\n");
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    fprintf(stderr, "Unable to initialise SDL: %s\n", SDL_GetError());
    exit(1);
  }
  atexit(SDL_Quit);
  
  machine->mem->screen = SDL_SetVideoMode(320, 256, 8, SDL_SWSURFACE);
  
  SDL_WM_SetCaption("Sophie v" VERSION, "");
#endif

#ifdef BASICTEST
  fprintf(stderr, "Loading BASIC\n");	
	load_aout(&mem->memory[0x8000], "BASIC");
  reg->r[15] = 0x200dc+8;
#endif

#if defined(CHECKSUMTEST) || defined(DIVISIONTEST)
  fprintf(stderr, "Loading checksum binary\n");	
  reg->r[15] = load_aout(&mem->memory[0x400],
	  "/net/jtb20.quns.cam.ac.uk/home/jules/checksum")+8;
#endif

#ifdef GENERICTEST
  fprintf(stderr, "Loading generic test binary\n");	
  reg->r[15] = load_aout(&mem->memory[0x400],
	  "/net/jtb20.quns.cam.ac.uk/home/jules/test")+8;
#endif

#ifdef EXHAUSTTEST
  fprintf(stderr, "Loading exhaust-emu binary\n");	
  load_aout(&mem->memory[0x400],
    "/net/jtb20.quns.cam.ac.uk/home/jules/exhaust-emu");

  // probably the nastiest hack yet...
  startaddr = fopen("/net/jtb20.quns.cam.ac.uk/home/jules/startaddr", "r");
  fscanf(startaddr, "%x", &reg->r[15]);
  fclose(startaddr);
  reg->r[15] += 8;
#endif

#ifdef DHRYSTONETEST
  fprintf(stderr, "Loading dhry-emu binary\n");	
  load_aout(&mem->memory[0x400],
    "/net/jtb20.quns.cam.ac.uk/home/jules/dhrystone/dhry-emu");
  startaddr = fopen("/net/jtb20.quns.cam.ac.uk/home/jules/dhrystone/startaddr",
                    "r");
  fscanf(startaddr, "%x", &reg->r[15]);
  fclose(startaddr);
  reg->r[15] += 8;
#endif

//  reg->r[15] = 0x1010+8;

	reg->r[13] = 2*1024*1024;  // stack

/*	puts("Disassembling...");

	for (i=0x8000; i<0x9000; i++)
	{
		instructionformat ins;
		ins.instruction = mem->memory[i];
		dispatch(machine, ins, &diss, 0);
    fprintf(stderr, "\n");
	}*/

#ifdef DIVISIONTEST
#define BASE 0x1184
#define LENGTH 0x17
  reg->r[15] = BASE+8;
  {
    blockinfo* block = block_new();
    hashentry* h = hash_insert(machine->blocks, BASE);
    h->data = block;
    machine->live = h;
    block->length = LENGTH;
    block->psb = 0;
    block->lastset = block->nextused = 0;
    block->native = 0;
    pseudo_translateblock(machine, block, BASE);
    analyse_block(block);
    codegen_recompile(machine, block);
    pseudodism_block(block->psb);
    block_clean(block);
    x86dism_block(block->native);
    reg->r[0] = 999;
    reg->r[1] = 6;
    printf("%d/%d=", reg->r[0], reg->r[1]);
    nativesupport_invoke(machine, block->native);
    printf("%d (rem %d)\n", reg->r[0], reg->r[1]);
  }
  exit(0);
#endif

	fprintf(stderr, "Beginning emulation at %x...\n", reg->r[15]-8);

	while (1)
	{
    uint5 instaddr = PCADDR-8, x;
#ifdef RECOMPILE
    if (!block_gather(machine, instaddr))
    {
#endif
      uint5* instptr = memory_lookup(mem, (uint5*)instaddr);
		  instructionformat ins;
		  ins.instruction = *instptr;
  #ifdef TRACE
		  fprintf(stderr, "PC-8=%x %x ", instaddr, instptr);
		  dispatch(machine, ins, &diss, 0);
      fprintf(stderr, "\n");
  #endif
      dispatch(machine, ins, &exec, 0);
  #ifdef TRACE
      for (x=0; x<15; x++)
      {
        fprintf(stderr, "r%d=%x ", x, reg->r[x]);
      }
      fprintf(stderr, "%c%c%c%c\n", FLAG(c)?'C':'c', FLAG(v)?'V':'v',
              FLAG(n)?'N':'n', FLAG(z)?'Z':'z');
  #endif
#ifdef RECOMPILE
    }
#endif
  }

#ifdef OLDSTUFF

#ifdef TRACE
		fprintf(stderr, "PC-8=%x %x ", instaddr, instptr);
		dispatch(machine, ins, &diss, 0);
    fprintf(stderr, "\n");
#endif



#ifdef RECOMPILE
/*    if (instaddr>=LOWER && instaddr<=UPPER)
    {*/
//      fprintf(stderr, "Executing code at %x\n", instaddr);
/*    }
    else
      dispatch(machine, ins, &exec, 0);*/
#else
		dispatch(machine, ins, &exec, 0);
#endif
/*        if (reg->r[15]-8==0x210a4)
	    {
          for (i=0; i<15; i++) fprintf(stderr, "r%d=%x ", i, reg->r[i]);
		  exit(1);
		}*/

#ifdef TRACE
    fprintf(stderr, "    [%s%s%s%s]\n", FLAG(c)?"C":"c", FLAG(v)?"V":"v",
      FLAG(n)?"N":"n", FLAG(z)?"Z":"z");
//		r = getchar();
/*    r='r';
		if (r=='r' || r=='R')
		{*/
	  for (i=0; i<15; i++) fprintf(stderr, "r%d=%x ", i, reg->r[i]);
    fprintf(stderr, "\n");
//			r = getchar();
/*    }*/
#endif
#ifdef DEBUG
/*    cycle++;
    if (cycle>max) exit(0);*/
#endif
	}

#endif

	return 0;
}
