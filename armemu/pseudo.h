#ifndef PSEUDO_H
#define PSEUDO_H 1

#include "defs.h"
#include "list.h"
#include "hash.h"
#include "decode.h"
#include "codegen.h"

typedef enum {
  reg_R0,
	reg_R1,
	reg_R2,
	reg_R3,
	reg_R4,
	reg_R5,
	reg_R6,
	reg_R7,
	reg_R8,
	reg_R9,
	reg_R10,
	reg_R11,
	reg_R12,
	reg_R13,
	reg_R14,
	reg_R15,
	reg_T0,
	reg_T1,
	reg_T2,
	reg_T3,
	reg_T4,
  reg_T5,
	reg_CPSRALL,
	reg_SPSRCURRENT,
	reg_CPSRFLAGS,       // below here, registers are virtual
	reg_CPSRC,
	reg_CPSRV,
	reg_CPSRZ,
	reg_CPSRN,
	reg_CPSRMODE,
	reg_CPSRINTMASK,
//	reg_PSLINK,         // for BL, store return address in pseudocode here (?)
	reg_UNUSED
} regnumber;

typedef enum {
/*  op_SUBC,
	op_ADDC,
	op_SBCC,
	op_ADCC,
	op_SUBV,
	op_ADDV,
	op_SBCV,
	op_ADCV,
	op_Z,
	op_N,
	op_LSLC,
	op_LSRC,
	op_ASRC,
	op_RORC,
	op_RRXC,*/
	op_BEQ,
	op_BNE,
	op_BCS,
	op_BCC,
	op_BMI,
	op_BPL,
	op_BVS,
	op_BVC,
	op_BHI,
	op_BLS,
	op_BGE,
	op_BLT,
	op_BGT,
	op_BLE,
	op_BAL,
	op_LSL,
	op_LSR,
	op_ASR,
	op_ROR,
	op_RRX,
	op_AND,
	op_EOR,
	op_SUB,
	op_SBC,
	op_ADD,
	op_ADC,
	op_OR,
	op_MOV,
	op_MVN,
	op_CMP,
  op_CMN,
  op_TEQ,
  op_TST,
	op_MUL,
	op_CONST,
	op_LDW,
	op_LDB,
	op_STW,
	op_STB,
	op_SWI,  // instructions here and below interrupt flow of execution
	op_XJMP,
	op_SETPC,
  op_RECONSTRUCTPC,
	op_END
} pseudoopcode;

// if ORred with a psopcode, reads an extra 'flags affected' arg in pseudo_emit
#define PSSETFLAGS (1U<<7)
#define NOFLAGS (1U<<8)
#define CFLAG  0x01
#define VFLAG  0x02
#define ZFLAG  0x04
#define NFLAG  0x08
#define ICFLAG 0x10
#define ALLFLAGS 15

// format of an intermediate-code instruction
typedef union {
  struct {
	  uint5 opcode	: 7;
		uint5 rd			: 5;
		uint5 rm			: 5;
		uint5 rn			: 5;
		uint5 ro      : 5;
		uint5 next    : 1;  // if one, next word is part of this instruction
		uint5 flags   : 4;  // any flags affected
	} parts;
	uint5 value;
} pseudoformat;

// information held for each ARM instruction being translated
typedef struct {
	uint5 pseudo;
	list* patchback;
} psmapinfo;

// one of these things is created for each instruction in a block
typedef struct {
	union {
	  struct {
		  uint5 leader     : 1;   // instruction starts a basic block
			uint5 prev       : 1;   // previous instruction is an immediate field
			uint5 allocated  : 1;   // (if a leader) - has an allocation state
//			uint5 follows    : 1;   // immediately follows another block
      uint5 complete   : 1;
			uint5 unused     : 28;
		} flag;
		uint5 value;
	} special;
  gen_fn rule;
	matchstate mstate;          // a bit extravagant maybe...
} imapinfo;

// information about a block of intermediate code
typedef struct {
  uint5* base;
	uint5 length;
	uint5 size;
  imapinfo* info;  // extra info for each pseudocode instruction
	hashtable* pseudomap;
	hashtable* basic;
} psblock;

extern psblock*		pseudo_newblock(uint5 startsize);
extern void 			pseudo_deleteblock(psblock* psb);
extern psmapinfo* pseudo_newpsminfo(uint5 pseudo);
extern void				pseudo_deletepsminfo(psmapinfo* psm);
extern void       pseudo_psminfodestructor(void* data);
extern void				pseudo_emit(psblock* blk, pseudoopcode op, ...);
extern uint5			pseudo_getpsaddr(hashtable*, uint5*, sint5);
extern void				pseudo_addmapping(hashtable*, uint5*, uint5*, sint5);
extern sint5			pseudo_newtemp(sint5* prevtemp);
extern void				pseudo_condition(machineinfo*, uint5 cond, psblock*);
extern void       pseudo_reconstitutepc(machineinfo* machine, uint5 sflag,
                                        psblock* psb, sint5* tc, sint5 offset);
extern void       pseudo_jmp(machineinfo*, uint5 cond, uint5* dest, psblock*);
extern void				pseudo_dp(machineinfo*, instructionformat, void*);
extern void				pseudo_dp_imm(machineinfo*, instructionformat, void*);
extern void				pseudo_dp_guts(machineinfo*, instructionformat, void*,
                                 uint5, sint5*);
extern void				pseudo_bra(machineinfo*, instructionformat, void*);
extern void				pseudo_mul(machineinfo*, instructionformat, void*);
extern void				pseudo_sdt(machineinfo*, instructionformat, void*);
extern void				pseudo_bdt(machineinfo*, instructionformat, void*);
extern void				pseudo_swi(machineinfo*, instructionformat, void*);
extern void				pseudo_cdt(machineinfo*, instructionformat, void*);
extern void				pseudo_cdo(machineinfo*, instructionformat, void*);
extern void				pseudo_crt(machineinfo*, instructionformat, void*);
extern void				pseudo_sds(machineinfo*, instructionformat, void*);
extern void				pseudo_und(machineinfo*, instructionformat, void*);

#endif
