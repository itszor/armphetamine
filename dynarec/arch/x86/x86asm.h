#ifndef X86ASM_H
#define X86ASM_H 1

#include "defs.h"
#include "hash.h"
#include "registers.h"
#include "clist.h"

// Instruction encodings ((number of bits, value) pairs)
#define ADClrr   8,0x11,3,rm,3,reg,2,0x3
#define ADClri   8,0x81,3,rm,3,0x2,2,0x3,32,imm
#define ADClmi   8,0x81,3,rm,3,0x2,MODSIBDISP,32,imm
#define ADClrm   8,0x13,3,rm,3,reg,MODSIBDISP
#define ADClmr   8,0x11,3,rm,3,reg,MODSIBDISP
#define ADCdef   CFLAG | VFLAG | NFLAG | ZFLAG
#define ADCundef 0

#define ADDlrr   8,0x01,3,rm,3,reg,2,0x3
#define ADDlri   8,0x81,3,rm,3,0x0,2,0x3,32,imm
#define ADDlmi   8,0x81,3,rm,3,0x0,MODSIBDISP,32,imm
#define ADDlrm   8,0x03,3,rm,3,reg,MODSIBDISP
#define ADDlmr   8,0x01,3,rm,3,reg,MODSIBDISP
#define ADDdef   CFLAG | VFLAG | NFLAG | ZFLAG
#define ADDundef 0

#define ANDlrr   8,0x21,3,rm,3,reg,2,0x3
#define ANDlri   8,0x81,3,rm,3,0x4,2,0x3,32,imm
#define ANDlmi   8,0x81,3,rm,3,0x4,MODSIBDISP,32,imm
#define ANDlrm   8,0x23,3,rm,3,reg,MODSIBDISP
#define ANDlmr   8,0x21,3,rm,3,reg,MODSIBDISP
#define ANDdef   NFLAG | ZFLAG
#define ANDundef CFLAG | VFLAG

// this undefines C because it won't be used in the context of a CC-setting
// ARM instruction
#define BTlrr    8,0x0f,8,0xa3,3,rm,3,reg,2,0x3
#define BTlrm    8,0x0f,8,0xa3,3,rm,3,reg,MODSIBDISP
#define BTlri    8,0x0f,8,0xba,3,rm,3,0x4,2,0x3,8,imm
#define BTlmi    8,0x0f,8,0xba,3,rm,3,0x4,MODSIBDISP,8,imm
#define BTdef    CFLAG
#define BTundef  VFLAG | NFLAG | ZFLAG

// call doesn't actually affect anything, but...
#define CALLnar  8,0xff,3,rm,3,0x2,2,0x3
#define CALLnam  8,0xff,3,rm,3,0x2,MODSIBDISP
#define CALLdef  0
#define CALLundef CFLAG | VFLAG | NFLAG | ZFLAG

#define CLC      8,0xf8
#define CLCdef   CFLAG
#define CLCundef 0

#define CMC      8,0xf5
#define CMCdef   CFLAG
#define CMCundef 0

#define CMPlrr   8,0x39,3,rm,3,reg,2,0x3
#define CMPlri   8,0x81,3,rm,3,0x7,2,0x3,32,imm
#define CMPlmi   8,0x81,3,rm,3,0x7,MODSIBDISP,32,imm
#define CMPlrm   8,0x3b,3,rm,3,reg,MODSIBDISP
#define CMPlmr   8,0x39,3,rm,3,reg,MODSIBDISP
#define CMPdef   CFLAG | ICFLAG | VFLAG | NFLAG | ZFLAG
#define CMPundef 0

#define DEClr    8,0xff,3,rm,5,0x09
#define DEClm    8,0xff,3,rm,3,0x1,MODSIBDISP
#define DECdef   VFLAG | NFLAG | ZFLAG
#define DECundef 0

// IMUL seems to invert the usual meanings of 'rm' and 'reg'...
//#define IMULlrr  8,0x0f,8,0xaf,3,rm,3,reg,2,0x3
// so here's a version which should work properly :-P
#define IMULlrr  8,0x0f,8,0xaf,3,reg,3,rm,2,0x3
// these others will probably be wrong as well, but aren't currently used
#define IMULlri  8,0x69,3,rm,3,reg,2,0x3,32,imm
#define IMULlrm  8,0x0f,8,0xaf,3,rm,3,reg,MODSIBDISP
#define IMULdef  0
#define IMULundef CFLAG | VFLAG | NFLAG | ZFLAG

#define INClr    5,0x08,3,rm
#define INClm    8,0xff,3,rm,3,0x0,MODSIBDISP
#define INCdef   VFLAG | NFLAG | ZFLAG
#define INCundef 0

#define JAnr     8,0x0f,8,0x87,32,disp
#define JAsr     8,0x77,8,disp
#define JAEnr    8,0x0f,8,0x83,32,disp
#define JAEsr    8,0x73,8,disp
#define JBnr     8,0x0f,8,0x82,32,disp
#define JBsr     8,0x72,8,disp
#define JBEnr    8,0x0f,8,0x86,32,disp
#define JBEsr    8,0x76,8,disp
#define JCnr     8,0x0f,8,0x82,32,disp
#define JCsr     8,0x72,8,disp
#define JEnr     8,0x0f,8,0x84,32,disp
#define JEsr     8,0x74,8,disp
#define JGnr     8,0x0f,8,0x8f,32,disp
#define JGsr     8,0x7f,8,disp
#define JGEnr    8,0x0f,8,0x8d,32,disp
#define JGEsr    8,0x7d,8,disp
#define JLnr     8,0x0f,8,0x8c,32,disp
#define JLsr     8,0x7c,8,disp
#define JLEnr    8,0x0f,8,0x8e,32,disp
#define JLEsr    8,0x7e,8,disp
#define JNAnr    8,0x0f,8,0x86,32,disp
#define JNAsr    8,0x76,8,disp
#define JNAEnr   8,0x0f,8,0x82,32,disp
#define JNAEsr   8,0x72,8,disp
#define JNBnr    8,0x0f,8,0x83,32,disp
#define JNBsr    8,0x73,8,disp
#define JNBEnr   8,0x0f,8,0x87,32,disp
#define JNBEsr   8,0x77,8,disp
#define JNCnr    8,0x0f,8,0x83,32,disp
#define JNCsr    8,0x73,8,disp
#define JNEnr    8,0x0f,8,0x85,32,disp
#define JNEsr    8,0x75,8,disp
#define JNGnr    8,0x0f,8,0x8e,32,disp
#define JNGsr    8,0x7e,8,disp
#define JNGEnr   8,0x0f,8,0x8c,32,disp
#define JNGEsr   8,0x7c,8,disp
#define JNLnr    8,0x0f,8,0x8d,32,disp
#define JNLsr    8,0x7d,8,disp
#define JNLEnr   8,0x0f,8,0x8f,32,disp
#define JNLEsr   8,0x7f,8,disp
#define JNOnr    8,0x0f,8,0x81,32,disp
#define JNOsr    8,0x71,8,disp
#define JNPnr    8,0x0f,8,0x8b,32,disp
#define JNPsr    8,0x7b,8,disp
#define JNSnr    8,0x0f,8,0x89,32,disp
#define JNSsr    8,0x79,8,disp
#define JNZnr    8,0x0f,8,0x85,32,disp
#define JNZsr    8,0x75,8,disp
#define JOnr     8,0x0f,8,0x80,32,disp
#define JOsr     8,0x70,8,disp
#define JPnr     8,0x0f,8,0x8a,32,disp
#define JPsr     8,0x7a,8,disp
#define JPEnr    8,0x0f,8,0x8a,32,disp
#define JPEsr    8,0x7a,8,disp
#define JPOnr    8,0x0f,8,0x8b,32,disp
#define JPOsr    8,0x7b,8,disp
#define JSnr     8,0x0f,8,0x88,32,disp
#define JSsr     8,0x78,8,disp
#define JZnr     8,0x0f,8,0x84,32,disp
#define JZsr     8,0x74,8,disp
#define Jdef     0
#define Jundef   CFLAG | VFLAG | NFLAG | ZFLAG

#define JMPnr    8,0xe9,32,disp
#define JMPnar   8,0xff,3,rm,3,0x4,2,0x3
#define JMPnam   8,0xff,3,rm,3,0x4,MODSIBDISP
#define JMPdef   0
#define JMPundef CFLAG | VFLAG | NFLAG | ZFLAG

#define MOVlrr   8,0x89,3,rm,3,reg,2,0x3
#define MOVlri   8,0xc7,3,rm,3,0x0,2,0x3,32,imm
#define MOVlmi   8,0xc7,3,rm,3,0x0,MODSIBDISP,32,imm
#define MOVlrm   8,0x8b,3,rm,3,reg,MODSIBDISP
#define MOVlmr   8,0x89,3,rm,3,reg,MODSIBDISP
#define MOVbmr   8,0x88,3,rm,3,reg,MODSIBDISP
#define MOVZXlrbm 8,0x0f,8,0xb6,3,rm,3,reg,MODSIBDISP
#define MOVdef   0
#define MOVundef 0

#define NEGlr    8,0xf7,3,rm,3,0x3,2,0x3
#define NEGlm    8,0xf7,3,rm,3,0x3,MODSIBDISP
#define NEGdef   CFLAG | VFLAG | NFLAG | ZFLAG
#define NEGundef 0

#define NOP      8,0x90

#define NOTlr    8,0xf7,3,rm,3,0x2,2,0x3
#define NOTlm    8,0xf7,3,rm,3,0x2,MODSIBDISP
#define NOTdef   0
#define NOTundef 0

#define ORlrr    8,0x09,3,rm,3,reg,2,0x3
#define ORlri    8,0x81,3,rm,3,0x1,2,0x3,32,imm
#define ORlmi    8,0x81,3,rm,3,0x1,MODSIBDISP,32,imm
#define ORlrm    8,0x0b,3,rm,3,reg,MODSIBDISP
#define ORlmr    8,0x09,3,rm,3,reg,MODSIBDISP
#define ORdef    NFLAG | ZFLAG
#define ORundef  CFLAG | VFLAG

#define POPlr    8,0x8f,3,rm,3,0x0,2,0x3
#define POPlm    8,0x8f,3,rm,3,0x0,MODSIBDISP
#define POPdef   0
#define POPundef 0
#define POPF     8,0x9d
#define POPFdef  CFLAG | VFLAG | NFLAG | ZFLAG
#define POPFundef 0

#define PUSHlr   8,0xff,3,rm,3,0x6,2,0x3
#define PUSHli   8,0x68,32,imm
#define PUSHlm   8,0xff,3,rm,3,0x6,MODSIBDISP
#define PUSHdef  0
#define PUSHundef 0

#define PUSHF    8,0x9c
#define PUSHFdef 0
#define PUSHFundef 0

#define RCLlrc   8,0xd3,3,rm,3,0x2,2,0x3
#define RCLlri   8,0xc1,3,rm,3,0x2,2,0x3,8,imm
#define RCLdef   CFLAG
#define RCLundef VFLAG

#define RCRlrc   8,0xd3,3,rm,3,0x3,2,0x3
#define RCRlri   8,0xc1,3,rm,3,0x3,2,0x3,8,imm
#define RCRdef   CFLAG
#define RCRundef VFLAG

#define RET      8,0xc3
#define RETdef   0
#define RETundef 0

#define ROLlrc   8,0xd3,3,rm,3,0x0,2,0x3
#define ROLlri   8,0xc1,3,rm,3,0x0,2,0x3,8,imm
#define ROLdef   CFLAG
#define ROLundef VFLAG

#define RORlrc   8,0xd3,3,rm,3,0x1,2,0x3
#define RORlri   8,0xc1,3,rm,3,0x1,2,0x3,8,imm
#define RORdef   CFLAG
#define RORundef VFLAG

#define SHLlrc   8,0xd3,3,rm,3,0x4,2,0x3
#define SHLlri   8,0xc1,3,rm,3,0x4,2,0x3,8,imm
#define SHLdef   CFLAG
#define SHLundef VFLAG | NFLAG | ZFLAG

#define SARlrc   8,0xd3,3,rm,3,0x7,2,0x3
#define SARlri   8,0xc1,3,rm,3,0x7,2,0x3,8,imm
#define SARdef   CFLAG
#define SARundef VFLAG | NFLAG | ZFLAG

#define SBBlrr   8,0x19,3,rm,3,reg,2,0x3
#define SBBlri   8,0x81,3,rm,3,0x3,2,0x3,32,imm
#define SBBlmi   8,0x81,3,rm,3,0x3,MODSIBDISP,32,imm
#define SBBlrm   8,0x1b,3,rm,3,reg,MODSIBDISP
#define SBBlmr   8,0x19,3,rm,3,reg,MODSIBDISP
#define SBBdef   CFLAG | ICFLAG | VFLAG | NFLAG | ZFLAG
#define SBBundef 0

#define SETAr    8,0x0f,8,0x97,3,rm,3,0x0,2,0x3
#define SETAm    8,0x0f,8,0x97,3,rm,3,0x0,MODSIBDISP
#define SETAEr   8,0x0f,8,0x93,3,rm,3,0x0,2,0x3
#define SETAEm   8,0x0f,8,0x93,3,rm,3,0x0,MODSIBDISP
#define SETBr    8,0x0f,8,0x92,3,rm,3,0x0,2,0x3
#define SETBm    8,0x0f,8,0x92,3,rm,3,0x0,MODSIBDISP
#define SETBEr   8,0x0f,8,0x96,3,rm,3,0x0,2,0x3
#define SETBEm   8,0x0f,8,0x96,3,rm,3,0x0,MODSIBDISP
#define SETCr    8,0x0f,8,0x92,3,rm,3,0x0,2,0x3
#define SETCm    8,0x0f,8,0x92,3,rm,3,0x0,MODSIBDISP
#define SETEr    8,0x0f,8,0x94,3,rm,3,0x0,2,0x3
#define SETEm    8,0x0f,8,0x94,3,rm,3,0x0,MODSIBDISP
#define SETGr    8,0x0f,8,0x9F,3,rm,3,0x0,2,0x3
#define SETGm    8,0x0f,8,0x9F,3,rm,3,0x0,MODSIBDISP
#define SETGEr   8,0x0f,8,0x9D,3,rm,3,0x0,2,0x3
#define SETGEm   8,0x0f,8,0x9D,3,rm,3,0x0,MODSIBDISP
#define SETLr    8,0x0f,8,0x9C,3,rm,3,0x0,2,0x3
#define SETLm    8,0x0f,8,0x9C,3,rm,3,0x0,MODSIBDISP
#define SETLEr   8,0x0f,8,0x9E,3,rm,3,0x0,2,0x3
#define SETLEm   8,0x0f,8,0x9E,3,rm,3,0x0,MODSIBDISP
#define SETNAr   8,0x0f,8,0x96,3,rm,3,0x0,2,0x3
#define SETNAm   8,0x0f,8,0x96,3,rm,3,0x0,MODSIBDISP
#define SETNAEr  8,0x0f,8,0x92,3,rm,3,0x0,2,0x3
#define SETNAEm  8,0x0f,8,0x92,3,rm,3,0x0,MODSIBDISP
#define SETNBr   8,0x0f,8,0x93,3,rm,3,0x0,2,0x3
#define SETNBm   8,0x0f,8,0x93,3,rm,3,0x0,MODSIBDISP
#define SETNBEr  8,0x0f,8,0x97,3,rm,3,0x0,2,0x3
#define SETNBEm  8,0x0f,8,0x97,3,rm,3,0x0,MODSIBDISP
#define SETNCr   8,0x0f,8,0x93,3,rm,3,0x0,2,0x3
#define SETNCm   8,0x0f,8,0x93,3,rm,3,0x0,MODSIBDISP
#define SETNEr   8,0x0f,8,0x95,3,rm,3,0x0,2,0x3
#define SETNEm   8,0x0f,8,0x95,3,rm,3,0x0,MODSIBDISP
#define SETNGr   8,0x0f,8,0x9E,3,rm,3,0x0,2,0x3
#define SETNGm   8,0x0f,8,0x9E,3,rm,3,0x0,MODSIBDISP
#define SETNGEr  8,0x0f,8,0x9C,3,rm,3,0x0,2,0x3
#define SETNGEm  8,0x0f,8,0x9C,3,rm,3,0x0,MODSIBDISP
#define SETNLr   8,0x0f,8,0x9D,3,rm,3,0x0,2,0x3
#define SETNLm   8,0x0f,8,0x9D,3,rm,3,0x0,MODSIBDISP
#define SETNLEr  8,0x0f,8,0x9F,3,rm,3,0x0,2,0x3
#define SETNLEm  8,0x0f,8,0x9F,3,rm,3,0x0,MODSIBDISP
#define SETNOr   8,0x0f,8,0x91,3,rm,3,0x0,2,0x3
#define SETNOm   8,0x0f,8,0x91,3,rm,3,0x0,MODSIBDISP
#define SETNPr   8,0x0f,8,0x9B,3,rm,3,0x0,2,0x3
#define SETNPm   8,0x0f,8,0x9B,3,rm,3,0x0,MODSIBDISP
#define SETNSr   8,0x0f,8,0x99,3,rm,3,0x0,2,0x3
#define SETNSm   8,0x0f,8,0x99,3,rm,3,0x0,MODSIBDISP
#define SETNZr   8,0x0f,8,0x95,3,rm,3,0x0,2,0x3
#define SETNZm   8,0x0f,8,0x95,3,rm,3,0x0,MODSIBDISP
#define SETOr    8,0x0f,8,0x90,3,rm,3,0x0,2,0x3
#define SETOm    8,0x0f,8,0x90,3,rm,3,0x0,MODSIBDISP
#define SETPr    8,0x0f,8,0x9A,3,rm,3,0x0,2,0x3
#define SETPm    8,0x0f,8,0x9A,3,rm,3,0x0,MODSIBDISP
#define SETPEr   8,0x0f,8,0x9A,3,rm,3,0x0,2,0x3
#define SETPEm   8,0x0f,8,0x9A,3,rm,3,0x0,MODSIBDISP
#define SETPOr   8,0x0f,8,0x9B,3,rm,3,0x0,2,0x3
#define SETPOm   8,0x0f,8,0x9B,3,rm,3,0x0,MODSIBDISP
#define SETSr    8,0x0f,8,0x98,3,rm,3,0x0,2,0x3
#define SETSm    8,0x0f,8,0x98,3,rm,3,0x0,MODSIBDISP
#define SETZr    8,0x0f,8,0x94,3,rm,3,0x0,2,0x3
#define SETZm    8,0x0f,8,0x94,3,rm,3,0x0,MODSIBDISP
#define SETdef   0
#define SETundef 0

#define SHRlrc   8,0xd3,3,rm,3,0x5,2,0x3
#define SHRlri   8,0xc1,3,rm,3,0x5,2,0x3,8,imm
#define SHRdef   CFLAG
#define SHRundef VFLAG | NFLAG | ZFLAG

#define STC      8,0xf9
#define STCdef   CFLAG
#define STCundef 0

#define SUBlrr   8,0x29,3,rm,3,reg,2,0x3
#define SUBlri   8,0x81,3,rm,3,0x5,2,0x3,32,imm
#define SUBlmi   8,0x81,3,rm,3,0x5,MODSIBDISP,32,imm
#define SUBlrm   8,0x2b,3,rm,3,reg,MODSIBDISP
#define SUBlmr   8,0x29,3,rm,3,reg,MODSIBDISP
#define SUBdef   CFLAG | ICFLAG | VFLAG | NFLAG | ZFLAG
#define SUBundef 0

#define TESTlrr  8,0x85,3,rm,3,reg,2,0x3
#define TESTlri  8,0xf7,3,rm,3,0x0,2,0x3,32,imm
#define TESTlrm  8,0x85,3,rm,3,reg,MODSIBDISP
#define TESTlmr  TESTlrm
#define TESTdef  NFLAG | ZFLAG
#define TESTundef CFLAG | VFLAG

#define XADDlrr  8,0x0f,8,0xc1,3,rm,3,reg,2,0x3
#define XADDlmr  8,0x0f,8,0xc1,3,rm,3,reg,MODSIBDISP
#define XADDlrm  XADDlmr
#define XADDdef  CFLAG | VFLAG | NFLAG | ZFLAG
#define XADDundef 0

#define XCHGlrr  8,0x87,3,rm,3,reg,2,0x3
#define XCHGlmr  8,0x87,3,rm,3,reg,MODSIBDISP
#define XCHGlrm  XCHGlmr
#define XCHGdef  0
#define XCHGundef 0

#define XORlrr   8,0x31,3,rm,3,reg,2,0x3
#define XORlri   8,0x81,3,rm,3,0x6,2,0x3,32,imm
#define XORlmi   8,0x81,3,rm,3,0x6,MODSIBDISP,32,imm
#define XORlrm   8,0x33,3,rm,3,reg,MODSIBDISP
#define XORlmr   8,0x31,3,rm,3,reg,MODSIBDISP
#define XORdef   NFLAG | ZFLAG
#define XORundef CFLAG | VFLAG

// parts of instruction to be plugged into above defines
#define SIB          3,base,3,index,2,scale

#define MOD          2,mod
#define MODDISP8     2,mod,8,disp
#define MODDISP32    2,mod,32,disp
#define MODSIB       2,mod,SIB
#define MODSIBDISP8  2,mod,SIB,8,disp
#define MODSIBDISP32 2,mod,SIB,32,disp

// value "index" can take in sib byte
#define NOINDEX      4
// values "rm" can take in memory-access-type instructions
#define USESIB       4
#define USEDISP      5

typedef enum {
  EAX,
	ECX,
	EDX,
	EBX,
	ESP,
	EBP,
	ESI,
	EDI
} regfield;

typedef enum {
  AL,
  CL,
  DL,
  BL,
  AH,
  CH,
  DH,
  BH
} byteregfield;

// these scale values multiply index register
typedef enum {
  scale_1,
	scale_2,
	scale_4,
	scale_8
} scalevalue;

typedef enum {
  mod_INDIRECT,
	mod_DISP8,
	mod_DISP32,
	mod_REGISTER
} modvalue;

// a variable-length block of shiny native code
typedef struct {
  uint3* base;
  uint5 length;
  uint5 size;
  jt_list* reloc;
} nativeblockinfo;

extern nativeblockinfo* x86asm_newnative(void);
extern void x86asm_deletenative(nativeblockinfo* nat);
extern void x86asm_out(nativeblockinfo* nat, ...);
extern void x86asm_swi(registerinfo* reg);

#endif
