#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "defs.h"
#include "iomd.h"
#include "keyboard.h"
#include "mouse.h"
#include "machine.h"

#ifdef EMULART

void ostimer_write(meminfo* mem, uint5 address, uint5 data)
{
}


uint5 ostimer_read(meminfo* mem, uint5 address)
{ 
  return 0xdeadbeef;
}

#endif
