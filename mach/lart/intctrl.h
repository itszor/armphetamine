#ifndef INTCTRL_H
#define INTCTRL_H 1

#include "libjtype/defs.h"
#include "core/machine.h"

struct intctrl_registers {
  uint5 icip;  // IRQ pending
  uint5 icmr;  // mask register
  uint5 iclr;  // level register
  uint5 icfp;  // FIQ pending
  uint5 icpr;  // Pending register
  uint5 iccr;  // Control register
};

typedef struct intctrl_registers intctrl_registers;

extern intctrl_registers* intctrl_new(void);

extern void intctrl_blank(machineinfo* machine, uint5 unit);

extern void intctrl_add(machineinfo* machine, uint5 intsrc);

extern void intctrl_fire(machineinfo* machine);

extern void intctrl_pending(machineinfo* machine);

#endif
