#ifndef EXECUTE_H
#define EXECUTE_H 1

#include "processor.h"
#include "decode.h"
#include "machine.h"

#define TOPBIT 0x80000000U

#ifdef ARM26BIT
#  define FLAG(N) (((pcinfo*)&reg->r[15])->flag.N)
#else
#  define FLAG(N) (reg->cpsr.flag.N)
#endif

// this logic is stolen from ArcEm
#define ADDFLAGS(A,B) \
  if (inst.dp.rd!=15) { \
    FLAG(v) = ((((A)&(B)&~rd)&TOPBIT) || ((~(A)&~(B)&rd)&TOPBIT)) ? 1 : 0; \
    FLAG(c) = ((((A)&(B))&TOPBIT) \
            || (((A)&~rd)&TOPBIT) \
	          || (((B)&~rd)&TOPBIT)) ? 1 : 0; }

#define SUBFLAGS(A,B) \
  if (inst.dp.rd!=15) { \
    FLAG(v) = ((((A)&~(B)&~rd)&TOPBIT) || ((~(A)&(B)&rd)&TOPBIT)) ? 1 : 0; \
    FLAG(c) = ((((A)&~(B))&TOPBIT) \
            || (((A)&~rd)&TOPBIT) \
            || ((~(B)&~rd)&TOPBIT)) ? 1 : 0; }

#define ZNFLAGS \
  if (inst.dp.rd!=15) { \
    FLAG(z) = rd==0; \
    FLAG(n) = rd>>31; }

// macros to read & write registers
#ifdef ARM26BIT
#  define GET(X) ((X)==15 ? reg->r[X] & ~0xfc000003 : reg->r[X])
#else
#  define GET(X) (reg->r[X])
#endif
// if program counter is written to, normally only the address bits are affected
// (in 26 bit mode)
#ifdef ARM26BIT
#  define PUT(X,V) reg->r[X] = (X)!=15 ? (V) \
									: (reg->r[15] & 0xfc000003) | ((V) & ~0xfc000003)
#else
#  define PUT(X,V) reg->r[X] = (V)
#endif

// raw versions
#define RGET(X) (reg->r[X])
#define RPUT(X,V) reg->r[X] = (V)

#define INCPC reg->r[15]+=4

#ifdef ARM26BIT
#  define PCADDR (reg->r[15] & ~0xfc000003)
#  define PCSETADDR(X) reg->r[15] = (reg->r[15] & 0xfc000003) | ((X) & \
                                     ~0xfc000003)
#  define PCSETADFL(X) reg->r[15] = (reg->r[15] & 0x0c000003) | ((X) & \
                                     ~0x0c000003)
#else
#  define PCADDR (reg->r[15])
#  define PCSETADDR(X) reg->r[15] = (X)
// also do (somethingorother) with CPSR...
#  define PCSETADFL(X) reg->r[15] = (X)
//#  define PCSETADFL(X) reg->r[15] = (reg->r[15] & 0x0c000003) | ((X) & ~0x0c000003)
#endif

#ifdef ARM26BIT
#  define STOREREG(C,V) if (inst.##C##.rd==15) \
                          if (inst.##C##.s) PCSETADFL(V); else PCSETADDR(V); \
                        else PUT(inst.##C##.rd, V)
#else
#  define STOREREG(C,V) PUT(inst.##C##.rd, V)
#endif

#define PRINTOPS 1
#define FAKESWI 1

extern void exec_dp(machineinfo* machine, instructionformat inst, void* null);
extern void exec_dp_imm(machineinfo* machine, instructionformat inst,
                        void* null);
extern void exec_mul(machineinfo* machine, instructionformat inst, void* null);
extern void exec_sdt(machineinfo* machine, instructionformat inst, void* null);
extern void exec_bdt(machineinfo* machine, instructionformat inst, void* null);
extern void exec_bra(machineinfo* machine, instructionformat inst, void* null);
extern void exec_swi(machineinfo* machine, instructionformat inst, void* null);
extern void exec_cdt(machineinfo* machine, instructionformat inst, void* null);
extern void exec_cdo(machineinfo* machine, instructionformat inst, void* null);
extern void exec_crt(machineinfo* machine, instructionformat inst, void* null);
extern void exec_sds(machineinfo* machine, instructionformat inst, void* null);
extern void exec_und(machineinfo* machine, instructionformat inst, void* null);

#endif
