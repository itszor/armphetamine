#ifndef DAG_H
#define DAG_H 1

#include "defs.h"
#include "machine.h"
#include "pseudo.h"
#include "list.h"
#include "codegen.h"

#error Please don't include dag.h!

/*
#define bit_R0           0x00000001U
#define bit_R1           0x00000002U
#define bit_R2           0x00000004U
#define bit_R3           0x00000008U
#define bit_R4           0x00000010U
#define bit_R5           0x00000020U
#define bit_R6           0x00000040U
#define bit_R7           0x00000080U
#define bit_R8           0x00000100U
#define bit_R9           0x00000200U
#define bit_R10          0x00000400U
#define bit_R11          0x00000800U
#define bit_R12          0x00001000U
#define bit_R13          0x00002000U
#define bit_R14          0x00004000U
#define bit_R15          0x00008000U
#define bit_CPSRALL      0x00010000U
#define bit_SPSRCURRENT  0x00020000U
#define bit_CPSRFLAGS    0x00040000U
#define bit_CPSRC        0x00080000U
#define bit_CPSRV        0x00100000U
#define bit_CPSRZ        0x00200000U
#define bit_CPSRN        0x00400000U
#define bit_CPSRMODE     0x00800000U
#define bit_CPSRINTMASK  0x01000000U
#define bit_T0           0x02000000U
#define bit_T1           0x04000000U
#define bit_T2           0x08000000U
#define bit_T3           0x10000000U
#define bit_T4           0x20000000U
#define bit_PSLINK       0x40000000U
#define bit_UNUSED       0x80000000U

// flag set if this instruction has been branched to
#define flag_BLOCKSTART  0x01
// flag set if instruction becomes assimilated by a subsequent instruction
#define flag_KILLED      0x02
// if it turns out the result from the previous instruction is used again,
// the assimilation is rolled back (ugly and horrible...)
#define flag_ZOMBIE      0x04
// if any flags need storing, say so
#define flag_SAVEC       0x08
#define flag_SAVEV       0x10
#define flag_SAVEZ       0x20
#define flag_SAVEN       0x40
#define flag_SAVEALL     (flag_SAVEC | flag_SAVEV | flag_SAVEZ | flag_SAVEN)
// flag set if there is a hole before the current instruction
#define flag_PREV        0x80
*/

extern dagnode* dag_build(psblock* psb);
extern void dag_backpass(psblock* psb, dagnode* dag);

#endif
