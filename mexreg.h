#ifndef MEXREG_H
#define MEXREH_H 1

#include "defs.h"
#include "memory.h"

struct mex_registers {
  uint5 dramconfig;
  uint5 casshift0;
  uint5 casshift1;
  uint5 casshift2;
  uint5 staticctrl0;
  uint5 staticctrl1;
  uint5 expansionctrl;
};

typedef struct mex_registers mex_registers;

extern mex_registers* mexreg_new(void);

extern uint5 sa1100_mexreg_read(meminfo* mem, uint5 physaddress);

extern void sa1100_mexreg_write(meminfo* mem, uint5 physaddress, uint5 data);

#endif
