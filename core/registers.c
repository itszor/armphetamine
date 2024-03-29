#include <stdio.h>

#include "libjtype/cnew.h"
#include "libjtype/defs.h"
#include "core/registers.h"

registerinfo* registers_new(void)
{
	registerinfo* reg = jt_new(registerinfo);
	int i;
	
	for (i=0; i<16; i++) reg->r[i] = 0;
  reg->pcmask = 0x0c000003;  // user mode bits

  reg->cpsr.value = 0;
  reg->cpsr.flag.mode = pm_SVC32;
  reg->spsr_current = 3;
  reg->vectorbase = 0;

  for (i=0; i<5; i++)
  {
    reg->spsr[i].value = 0;
  }

  for (i=0; i<7; i++)
  {
    reg->usr[i] = 0;
    reg->fiq[i] = 0;
  }

  for (i=0; i<2; i++)
  {
    reg->irq[i] = 0;
    reg->svc[i] = 0;
    reg->abt[i] = 0;
    reg->und[i] = 0;
  }

  reg->mode = reg->intmask = 0;
  reg->process_id = 0;

//	fprintf(stderr, "Initialised register bank\n");
	
	return reg;
}

void registers_clone(registerinfo* dest, registerinfo* src)
{
  uint5 i;
  
  for (i=0; i<16; i++)
  {
    dest->r[i] = src->r[i];
  }
  dest->mode = src->mode;
  dest->intmask = src->intmask;
  dest->cpsr = src->cpsr;
  dest->pcmask = src->pcmask;
  for (i=0; i<7; i++)
  {
    dest->usr[i] = src->usr[i];
    dest->fiq[i] = src->fiq[i];
  }
  for (i=0; i<2; i++)
  {
    dest->irq[i] = src->irq[i];
    dest->svc[i] = src->svc[i];
    dest->abt[i] = src->abt[i];
    dest->und[i] = src->und[i];
  }
  for (i=0; i<5; i++)
  {
    dest->spsr[i] = src->spsr[i];
  }
  dest->spsr_current = src->spsr_current;
}

#define BOMB(A,B,C) if ((A) != (B)) { \
    fprintf(stderr, "%s != %s, n=%d!\n", #A, #B, C); \
    abort(); \
  }

void registers_diff(registerinfo* a, registerinfo* b)
{
  uint5 i;
  
  for (i=0; i<16; i++)
  {
    BOMB(a->r[i], b->r[i], i);
  }
  BOMB(a->mode, b->mode, 0);
  BOMB(a->intmask, b->intmask, 0);
  BOMB(a->cpsr.value, b->cpsr.value, 0);
  BOMB(a->pcmask, b->pcmask, 0);
  for (i=0; i<7; i++)
  {
    BOMB(a->usr[i], b->usr[i], i);
    BOMB(a->fiq[i], b->fiq[i], i);
  }
  for (i=0; i<2; i++)
  {
    BOMB(a->irq[i], b->irq[i], i);
    BOMB(a->svc[i], b->svc[i], i);
    BOMB(a->abt[i], b->abt[i], i);
    BOMB(a->und[i], b->und[i], i);
  }
  for (i=0; i<5; i++)
  {
    BOMB(a->spsr[i].value, b->spsr[i].value, i);
  }
  BOMB(a->spsr_current, b->spsr_current, i);
}
