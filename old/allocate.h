#ifndef ALLOCATE_H
#define ALLOCATE_H 1

#include "defs.h"
#include "x86asm.h"

// number of general-purpose registers available
#define NUMIREG 6

// use EDI as memory base pointer, EBP as register file base,
// ESP as a real stack pointer, EIP as a real program counter

// internal register numbers
typedef enum {
  ireg_EAX,
	ireg_EBX,
	ireg_ECX,
	ireg_EDX,
	ireg_ESI,
	ireg_EDI
} iregnumber;

extern uint3 iregset[];

#define var_NUMBER       0x00ff
#define var_MEM          0x0100
#define var_REG          0x0200
#define var_FLAG         0x0400
#define var_INVERTEDFLAG 0x0800
#define var_VALID        0x1000

// just in case there aren't already enough ways of referring to these
#define msk_EAX          0x01
#define msk_EBX          0x02
#define msk_ECX          0x04
#define msk_EDX          0x08
#define msk_ESI          0x10
#define msk_EDI          0x20

typedef union {
  struct {
    uint5 number       : 8;   // lots spare, might produce better code
	  uint5 mem          : 1;
    uint5 reg          : 1;
    uint5 flag         : 1;
    uint5 invertedflag : 1;
    uint5 valid        : 1;
		uint5 unused       : 19;
	} in;
	uint5 all;
} varlocinfo;

#define ctn_R0           0x00000001U
#define ctn_R1           0x00000002U
#define ctn_R2           0x00000004U
#define ctn_R3           0x00000008U
#define ctn_R4           0x00000010U
#define ctn_R5           0x00000020U
#define ctn_R6           0x00000040U
#define ctn_R7           0x00000080U
#define ctn_R8           0x00000100U
#define ctn_R9           0x00000200U
#define ctn_R10          0x00000400U
#define ctn_R11          0x00000800U
#define ctn_R12          0x00001000U
#define ctn_R13          0x00002000U
#define ctn_R14          0x00004000U
#define ctn_R15          0x00008000U
#define ctn_T0           0x00010000U
#define ctn_T1           0x00020000U
#define ctn_T2           0x00040000U
#define ctn_T3           0x00080000U
#define ctn_T4           0x00100000U
#define ctn_T5           0x00200000U
#define ctn_CPSRALL      0x00400000U
#define ctn_SPSRCURRENT  0x00800000U
#define ctn_CPSRFLAGS    0x01000000U
#define ctn_CPSRC        0x02000000U
#define ctn_CPSRV        0x04000000U
#define ctn_CPSRZ        0x08000000U
#define ctn_CPSRN        0x10000000U
#define ctn_CPSRMODE     0x20000000U
#define ctn_CPSRINTMASK  0x40000000U
#define ctn_UNUSED       0x80000000U

typedef union {
  struct {
	  uint5 r0          : 1;
		uint5 r1          : 1;
		uint5 r2          : 1;
		uint5 r3          : 1;
		uint5 r4          : 1;
		uint5 r5          : 1;
		uint5 r6          : 1;
		uint5 r7          : 1;
		uint5 r8          : 1;
		uint5 r9          : 1;
		uint5 r10         : 1;
		uint5 r11         : 1;
		uint5 r12         : 1;
		uint5 r13         : 1;
		uint5 r14         : 1;
		uint5 r15         : 1;
		uint5 t0          : 1;
		uint5 t1          : 1;
		uint5 t2          : 1;
		uint5 t3          : 1;
		uint5 t4          : 1;
		uint5 t5          : 1;
		uint5 cpsrall     : 1;
		uint5 spsrcurrent : 1;
		uint5 cpsrflags   : 1;
		uint5 cpsrc       : 1;
		uint5 cpsrv       : 1;
		uint5 cpsrz       : 1;
		uint5 cpsrn       : 1;
		uint5 cpsrmode    : 1;
		uint5 cpsrintmask : 1;
		uint5 unused      : 1;
	} contains;
	uint5 all;
} regcontentinfo;

#define ALWAYSLOCK         0xffc00000U

#define CACHEABLE 22

typedef regcontentinfo regctn[NUMIREG];
// specify whether ARM registers are in memory or Intel registers
typedef varlocinfo varloc[32];

#define alloc_UNSET   0
#define alloc_VALID   1
#define alloc_INVALID 2
#define alloc_MEMORY  3
#define alloc_HIDDEN  4

typedef union {
  struct {
    uint5 original : 8;
    uint5 mappedto : 8;  // x86-style register number
    uint5 mapnum   : 8;  // internal register number (0..IREGNUM-1)
    uint5 status   : 4;
    uint5 unused   : 4;
  } parts;
  uint5 all;
} registermapping;

typedef struct {
  regctn rctn;
	varloc vloc;
	uint5 spill;   // cycling value containing register to spill next
} allocationinfo;

typedef struct {
  uint5 imm;
	uint5 num;
  registermapping regset[6];
	uint3 overwrite;  // bit 1<<n set if regset[n] overwrites its value
  uint3 hidden;     // bit 1<<n set if regset[n] is hidden by assimilation
} matchstate;

#include "codegen.h"

extern void allocate_cpyregtomem(nativeblockinfo* nat, allocationinfo* alloc,
                                 uint5 ireg, uint5 var);
extern void allocate_cpymemtoreg(nativeblockinfo* nat, allocationinfo* alloc,
                                 uint5 var, uint5 ireg);
extern uint5 allocate_obtain(allocationinfo* alloc, uint5 lock, nativeblockinfo*
                             nat);
extern registermapping allocate_reg(allocationinfo* alloc, uint5 var,
                                    uint5 lock, uint5 type,
                                    nativeblockinfo* nat);
extern void allocate_setup(allocationinfo* alloc, matchstate* mstate,
                           uint5 inst, nativeblockinfo* nat, uint5* lock);
extern void allocate_npostfix(allocationinfo* alloc, matchstate* mstate);
extern uint5 allocate_nsolidify(allocationinfo* alloc, registermapping* rmap,
                                uint5 lock, nativeblockinfo* nat);
extern void allocate_reset(allocationinfo* alloc, nativeblockinfo* nat);
extern void allocate_swap(allocationinfo* alloc, registermapping* x,
                          registermapping* y, nativeblockinfo* nat);
extern void allocate_coerce(allocationinfo* from, allocationinfo* to,
                            nativeblockinfo* nat);
extern void allocate_force(allocationinfo* alloc, registermapping req,
                           nativeblockinfo* nat);
extern uint5 allocate_push(nativeblockinfo* nat, allocationinfo* alloc, uint5
                           ireg, uint5 var);
extern uint5 allocate_pop(nativeblockinfo* nat, allocationinfo* alloc,
                          uint5 var, uint5 ireg);
extern uint5 allocate_rpush(allocationinfo* alloc, uint5 which, nativeblockinfo*
                            nat);
extern void allocate_rpop(allocationinfo* alloc, uint5 stacked, nativeblockinfo*
                          nat);
/*extern void allocate_registerpattern(allocationinfo* alloc, matchstate* mstate,
                                     uint5* lock, nativeblockinfo* native);
extern void allocate_postfix(allocationinfo* alloc, matchstate* mstate);
extern void allocate_checkalloc(allocationinfo* alloc);
extern uint5 allocate_spill(allocationinfo* alloc, uint5 ireg, registermapping*
                            into, nativeblockinfo* nat);
extern void allocate_unspill(allocationinfo* alloc, registermapping* thisone,
                             nativeblockinfo* nat);
extern uint5 allocate_solidify(allocationinfo* alloc, registermapping* rmap,
                               uint5 lock, nativeblockinfo* nat);
extern void allocate_move(allocationinfo* alloc, registermapping from,
                          registermapping to, nativeblockinfo* nat);
extern sint5 allocate_forcereg(allocationinfo* alloc, matchstate* mstate,
                               uint5 ireg, uint5 forceto, uint5 lock,
	    							           nativeblockinfo* nat);
extern void allocate_restorereg(allocationinfo* alloc, uint5 ireg, sint5 temp,
                                nativeblockinfo* nat);*/
extern void allocate_preflags(allocationinfo* alloc, uint5 corrupts,
                              uint5 overwrites, nativeblockinfo* nat);
extern void allocate_postflags(allocationinfo* alloc, matchstate* mstate,
                               uint5 sets, uint5 required,
                               nativeblockinfo* nat);
extern void allocate_ensureflags(allocationinfo* alloc, matchstate* mstate,
                                 uint5 flags, uint5 lock, nativeblockinfo* nat);
/*extern void allocate_massage(allocationinfo* from, allocationinfo* to,
                             nativeblockinfo* nat);*/

#endif
