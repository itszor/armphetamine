#ifndef OSTIMER_H
#define OSTIMER_H 1

#include "core/memory.h"
#include "core/machine.h"

struct ostimer_registers {
  uint5 clock;
  uint5 osmr[4];  // match registers 0..3
  uint5 ower;     // watchdog match enable
  uint5 ossr;     // timer status register
  uint5 oier;     // interrupt enable register
};

typedef struct ostimer_registers ostimer_registers;

extern ostimer_registers* ostimer_new(void);
extern void ostimer_write(meminfo* mem, uint5 address, uint5 data);
extern uint5 ostimer_read(meminfo* mem, uint5 address);
extern void ostimer_clock(machineinfo* machine);

#endif
