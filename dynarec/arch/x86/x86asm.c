#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#include "libjtype/cnew.h"
#include "libjtype/defs.h"
#include "core/machine.h"
#include "libjtype/hash.h"
#include "dynarec/pseudo.h"
#include "dynarec/arch/x86/x86asm.h"
#include "core/registers.h"

nativeblockinfo* x86asm_newnative(void)
{
  nativeblockinfo* nat = cnew(nativeblockinfo);
  nat->base = cnewarray(char, nat->size=2048);
  nat->length = 0;
  nat->expecting = 0;
  nat->reloc = 0;
  return nat;
}

void x86asm_deletenative(nativeblockinfo* nat)
{
  free(nat->base);
  free(nat);
}

// assemble an x86 instruction from a series of bit-blocks
void x86asm_out(nativeblockinfo* nat, ...)
{
	sint5 bit = 0, nb;
	uint5 argval;
	uint5* outc;
	va_list ap;
		
	va_start(ap, nat);
	
	while (nb=va_arg(ap, uint5))
	{
		if (!bit) nat->base[nat->length] = 0;
		argval = va_arg(ap, uint5);
		while (nb>0)
		{
		  uint5 chunk = nb>8 ? 8 : nb;
			nat->base[nat->length] |= (argval << bit) & 0xff;
			bit += chunk;
			argval >>= chunk;
			nb -= chunk;
			if (bit>=8)
			{
	  		bit -= 8;
        if (++nat->length==nat->size)
          nat->base = realloc(nat->base, nat->size*=2);
			}
		}
  }
	
	va_end(ap);

  if (bit!=0) fprintf(stderr, "Instruction doesn't seem to fill byte?\n");
	
//	fprintf(stderr, "Written %d bytes\n", byte);
}
