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

#ifdef ARM26BIT
#  define PCFLAGPUT(X,V) reg->r[X] = ((X)!=15) ? (V) \
                         : ((reg->r[15] & 0x0fffffff) | ((V) & 0xf0000000))
#else
#  define PCFLAGPUT(X,V) ({ \
  if ((X)==15) \
  { \
    reg->cpsr.value = (reg->cpsr.value & 0x0fffffff) | ((V) & 0xf0000000); \
    fprintf(stderr, "Badly tested code\n"); abort(); \
  } \
  else \
    reg->r[X] = (V); })
#endif

// raw versions
#define RGET(X) (reg->r[X])
#define RPUT(X,V) reg->r[X] = (V)

#ifdef ARMTHUMB
#  define INSTSIZE (2)
#else
#  define INSTSIZE (4)
#endif

#define INCPC reg->r[15]+=INSTSIZE

#ifdef ARM26BIT
#  define PCADDR (reg->r[15] & ~0xfc000003)
#  define PCSETADDR(X) reg->r[15] = (reg->r[15] & 0xfc000003) | ((X) & \
                                     ~0xfc000003)
#  define PCSETADFL(X) do { \
            reg->r[15] = (reg->r[15] & reg->pcmask) | ((X) & ~reg->pcmask); \
          /*  processor_mode(machine, reg->r[15] & 0x3); */\
          } while (0);

#else
#  define PCADDR (reg->r[15])
#  define PCSETADDR(X) reg->r[15] = (X)
// also do (somethingorother) with CPSR...
#  define PCSETADFL(X) do { \
            /*psrinfo spsr = reg->spsr[reg->spsr_current]; */\
            reg->r[15] = (X); \
            /*processor_mode(machine, reg->spsr[reg->spsr_current].flag.mode); \
            reg->cpsr = spsr; \
            if (reg->cpsr.flag.interrupt & 0x2) \
              fprintf(stderr, "-- IRQ Disabled (pcsetadfl)\n"); \
            else \
            { \
              fprintf(stderr, "-- IRQ Enabled\n"); \
             |* fprintf(stderr, "Unchecked (pcsetadfl)\n"); \
              abort();*| \
            } */\
          } while (0);
#endif

#ifdef ARM26BIT
#  define STOREREG(C,V) if ((C)==15) \
                          if (inst.dp.s) PCSETADFL(V) else PCSETADDR(V); \
                        else RPUT((C), V)
//#  error "Guess who broke 26-bit mode?"
#else
#  define STOREREG(C,V) if ((C)==15) { \
                          RPUT((C), V); \
                          if (inst.dp.s && reg->spsr_current!=0) { \
                            processor_mode(machine, \
                              reg->spsr[reg->spsr_current].flag.mode); \
                            if (reg->cpsr.flag.interrupt & 0x2) \
                            { \
                              fprintf(stderr, "-- IRQ Disabled (storereg)\n"); \
                            } \
                            else \
                            { \
                              fprintf(stderr, "-- IRQ Enabled (storereg)\n"); \
                            } \
                          } \
                        } \
                        else RPUT((C), V)
#endif

#define PRINTOPS 1

#ifdef EMULART
#undef STOREPCPLUS12
#define STOREPCPLUS8
#else
#define STOREPCPLUS12
#undef STOREPCPLUS8
#endif

// #define FAKESWI 1

extern int exec_dp_26(machineinfo* machine, instructionformat inst,
                       void* null);
extern int exec_dp_imm_26(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_psrt_26(machineinfo* machine, instructionformat inst,
                         uint5 op2);
extern int exec_mul_26(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_mull_26(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_sdt_26(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_sdth_26(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_bdt_26(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_bra_26(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_swi_26(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_cdt_26(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_cdo_26(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_crt_26(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_sds_26(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_und_26(machineinfo* machine, instructionformat inst,
                        void* null);

extern int exec_dp_32(machineinfo* machine, instructionformat inst,
                       void* null);
extern int exec_dp_imm_32(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_psrt_32(machineinfo* machine, instructionformat inst,
                         uint5 op2);
extern int exec_mul_32(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_mull_32(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_sdt_32(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_sdth_32(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_bdt_32(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_bra_32(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_swi_32(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_cdt_32(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_cdo_32(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_crt_32(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_sds_32(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_und_32(machineinfo* machine, instructionformat inst,
                        void* null);

extern int exec_dp_thm(machineinfo* machine, instructionformat inst,
                       void* null);
extern int exec_dp_imm_thm(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_psrt_thm(machineinfo* machine, instructionformat inst,
                         uint5 op2);
extern int exec_mul_thm(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_mull_thm(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_sdt_thm(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_sdth_thm(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_bdt_thm(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_bra_thm(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_swi_thm(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_cdt_thm(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_cdo_thm(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_crt_thm(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_sds_thm(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_und_thm(machineinfo* machine, instructionformat inst,
                        void* null);
extern int exec_thumbl_thm(machineinfo* machine, instructionformat inst,
                            void* null);

#endif
