#ifndef PHETADISM_H
#define PHETADISM_H 1

#include <stdio.h>

#include "libjtype/defs.h"
#include "dynarec/pheta.h"
#include "core/machine.h"

extern const char* opname[];

extern void phetadism_chunk(pheta_chunk* chunk);
extern void phetadism_instruction(FILE* f, pheta_instr* instr);
extern void phetadism_block(pheta_basicblock* basic, uint5 startline);

#endif
