#include "intctrl.h"
#include "cnew.h"

intctrl_registers* intctrl_new(void)
{
  intctrl_registers* ir = cnew(intctrl_registers);
  ir->icip = ir->icmr = ir->iclr = ir->icfp = ir->icpr = ir->iccr = 0;
  return ir;
}
