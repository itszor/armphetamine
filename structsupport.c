#include <stdio.h>
#include <stddef.h>

#include "defs.h"
#include "memory.h"

int main(void)
{
  printf("%%define meminfo_currentmode %d\n",
    offsetof(meminfo, currentmode));
  
  return 0;
}
