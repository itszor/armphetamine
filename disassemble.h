#ifndef DISASSEMBLE_H
#define DISASSEMBLE_H 1

#include "decode.h"
#include "machine.h"

extern void diss_dp(machineinfo* machine, instructionformat inst, void* null);
extern void diss_dp_imm(machineinfo* machine, instructionformat inst, void* null);
extern void diss_mul(machineinfo* machine, instructionformat inst, void* null);
extern void diss_sdt(machineinfo* machine, instructionformat inst, void* null);
extern void diss_bdt(machineinfo* machine, instructionformat inst, void* null);
extern void diss_bra(machineinfo* machine, instructionformat inst, void* null);
extern void diss_swi(machineinfo* machine, instructionformat inst, void* null);
extern void diss_cdt(machineinfo* machine, instructionformat inst, void* null);
extern void diss_cdo(machineinfo* machine, instructionformat inst, void* null);
extern void diss_crt(machineinfo* machine, instructionformat inst, void* null);
extern void diss_sds(machineinfo* machine, instructionformat inst, void* null);
extern void diss_und(machineinfo* machine, instructionformat inst, void* null);

#endif
