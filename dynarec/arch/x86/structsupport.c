#include <stdio.h>
#include <stddef.h>

#include "defs.h"
#include "memory.h"

#define MEMOFFSET(X) printf("%%define meminfo_"#X" %d\n", \
                       offsetof(meminfo, X));

int main(void)
{
  MEMOFFSET(currentmode)
  MEMOFFSET(bank0)
  MEMOFFSET(bank1)
  MEMOFFSET(bank2)
  MEMOFFSET(bank3)
  
  return 0;
}
