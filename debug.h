#ifndef DEBUG_H
#define DEBUG_H 1

#include <stdarg.h>

#include "machine.h"

typedef enum {
  debug_COMMENT,
  debug_RUN,
  debug_DISASSEMBLE,
  debug_MEMORY,
  debug_REGISTERS,
  debug_SETREG,
  debug_BREAKSET,
  debug_BREAKCLEAR,
  debug_BREAKLIST,
  debug_TRACE,
  debug_LOAD,
  debug_ROMLOAD,
  debug_SCRIPT,
  debug_VIRTUAL,
  debug_PHETATRANS,
  debug_QUIT,
  debug_HELP,
  debug_COMMANDS
} debug_enum;

typedef void (*debugfn)(machineinfo* machine, char* cmd);

typedef struct {
  const char* command;
  debug_enum ref;
  debugfn exec;
} debug_cmd;

extern void debug_command(machineinfo* machine, char* cmd);
extern void debug_shell(machineinfo* machine);
extern int debug_hexargs(char* cmd, ...);

// debugger commands
extern void debug_null(machineinfo* machine, char* cmd);
extern void debug_run(machineinfo* machine, char* cmd);
extern void debug_disassemble(machineinfo* machine, char* cmd);
extern void debug_memory(machineinfo* machine, char* cmd);
extern void debug_registers(machineinfo* machine, char* cmd);
extern void debug_setreg(machineinfo* machine, char* cmd);
extern void debug_breakset(machineinfo* machine, char* cmd);
extern void debug_breakclear(machineinfo* machine, char* cmd);
extern void debug_breaklist(machineinfo* machine, char* cmd);
extern void debug_trace(machineinfo* machine, char* cmd);
extern void debug_load(machineinfo* machine, char* cmd);
extern void debug_romload(machineinfo* machine, char* cmd);
extern void debug_script(machineinfo* machine, char* cmd);
extern void debug_virtual(machineinfo* machine, char* cmd);
extern void debug_phetatrans(machineinfo* machine, char* cmd);
extern void debug_quit(machineinfo* machine, char* cmd);
extern void debug_help(machineinfo* machine, char* cmd);

#endif
