#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "x86asm.h"
#include "x86dism.h"

void x86dism_block(nativeblockinfo* nat)
{
  x86dism_partblock(nat, 0, nat->length);
}

// Disassemble a block of x86 code using 'objdump'
void x86dism_partblock(nativeblockinfo* nat, uint5 start, uint5 length)
{
  char template[] = "/tmp/x86codeXXXXXX";
	FILE* temp;
		
  mkstemp(template);
	temp = fopen(template, "w+b");
	fwrite(nat->base+start, sizeof(char), length-start, temp);
	fclose(temp);

	fprintf(stderr, "Disassembling x86 code at %x...\n", nat->base);
	
	if (fork())
  {
    wait(0);
    remove(template);
    fprintf(stderr, "\n");
    return;
  }
	
	execl("/usr/bin/objdump", "--full-contents", "--target", "binary",
	      "--architecture=i386", "--disassemble-all", template, 0);
}
