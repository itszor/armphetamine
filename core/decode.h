#ifndef DECODE_H
#define DECODE_H 1

#include "libjtype/defs.h"
#include "core/machine.h"

/*
#ifndef MACHINE_H
#error Please include machine.h before decode.h
#endif
*/

// Fields in instruction types to decode
typedef union {
  // Data processing
  struct {
    uint5 operand2 : 12;
    uint5 rd       : 4;
    uint5 rn       : 4;
    uint5 s        : 1;
    uint5 opcode   : 4;
    uint5 i        : 1;
    uint5 ident    : 2;
    uint5 cond     : 4;
  } dp;

  // Multiply
  struct {
    uint5 rm       : 4;
    uint5 ident    : 4;
    uint5 rs       : 4;
    uint5 rn       : 4;
    uint5 rd       : 4;
    uint5 s        : 1;
    uint5 a        : 1;
    uint5 ident2   : 6;
    uint5 cond     : 4;
  } mul;

  // Long multiply
  struct {
    uint5 rm       : 4;
    uint5 ident    : 4;
    uint5 rs       : 4;
    uint5 rdlo     : 4;
    uint5 rdhi     : 4;
    uint5 s        : 1;
    uint5 a        : 1;
    uint5 u        : 1;
    uint5 ident2   : 5;
    uint5 cond     : 4;
  } mull;

  // Single data swap
  struct {
    uint5 rm       : 4;
    uint5 ident    : 8;
    uint5 rd       : 4;
    uint5 rn       : 4;
    uint5 ident2   : 2;
    uint5 b        : 1;
    uint5 ident3   : 5;
    uint5 cond     : 4;
  } sds;

  // Single data transfer
  struct {
    uint5 offset   : 12;
    uint5 rd       : 4;
    uint5 rn       : 4;
    uint5 l        : 1;
    uint5 w        : 1;
    uint5 b        : 1;
    uint5 u        : 1;
    uint5 p        : 1;
    uint5 i        : 1;
    uint5 ident    : 2;
    uint5 cond     : 4;
  } sdt;

  struct {
    uint5 rm       : 4;
    uint5 ident    : 1;
    uint5 h        : 1;
    uint5 s        : 1;
    uint5 ident2   : 5;
    uint5 rd       : 4;
    uint5 rn       : 4;
    uint5 l        : 1;
    uint5 w        : 1;
    uint5 imm      : 1;
    uint5 u        : 1;
    uint5 p        : 1;
    uint5 ident4   : 3;
    uint5 cond     : 4;
  } sdth;

  // Block data transfer
  struct {
    uint5 reglist  : 16;
    uint5 rn       : 4;
    uint5 l        : 1;
    uint5 w        : 1;
    uint5 s        : 1;
    uint5 u        : 1;
    uint5 p        : 1;
    uint5 ident    : 3;
    uint5 cond     : 4;
  } bdt;

  // Branch
  struct {
    uint5 offset   : 24;
    uint5 l        : 1;
    uint5 ident    : 3;
    uint5 cond     : 4;
  } bra;

  // Co-processor data transfer
  struct {
    uint5 offset   : 8;
    uint5 cpn      : 4;
    uint5 crd      : 4;
    uint5 rn       : 4;
    uint5 l        : 1;
    uint5 w        : 1;
    uint5 n        : 1;
    uint5 u        : 1;
    uint5 p        : 1;
    uint5 ident    : 3;
    uint5 cond     : 4;
  } cdt;

  // Co-processor data operation
  struct {
    uint5 crm      : 4;
    uint5 ident    : 1;
    uint5 cp       : 3;
    uint5 cpn      : 4;
    uint5 crd      : 4;
    uint5 crn      : 4;
    uint5 cpopc    : 4;
    uint5 ident2   : 4;
    uint5 cond     : 4;
  } cdo;

  // Co-processor register transfer
  struct {
    uint5 crm      : 4;
    uint5 ident    : 1;
    uint5 cp       : 3;
    uint5 cpn      : 4;
    uint5 rd       : 4;
    uint5 crn      : 4;
    uint5 l        : 1;
    uint5 cpopc    : 3;
    uint5 ident2   : 4;
    uint5 cond     : 4;
  } crt;

  // Software interrupt
  struct {
    uint5 number   : 24;
    uint5 ident    : 4;
    uint5 cond     : 4;
  } swi;

  // PSR transfer - MRS
  struct {
    uint5 ident    : 12;
    uint5 rd       : 4;
    uint5 ident2   : 6;
    uint5 ps       : 1;
    uint5 ident3   : 5;
    uint5 cond     : 4;
  } mrs;

  // PSR transfer - MSR
  struct {
    uint5 rm       : 4;
    uint5 ident    : 18;
    uint5 pd       : 1;
    uint5 ident2   : 5;
    uint5 cond     : 4;
  } msr;

  // PSR transfer - MSR, flag bits only
  struct {
    uint5 operand  : 12;
    uint5 ident    : 10;
    uint5 pd       : 1;
    uint5 ident2   : 2;
    uint5 i        : 1;
    uint5 ident3   : 2;
    uint5 cond     : 4;
  } msrf;

  // Generic
  struct {
    uint5 padding  : 24;
    uint5 type     : 4;
    uint5 cond     : 4;
  } generic;

  // Instruction opcode
  uint5 instruction;
} instructionformat;

typedef int (*inst_fn)(machineinfo* m, instructionformat inst, void* data);

// instruction command-function table
struct insttab {
	inst_fn dp, dp_imm, mul, mull, sdt, sdth;
  inst_fn bdt, bra, swi, cdt, cdo, crt, sds, und;
  inst_fn thumbl;
};

typedef struct insttab insttab;

typedef enum {
  dp_AND=0,
  dp_EOR,
  dp_SUB,
  dp_RSB,
  dp_ADD,
  dp_ADC,
  dp_SBC,
  dp_RSC,
  dp_TST,
  dp_TEQ,
  dp_CMP,
  dp_CMN,
  dp_ORR,
  dp_MOV,
  dp_BIC,
  dp_MVN
} dp_opcodes;

/* None of this stuff is used any more

// Data-processing instructions
#define dp_ANDMASK  0x0c000000
#define dp_EORMASK  0x00000000

// Multiply instructions
#define mul_ANDMASK 0x0fc000f0
#define mul_EORMASK 0x00000090

// Single-data swap instructions
#define sds_ANDMASK 0x0fb00ff0
#define sds_EORMASK 0x01000090

// Single-data transfer instructions
#define sdt_ANDMASK 0x0c000000
#define sdt_EORMASK 0x04000000

// Undefined instructions
#define und_ANDMASK 0x0e000010
#define und_EORMASK 0x06000000

// Block-data transfer instructions
#define bdt_ANDMASK 0x0e000000
#define bdt_EORMASK 0x08000000

// Branch instructions
#define bra_ANDMASK 0x0e000000
#define bra_EORMASK 0x0a000000

// Co-processor data transfer instructions
#define cdt_ANDMASK 0x0e000000
#define cdt_EORMASK 0x0c000000

// Co-processor data operation instructions
#define cdo_ANDMASK 0x0f000010
#define cdo_EORMASK 0x0e000000

// Co-processor register transfer instructions
#define crt_ANDMASK 0x0f000010
#define crt_EORMASK 0x0e000010

// Software-interrupt instructions
#define swi_ANDMASK 0x0f000000
#define swi_EORMASK 0x0f000000
*/

// I wanted this to be nice and convoluted - it's turned out fairly sober. ish.
/*
#define is_itype(X,Y) (!(((Y) & X##_ANDMASK) ^ X##_EORMASK))

#define is_dp(Y)  is_itype(dp,Y)
#define is_mul(Y) is_itype(mul,Y)
#define is_sds(Y) is_itype(sds,Y)
#define is_sdt(Y) is_itype(sdt,Y)
#define is_und(Y) is_itype(und,Y)
#define is_bdt(Y) is_itype(bdt,Y)
#define is_bra(Y) is_itype(bra,Y)
#define is_cdt(Y) is_itype(cdt,Y)
#define is_cdo(Y) is_itype(cdo,Y)
#define is_crt(Y) is_itype(crt,Y)
#define is_swi(Y) is_itype(swi,Y)
*/
/* Condition codes:

   0000 = EQ - Z set
   0001 = NE - Z clear
   0010 = CS - C set (unsigned higher or same)
   0011 = CC - C clear (unsigned lower)
   0100 = MI - N set (negative)
   0101 = PL - N clear (positive or zero)
   0110 = VS - V set (overflow)
   0111 = VC - V clear (overflow)
   1000 = HI - C set and Z clear (unsigned higher)
   1001 = LS - C clear or Z set (unsigned lower or same)
   1010 = GE - N set and V set, or N clear and V clear (greater or equal)
   1011 = LT - N set and V clear, or N clear and V set (less than)
   1100 = GT - Z clear, and either N set and V set, or N clear and V clear (greater than)
   1101 = LE - Z set, or N set and V clear, or N clear and V set (less than or equal)
   1110 = AL - always
   1111 = NV - never
*/

typedef enum {
  cc_EQ=0,
  cc_NE,
  cc_CS,
  cc_CC,
  cc_MI,
  cc_PL,
  cc_VS,
  cc_VC,
  cc_HI,
  cc_LS,
  cc_GE,
  cc_LT,
  cc_GT,
  cc_LE,
  cc_AL,
  cc_NV
} cc_values;

//void decode(uint5 instruction, processorflags* flags, registers* reg, meminfo* mem);

extern int dispatch(machineinfo* machine, instructionformat inst,
                     insttab* action, void* data);

extern insttab diss, exec26, exec32, pseudo, phet4;

#endif
