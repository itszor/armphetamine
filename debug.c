#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdarg.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <readline.h>
#include <history.h>

#include "defs.h"
#include "machine.h"
#include "debug.h"
#include "execute.h"
#include "memory.h"
#include "decode.h"
#include "pheta.h"
#include "phetadism.h"
#include "vidc20.h"
#include "iomd.h"
#include "palloc.h"

void debug_shell(machineinfo* machine)
{
  processor_reset(machine);
  while (1)
  {
    char* buf = readline("debug> ");
    if (buf)  // ctrl-D makes readline return zero?
    {
      debug_command(machine, buf);
      free(buf);
    }
    else exit(0);
  }
}

const static debug_cmd commands[] =
{
  { "run",         debug_RUN,         debug_run         },
  { "disassemble", debug_DISASSEMBLE, debug_disassemble },
  { "memory",      debug_MEMORY,      debug_memory      },
  { "registers",   debug_REGISTERS,   debug_registers   },
  { "setreg",      debug_SETREG,      debug_setreg      },
  { "breakset",    debug_BREAKSET,    debug_breakset    },
  { "breakclear",  debug_BREAKCLEAR,  debug_breakclear  },
  { "breaklist",   debug_BREAKLIST,   debug_breaklist   },
  { "trace",       debug_TRACE,       debug_trace       },
  { "load",        debug_LOAD,        debug_load        },
  { "romload",     debug_ROMLOAD,     debug_romload     },
  { "script",      debug_SCRIPT,      debug_script      },
  { "virtual",     debug_VIRTUAL,     debug_virtual     },
  { "phetatrans",  debug_PHETATRANS,  debug_phetatrans  },
  { "quit",        debug_QUIT,        debug_quit        },
  { "help",        debug_HELP,        debug_help        }
};

// Euch, this is quite horrible. And it doesn't work properly.
void debug_command(machineinfo* machine, char* cmd)
{
  int com, uniq = 0, upto, dothis = -1;

  while (isspace(*cmd)) cmd++;

  if (!strcmp(cmd, "")) return;
  
  for (upto=1; cmd[upto] && !isspace(cmd[upto]); upto++);
  
  for (com=0; com<debug_COMMANDS; com++)
  {
    if (!strncasecmp(cmd, commands[com].command, upto))
    {
      uniq++;
      dothis = com;
    }
  }

  if (uniq>1)
  {
    int i;
    fprintf(stderr, "Ambiguous command '%s'\nPossibilities: ", cmd);
    
    for (i=0; i<debug_COMMANDS; i++)
    {
      if (!strncasecmp(cmd, commands[i].command, upto))
      {
        fprintf(stderr, "%s ", commands[i].command);
      }
    }
    fprintf(stderr, "\n");
    return;
  }
  else if (uniq==0)
  {
    fprintf(stderr, "Unknown command '%s'\n", cmd);
    return;
  }
  else if (dothis != -1)
  {
    commands[dothis].exec(machine, cmd);
  }
}

int debug_getnum(char* cmd)
{
  int val;
  if (sscanf(cmd, " 0x%x", &val)) return val;
  if (sscanf(cmd, " %d", &val)) return val;
}

char* debug_getstring(char* cmd)
{
  return cmd;
}

int debug_getreg(char* cmd)
{
  int val;
  if (sscanf(cmd, " cpsr")) return 16;
  if (sscanf(cmd, " spsr")) return 17;
  if (sscanf(cmd, " flags")) return 18;
  if (sscanf(cmd, " r%d", &val)) return val;
  if (sscanf(cmd, " %d", &val)) return val;
}

int debug_getyesno(char* cmd)
{
  if (sscanf(cmd, " yes")) return 1;
  if (sscanf(cmd, " no")) return 0;
  return -1;
}

int debug_hexargs(char* cmd, ...)
{
  va_list ap;
  int args = 0;

  strsep(&cmd, " \t");
  
  va_start(ap, cmd);
  while (cmd)
  {
    if (!sscanf(cmd, "%x", va_arg(ap, uint5*)))
    {
      fprintf(stderr, "Bad hex\n");
      break;
    }
    args++;
    strsep(&cmd, " \t");
  }
  va_end(ap);
  
  return args;
}

void debug_run(machineinfo* machine, char* cmd)
{
  registerinfo* reg = machine->reg;
  uint5 start, end, got;

  got = debug_hexargs(cmd, &start, &end);
  
  switch (got)
  {
    case 0:
    fprintf(stderr, "Running from zero\n");
    break;
    
    case 1:
    fprintf(stderr, "Running from %x\n", start);
    PCSETADDR(start+8);
    break;
    
    case 2:
    fprintf(stderr, "Running from %x to %x\n", start, end);
    PCSETADDR(start+8);
    fprintf(stderr, "(to do this, set a breakpoint first)\n");
    break;
  }
  
  machine_start(machine);
}

void debug_disassemble(machineinfo* machine, char* cmd)
{
  uint5 start, end, got;
  
  got = debug_hexargs(cmd, &start, &end);
  
  start &= ~3;
  end &= ~3;
  
  switch (got)
  {
    case 0:
    fprintf(stderr, "Please specify a start address or range\n");
    return;
    break;
    
    case 1:
    end = start+32;
    // deliberate fall-through
    
    case 2:
    fprintf(stderr, "Disassembling from %x to %x\n", start, end);
    break;
  }
  
  for (; start<end; start+=4)
  {
    uint5 word = memory_readinstword(machine->mem, start);
    instructionformat inst;
    inst.instruction = word;
    fprintf(stderr, "%.8x : %.8x : ", start, word);
    dispatch(machine, inst, &diss, 0);
    fprintf(stderr, "\n");
  }
}

void debug_memory(machineinfo* machine, char* cmd)
{
  uint5 start, end, got;

  got = debug_hexargs(cmd, &start, &end);

  switch (got)
  {
    case 0:
    fprintf(stderr, "Please specify a start address or range\n");
    break;

    case 1:
    end = start+32;
    // deliberate fall-through

    case 2:
    fprintf(stderr, "Dumping memory from %x to %x\n", start, end);
    break;
  }
}

void debug_registers(machineinfo* machine, char* cmd)
{
  int i;
  registerinfo* reg = machine->reg;
  extern const char* modename_st[];
  
  for (i=0; i<16; i++)
  {
    fprintf(stderr, "  r%-2d = 0x%.8x", i, reg->r[i]);
    if ((i&3)==3) fprintf(stderr, "\n");
  }
  fprintf(stderr, "flags = %c%c%c%c", FLAG(c) ? 'C' : 'c',
                                      FLAG(v) ? 'V' : 'v',
                                      FLAG(n) ? 'N' : 'n',
                                      FLAG(z) ? 'Z' : 'z');
  fprintf(stderr, "       cpsr = 0x%.8x", reg->cpsr.value);
  fprintf(stderr, " spsr = 0x%.8x",
    reg->spsr[machine->mem->currentmode & 0xf].value);
  fprintf(stderr, " mode = %s\n", modename_st[machine->mem->currentmode]);
}

void debug_setreg(machineinfo* machine, char* cmd)
{
  registerinfo* reg = machine->reg;
  uint5 regno, value;
  strsep(&cmd, " \t");
  
  if (cmd)
  {
    if (!sscanf(cmd, "%d", &regno))
    {
      fprintf(stderr, "Bad number\n");
      return;
    }
    strsep(&cmd, " \t");
    if (cmd)
    {
      if (!sscanf(cmd, "%x", &value))
      {
        fprintf(stderr, "Bad hex\n");
        return;
      }
      RPUT(regno, value);
    }
  }
}

void debug_breakset(machineinfo* machine, char* cmd)
{
  uint5 addr, got;
  
  got = debug_hexargs(cmd, &addr);

  if (got != 1)
  {
    fprintf(stderr, "Give an address\n");
    return;
  }
  
  hash_insert(machine->breakpoints, addr);
  fprintf(stderr, "Breakpoint set at %.8x\n", addr);
}

void debug_breakclear(machineinfo* machine, char* cmd)
{
  uint5 addr, got;
  
  got = debug_hexargs(cmd, &addr);
  
  if (got != 1)
  {
    fprintf(stderr, "Give an address\n");
    return;
  }
  
  if (hash_lookup(machine->breakpoints, addr))
  {
    hash_remove(machine->breakpoints, addr);
    fprintf(stderr, "Removed breakpoint at %.8x\n", addr);
  }
  else
  {
    fprintf(stderr, "No breakpoint at %.8x\n", addr);
  }
}

void debug_breaklist(machineinfo* machine, char* cmd)
{
  hashtable* hash = machine->breakpoints;
  list* w;
  uint5 i, num=0;
  
  fprintf(stderr, "Breakpoints at:\n");
  
  for (i=0; i<hash->size; i++)
  {
    for (w = hash->table[i]; w; w = w->prev)
    {
      hashentry* e = w->data;
      fprintf(stderr, "  %.8x\n", e->key);
      num++;
    }
  }
  if (!num)
  {
    fprintf(stderr, "  (none set)\n");
  }
}

void debug_trace(machineinfo* machine, char* cmd)
{
  strsep(&cmd, " \t");
  if (!cmd)
  {
    fprintf(stderr, "Expecting argument\n");
    return;
  }
  if (!strcasecmp(cmd, "on"))
  {
    machine->trace = 1;
    fprintf(stderr, "Trace enabled\n");
  }
  else if (!strcasecmp(cmd, "off"))
  {
    machine->trace = 0;
    fprintf(stderr, "Trace disabled\n");
  }
  else
  {
    fprintf(stderr, "Expecting 'on' or 'off'\n");
  }
}

void debug_load(machineinfo* machine, char* cmd)
{
  struct stat fileinfo;
  char filename[60];  // whee, buffer overflows ahead
  char* m;
  FILE* f;
  uint5 addr, i;
  
  strsep(&cmd, " \t");
  if (!cmd)
  {
    fprintf(stderr, "Enter a filename\n");
    return;
  }
  sscanf(cmd, "%s", &filename[0]);
  
  stat(filename, &fileinfo);
  
  strsep(&cmd, " \t");
  if (!cmd)
  {
    fprintf(stderr, "Enter an address\n");
    return;
  }
  sscanf(cmd, "%x", &addr);
  
  m = malloc(fileinfo.st_size);
  f = fopen(filename, "r");
  if (!f)
  {
    fprintf(stderr, "Bad filename '%s'\n", filename);
    return;
  }
  fread(m, fileinfo.st_size, 1, f);
  fclose(f);

  for (i=0; i<fileinfo.st_size; i++)
    memory_writebyte(machine->mem, addr+i, m[i]);
  
  free(m);
  fprintf(stderr, "Loaded file '%s' to %x\n", filename, addr);
}

void debug_romload(machineinfo* machine, char* cmd)
{
  struct stat fileinfo;
  char filename[60];  // whee, buffer overflows ahead
  char* m;
  FILE* f;
  uint5 addr, i;
  
  strsep(&cmd, " \t");
  if (!cmd)
  {
    fprintf(stderr, "Enter a filename\n");
    return;
  }
  sscanf(cmd, "%s", &filename[0]);
  
  stat(filename, &fileinfo);
  
  strsep(&cmd, " \t");
  if (!cmd)
  {
    fprintf(stderr, "Enter an address\n");
    return;
  }
  sscanf(cmd, "%x", &addr);
  
  m = malloc(fileinfo.st_size);
  f = fopen(filename, "r");
  if (!f)
  {
    fprintf(stderr, "Bad filename '%s'\n", filename);
    return;
  }
  fread(m, fileinfo.st_size, 1, f);
  fclose(f);

  for (i=0; i<fileinfo.st_size; i++)
    ((uint3*)machine->mem->rom0)[i] = m[i];
  
  free(m);
  fprintf(stderr, "Loaded ROM '%s' to ROM0:%x\n", filename, addr);
}

void debug_script(machineinfo* machine, char* cmd)
{
  char filename[60];
  char buf[100];
  FILE* f;
  
  strsep(&cmd, " \t");
  if (!cmd)
  {
    fprintf(stderr, "Bad filename\n");
    return;
  }
  sscanf(cmd, "%s", &filename[0]);
  
  f = fopen(filename, "r");
  if (!f)
  {
    fprintf(stderr, "Can't find file '%s'\n", filename);
    return;
  }

  for (;;)
  {
    fgets(buf, 100, f);
    if (feof(f)) break;
    if (strlen(buf)>0) buf[strlen(buf)-1] = 0;  // remove linefeed
    debug_command(machine, buf);
  }

  fclose(f);
}

void debug_virtual(machineinfo* machine, char* cmd)
{
  uint5 addr, got;
  tlbentry nulltlb;
  
  got = debug_hexargs(cmd, &addr);
  fprintf(stderr, "Virtual address: %.8x  Physical address: %.8x\n", addr,
    memory_virtualtophysical(machine->mem, addr, &nulltlb));
}

void debug_phetatrans(machineinfo* machine, char* cmd)
{
  pheta_chunk* mychunk;
  uint5 start, end;
  strsep(&cmd, " \t");
  if (!cmd)
  {
    fprintf(stderr, "Start address?\n");
    return;
  }
  start = debug_getnum(cmd);
  strsep(&cmd, " \t");
  if (!cmd)
  {
    fprintf(stderr, "End address?\n");
    return;
  }
  end = debug_getnum(cmd);
  start &= ~3;
  end &= ~3;
  fprintf(stderr, "Translating from %x to %x\n", start, end);
  mychunk = pheta_translatechunk(machine, start, (end-start)/4);
  phetadism_chunk(mychunk);
  fprintf(stderr, "Register allocating\n");
  palloc_init(mychunk);
  palloc_constant(mychunk);
//  palloc_nonorthog(mychunk);
//  palloc_fetchmem(mychunk);
  fprintf(stderr, "Commit shuffling\n");
  palloc_shufflecommit(mychunk);
  phetadism_chunk(mychunk);
  fprintf(stderr, "Finding live ranges\n");
  palloc_clearmarkers(mychunk);
  palloc_findspans(mychunk, mychunk->root, 0);
  palloc_clearmarkers(mychunk);
  palloc_linearscan(mychunk, mychunk->root, 0);
  palloc_printspans(mychunk);
  palloc_print(mychunk);
  // pheta_destroychunk(mychunk);  (when it's written...)
}

void debug_quit(machineinfo* machine, char* cmd)
{
  exit(0);
}

void debug_help(machineinfo* machine, char* cmd)
{
  int i;
  fprintf(stderr, "Commands available:\n");
  for (i=0; i<debug_COMMANDS; i++)
  {
    fprintf(stderr, "  %s\n", commands[i].command);
  }
}
