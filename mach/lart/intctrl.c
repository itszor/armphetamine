#include "mach/lart/intctrl.h"
#include "core/machine.h"
#include "core/registers.h"
#include "libjtype/cnew.h"

/* hmm. */
static const uint5 sourcemod[] =
  { 0x00000fff,  // GPIO bits
    0x00000fff,
    0x00000fff,
    0x00000fff,
    0x00000fff,
    0x00000fff,
    0x00000fff,
    0x00000fff,
    0x00000fff,
    0x00000fff,
    0x00000fff,
    0x00000fff,
    0x00001000,  // LCD ctrl
    0x00002000,  // Serial port 0
    0x00004000,  // Reserved
    0x00008000,  // Serial port 1b
    0x00010000,  // Serial port 2
    0x00020000,  // Serial port 3
    0x00040000,  // Serial port 4a
    0x00080000,  // Serial port 4b
    0x00100000,  // DMA channel 0
    0x00200000,  // DMA channel 1
    0x00400000,  // DMA channel 2
    0x00800000,  // DMA channel 3
    0x01000000,  // DMA channel 4
    0x02000000,  // DMA channel 5
    0x3c000000,  // OS timer
    0x3c000000,
    0x3c000000,
    0x3c000000,
    0xc0000000,  // RTC
    0xc0000000
  };


intctrl_registers* intctrl_new(void)
{
  intctrl_registers* ir = jt_new(intctrl_registers);
  ir->icip = ir->icmr = ir->iclr = ir->icfp = ir->icpr = ir->iccr = 0;
  return ir;
}

// clear interrupts for a particular unit
void intctrl_blank(machineinfo* machine, uint5 unit)
{
  intctrl_registers* icr = machine->mem->intctrl;
  uint5 bits = ~(1<<unit);

  icr->icpr &= bits;
  icr->icip &= bits;
  icr->icfp &= bits;
}

/* intsrc 0..31 */
void intctrl_add(machineinfo* machine, uint5 intsrc)
{
  intctrl_registers* icr = machine->mem->intctrl;
  
  icr->icpr |= intsrc;
  icr->icip |= intsrc & icr->icmr & ~icr->iclr;
  icr->icfp |= intsrc & icr->icmr & icr->iclr;
}

void intctrl_fire(machineinfo* machine)
{
  intctrl_registers* icr = machine->mem->intctrl;
  registerinfo* reg = machine->reg;
  meminfo* mem = machine->mem;

  if (icr->icfp && !(reg->cpsr.flag.interrupt & 0x1) &&
    mem->currentmode!=pm_IRQ32 && mem->currentmode!=pm_FIQ32)
  {
    fprintf(stderr, "Firing FIQ!\n");
    processor_fiq(machine);
    icr->icpr = 0;
    icr->icfp = 0;
  }
  else if (icr->icip && !(reg->cpsr.flag.interrupt & 0x2) &&
    mem->currentmode!=pm_FIQ32)
  {
    fprintf(stderr, "Firing IRQ!\n");
    processor_irq(machine);
    icr->icpr = 0;
    icr->icip = 0;
  }
}

