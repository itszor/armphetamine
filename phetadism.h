#ifndef PHETADISM_H
#define PHETADISM_H 1

#include "defs.h"
#include "pheta.h"
#include "machine.h"

extern void phetadism_chunk(pheta_chunk* chunk);
extern void phetadism_block(pheta_basicblock* basic, uint5 startline);

#endif
