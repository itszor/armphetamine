#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "x86asm.h"
#include "x86dism.h"

// Disassemble a block of x86 code using 'objdump'
void x86dism_block(nativeblockinfo* nat)
{
  char template[] = "/tmp/x86codeXXXXXX";
	FILE* temp;
		
  mkstemp(template);
	temp = fopen(template, "w+b");
	fwrite(nat->base, sizeof(char), nat->length, temp);
	fclose(temp);

	fprintf(stderr, "Disassembling x86 code at %x...\n", nat->base);
	
	if (fork())
  {
    wait(0);
    remove(template);
    return;
  }
	
	execl("/usr/bin/objdump", "--full-contents", "--target", "binary",
	      "--architecture=i386", "--disassemble-all", template, 0);
}
