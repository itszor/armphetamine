#include <stdio.h>

#include "cnew.h"
#include "defs.h"
#include "registers.h"

registerinfo* registers_new(void)
{
	registerinfo* reg = cnew(registerinfo);
	int i;
	
	for (i=0; i<16; i++) reg->r[i] = 0;

//	fprintf(stderr, "Initialised register bank\n");
	
	return reg;
}
