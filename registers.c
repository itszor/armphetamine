#include <stdio.h>

#include "cnew.h"
#include "defs.h"
#include "registers.h"

registerinfo* registers_new(void)
{
	registerinfo* reg = cnew(registerinfo);
	int i;
	
	for (i=0; i<16; i++) reg->r[i] = 0;
  reg->pcmask = 0x0c000003;  // user mode bits

//	fprintf(stderr, "Initialised register bank\n");
	
	return reg;
}
