#ifndef DISASSEMBLE_H
#define DISASSEMBLE_H 1

#include "core/decode.h"
#include "core/machine.h"

extern int diss_dp(machineinfo* machine, instructionformat inst, void* null);

extern int diss_dp_imm(machineinfo* machine, instructionformat inst,
  void* null);

extern int diss_mul(machineinfo* machine, instructionformat inst, void* null);

extern int diss_mull(machineinfo* machine, instructionformat inst, void* null);

extern int diss_sdt(machineinfo* machine, instructionformat inst, void* null);

extern int diss_sdth(machineinfo* machine, instructionformat inst, void* null);

extern int diss_bdt(machineinfo* machine, instructionformat inst, void* null);

extern int diss_bra(machineinfo* machine, instructionformat inst, void* null);

extern int diss_swi(machineinfo* machine, instructionformat inst, void* null);

extern int diss_cdt(machineinfo* machine, instructionformat inst, void* null);

extern int diss_cdo(machineinfo* machine, instructionformat inst, void* null);

extern int diss_crt(machineinfo* machine, instructionformat inst, void* null);

extern int diss_sds(machineinfo* machine, instructionformat inst, void* null);

extern int diss_und(machineinfo* machine, instructionformat inst, void* null);

#endif
