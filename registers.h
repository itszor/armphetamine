#ifndef REGISTERS_H
#define REGISTERS_H 1

#include "defs.h"
#include "processor.h"

typedef enum {
  spsr_FIQ,
	spsr_SVC,
	spsr_ABT,
	spsr_IRQ,
	spsr_UND
} spsrnumber;

typedef struct {
  uint5 r[16];			// live register set
 /* uint5 t[6]; */      // temporary cache
  // expanded out in dynarec code...
  uint3 vflag, cflag, zflag, nflag;
  uint3 predbuf[16];  // probably only need to be 14 really
  uint3 npredbuf[16];
/*  uint5 base, scratch;*/
  uint5 mode, intmask;
  psrinfo cpsr;
  uint5 pcmask;
  // only used by mode switch code (not written yet...)
  uint5 usr[7];		  // saved 'usr' registers
  uint5 fiq[7];		  // saved 'fiq' registers
  uint5 irq[2];		  // saved 'irq' registers
  uint5 svc[2];		  // saved 'svc' registers
  uint5 abt[2];		  // saved 'abt' registers
  uint5 und[2];		  // saved 'und' registers
  psrinfo spsr[5];
  uint5 spsr_current;
} registerinfo;

extern registerinfo* registers_new(void);

#endif
