#ifndef DECODE4_H
#define DECODE4_H 1

#include "defs.h"
#include "decode.h"

enum dec4_insttypes
{
  dec4_dp,
  dec4_dp_imm,
  dec4_mul,
  dec4_sdt,
  dec4_bdt,
  dec4_bra,
  dec4_swi,
  dec4_cdt,
  dec4_cdo,
  dec4_crt,
  dec4_sds,
  dec4_und
};

extern uint5 dec4_insttype(instructionformat inst);

#endif
