#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <a.out.h>

#include "loadaout.h"

// load an a.out format binary, return entry point (as word offset)
int load_aout(int* memory, char* filename)
{
	struct stat filest;
	struct exec* header;
	FILE* bin;
	
	stat(filename, &filest);
	bin = fopen(filename, "r");
	if (!bin)
	{
	  fprintf(stderr, "Couldn't open file %s\n", filename);
		exit(1);
	}
	fread(memory, 1, filest.st_size, bin);
	fclose(bin);
	
	header = (struct exec*) memory;
	
	return 0x1030;  // hmm, fix this
}
