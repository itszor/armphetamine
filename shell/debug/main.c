#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "libjtype/cnew.h"
#include "libjtype/defs.h"
#include "core/machine.h"
#include "core/memory.h"
#include "core/decode.h"
#include "dynarec/block.h"
#include "core/execute.h"
#include "libjtype/pqueue.h"
#include "debug.h"
#include "dynarec/arch/x86/genx86.h"

#ifdef VIRTUALFRAMEBUFFER
#include "SDL.h"
#endif

#ifdef ROHLE
#include "riscos.h"
#endif

//#define TRACE 1

//#define DIVISIONTEST 1
//#undef DIVISIONTEST

//#define BASICTEST 1
//#define BSD_KERNEL 1
//#define LIVEROM 1
#define DEBUGGER 1

//#define LOWER 0x233c
//#define LOWER 0x2370
//#define UPPER 0x23fc
//#define UPPER 0x236c

#define LOWER 0
#define UPPER (4096*1024)

//#define LOWER 0x119c
//#define UPPER 0x11fc

int main(int argc, char* argv[])
{
	machineinfo* machine;
	meminfo* mem;
	registerinfo* reg;
#ifdef CTRACE
  FILE* trc = fopen("trace.dat", "w");
#endif

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

  /* minor sanity check */
  assert(sizeof(instructionformat)==4);
  assert(sizeof(psrinfo)==4);

  machine = machine_create(4*1024*1024);
  mem = machine->mem;
  reg = machine->reg;
  mem->parent = machine;

#ifdef VIDCSUPPORT
  fprintf(stderr, "Initialising VIDC20\n");
  vidc20_init(mem);
#endif

/*  genx86_test();*/

//  fprintf(stderr, "\nInitialising RISC OS API emulation...\n");
#ifdef ROHLE
  riscosstate = riscos_initialise();
#endif
  
//	fprintf(stderr, "Tabulating translation rules...\n");
/*	machine->translaterules = generators_register();*/
	
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
  
  SDL_WM_SetCaption("ARMphetamine v" VERSION, "");
#endif

#ifdef BASICTEST
  fprintf(stderr, "Loading BASIC\n");	
 // load_aout(&mem->memory[0x8000], "BASIC");
  rom = fopen("BASIC", "r");
  if (!rom)
  {
    fprintf(stderr, "Can't load BASIC!\n");
    exit(1);
  }
  fread(&mem->rom0[0x8000], 1, 4*1024*1024, rom);
  fclose(rom);
  fprintf(stderr, "Attempting to start BASIC...\n");
  processor_reset(machine);
  reg->r[15] = 0x200dc+8;
  reg->r[13] = 0x1000000 + 4086*1024;
#endif

#ifdef LIVEROM
  fprintf(stderr, "Loading ROM\n");
  rom = fopen("Rom", "r");
  if (!rom)
  {
    fprintf(stderr, "Can't load ROM!\n");
    exit(1);
  }
  fread(mem->rom0, 1, 4*1024*1024, rom);
  /*fread(&mem->rom0[0x200000/4], 1, 1*1024*1024, rom);
  fread(&mem->rom1[0], 1, 1*1024*1024, rom);
  fread(&mem->rom1[0x200000/4], 1, 1*1024*1024, rom);*/
  fclose(rom);
  fprintf(stderr, "Attempting to boot RISC OS\n");
  fprintf(stderr, "Patching ROM...\n");
  mem->rom0[0x20/4] = 0xe3a0b632;  // mov r11, 0x03200000
  mem->rom0[0x24/4] = 0xe3a00000;  // mov r0, #0x0
  mem->rom0[0x28/4] = 0xe5cb0080;  // strb r0,[r11,0x80]  ; 32-bit ROM
  mem->rom0[0x2c/4] = 0xe3a0f000;  // mov pc,#0
  mem->rom0[0x30/4] = mem->rom0[0x34/4] = mem->rom0[0x38/4] = mem->rom0[0x3c/4]
                    = 0;  // andeq r0,r0,r0?
  processor_reset(machine);
  reg->r[15] = 0xc0000020+8;
#endif

#if defined(LINUX_KERNEL) || defined(BSD_KERNEL)
  fprintf(stderr, "Loading kernel...\n");
  rom = fopen("Kernel", "r");
  if (!rom)
  {
    fprintf(stderr, "Can't load image!\n");
    exit(1);
  }
  fread(mem->rom0, 1, 4*1024*1024, rom);
  /*fread(&mem->rom0[0x200000/4], 1, 1*1024*1024, rom);
  fread(&mem->rom1[0], 1, 1*1024*1024, rom);
  fread(&mem->rom1[0x200000/4], 1, 1*1024*1024, rom);*/
  fclose(rom);
  fprintf(stderr, "Attempting to boot kernel\n");
  processor_reset(machine);
  #ifdef BSD_KERNEL
  reg->r[15] = 0x8;
  #else
  reg->r[15] = 0xc0008000+8;
  #endif
#endif                                                                               

#if defined(CHECKSUMTEST) || defined(DIVISIONTEST)
  fprintf(stderr, "Loading checksum binary\n");	
  reg->r[15] = load_aout(&mem->memory[0x400],
	  "/net/jtb20.quns.cam.ac.uk/home/jules/checksum")+8;
#endif

#ifdef GENERICTEST
  fprintf(stderr, "Loading generic test binary\n");	
  reg->r[15] = load_aout(&mem->memory[0x2000],
	  "dhry-src/dhry")+8;
  reg->r[15] = 0x8034+8;
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

//	reg->r[13] = 2*1024*1024;  // stack

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

#ifdef DEBUGGER
  processor_reset(machine);
  {
    int i;
    for (i=1; i<argc; i++)
    {
      debug_command(machine, argv[i]);
    }
  }
  debug_shell(machine);
#endif

  fprintf(stderr, "Beginning emulation at %x...\n", reg->r[15]-8);

  while (1)
  {
    uint5 instaddr = PCADDR-8;
#ifdef RECOMPILE2
    if (!block_gather(machine, instaddr))
    {
#endif
      //uint5* instptr = memory_lookup(mem, (uint5*)instaddr);
		  instructionformat ins;
		  ins.instruction = memory_readinstword(mem, instaddr);
  #ifdef TRACE
		  fprintf(stderr, "PC-8=%x [%.8x] ", instaddr, ins.instruction);
		  dispatch(machine, ins, &diss, 0);
      fprintf(stderr, "\n");
  #endif
      dispatch(machine, ins, machine->exectab, 0);
  #ifdef CTRACE
    fprintf(trc, "%x,%x,", instaddr, ins.instruction);
    for (rnum=0; rnum<15; rnum++) fprintf(trc, "%x ", reg->r[rnum]);
    fprintf(trc, "\n");
  #endif
  #ifdef TRACE
      for (x=0; x<15; x++)
      {
        fprintf(stderr, "r%d=%x ", x, reg->r[x]);
      }
      fprintf(stderr, "%c%c%c%c\n", FLAG(c)?'C':'c', FLAG(v)?'V':'v',
              FLAG(n)?'N':'n', FLAG(z)?'Z':'z');
  #endif
#ifdef RECOMPILE2
    }
#endif
  }

	return 0;
}
