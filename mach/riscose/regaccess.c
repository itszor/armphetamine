#include "defs.h"
#include "machine.h"
#include "registers.h"
#include "execute.h"

static machineinfo* machine;

void arm_init(void)
{
  machine = machine_create(0);
  /* other setup */
}

int arm_get_reg(uint5 regno)
{
  registerinfo* reg = machine->reg;
  return RGET(regno);
}

void arm_set_reg(uint5 regno, uint5 value)
{
  registerinfo* reg = machine->reg;
  RPUT(regno, value);
}

int arm_get_r15_all(void)
{
  registerinfo* reg = machine->reg;
  return RGET(15);
}

void arm_set_pc(uint5 value)
{
  registerinfo* reg = machine->reg;
  RPUT(15, value);
}

extern int native_execution;

void arm_set_c(void)
{
  registerinfo* reg = machine->reg;
  if (native_execution)
    reg->cflag = 1;
  else
    (((pcinfo*)&reg->r[15])->flag.c) = 1;
}

void arm_clear_c(void)
{
  registerinfo* reg = machine->reg;
  if (native_execution)
    reg->cflag = 0;
  else
    (((pcinfo*)&reg->r[15])->flag.c) = 0;
}

void arm_set_v(void)
{
  registerinfo* reg = machine->reg;
  if (native_execution)
    reg->vflag = 1;
  else
    (((pcinfo*)&reg->r[15])->flag.v) = 1;
}

void arm_clear_v(void)
{
  registerinfo* reg = machine->reg;
  if (native_execution)
    reg->vflag = 0;
  else
    (((pcinfo*)&reg->r[15])->flag.v) = 0;
}

void arm_run_routine(uint5 address)
{
  /* do it! */
}

void arm_return(void)
{
  /* what? */
}
