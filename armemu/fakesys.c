#include <stdio.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "defs.h"
#include "machine.h"
#include "decode.h"
#include "memory.h"
#include "registers.h"
#include "processor.h"
#include "execute.h"
#include "fakesys.h"
#include "riscos.h"

// this doesn't work and is clearly The Wrong Thing To Do
#undef FAKE_NETBSD

#ifdef FAKE_NETBSD

// Evil macro; get syscall argument number 'X'
#define ARG(X) (temp=indirect?(X)+1:(X), temp<4?GET(temp):\
  *memory_lookup(machine->mem, (uint5*)(GET(13)+(temp-4)*4)))

void fake_syscall(machineinfo* machine, uint5 number)
{
	registerinfo* reg = machine->reg;
	int indirect = (number==0 || number==198), temp;
	int x, sysno = indirect ? GET(0) : number;
	
	for (x=0; x<16; x++)
	{
	  fprintf(stderr, "r%d=%d ", x, GET(x));
	}
	puts("");

	switch (sysno)
	{
		case 0x3:      // SYS_read
#ifdef VERBOSESYSCALL
    fprintf(stderr, "syscall read() made\n");
#endif
		PUT(0, (uint5)read(ARG(0), (void*)memory_lookup(machine->mem,
		  (uint5*)ARG(1)), (size_t)ARG(2)));
		break;

		case 0x4:      // SYS_write
#ifdef VERBOSESYSCALL
    fprintf(stderr, "syscall write() made\n");
#endif
    fprintf(stderr, "  write(%d,%x,%d)\n", ARG(0), ARG(1), ARG(2));
		PUT(0, (uint5)write(ARG(0), (const void*)memory_lookup(machine->mem,
		  (uint5*)ARG(1)), (size_t)ARG(2)));
		break;
		
		case 0x11:
#ifdef VERBOSESYSCALL
		fprintf(stderr, "syscall break() made (0x11)\n");
#endif
    // 'brk' supposedly returns 0 on success, so here goes...
		PUT(0,1024*1024);
		break;
		
		case 0x3a:   // readlink
#ifdef VERBOSESYSCALL
		fprintf(stderr, "called readlink(%s,%x,%d)\n",
		  (char*)memory_lookup(machine->mem, (uint5*)ARG(0)), ARG(1), ARG(2));
#endif
		PUT(0, readlink((const char*)ARG(0), (char*)ARG(1), (size_t)ARG(2)));
		break;
		
		case 0xc5:   // mmap
		{
		  static uint5 mapbase = 1024*1024;
	//		struct stat sb;
		  fprintf(stderr, "Passed args: %x,%d,%d,%d,%d,%d\n", ARG(0), ARG(1), 
			  ARG(2), ARG(3), ARG(4), ARG(5));
		/*	fstat(ARG(4), &sb);
			fprintf(stderr, "sb.st_size=%d\n", sb.st_size);*/
		  PUT(0,mapbase);
		}
		break;
		
		case 0xca:
#ifdef VERBOSESYSCALL
		fprintf(stderr, "syscall sysctl() made (0xca)\n");
#endif
    {
		  uint5* name = (uint5*)ARG(0);
			uint5 nlen = ARG(1);
			void* oldval = (void*)ARG(2);
			size_t* oldlen = (size_t*)ARG(3);
			void* newp = (void*)ARG(4);
			size_t newlen = (size_t)ARG(5);
    	fprintf(stderr, "  sysctl(%x,%d,%x,%x,%x,%d)\n",
		  	name, nlen, oldval, oldlen, newp, newlen);
			switch (*memory_lookup(machine->mem, name))
			{
			  case 0:
				fprintf(stderr, "Ignoring top-level name zero (unused?)\n");
				break;
				
			  case 6:     // CTL_HW, generic CPU/IO
				switch (*memory_lookup(machine->mem, &name[1]))
				{
				  case 7:      // HW_PAGESIZE
					PUT(0, 4096);   // pick a number, any number...
					break;
					
					default:
  				printf("Next-level name for CTL_HW=%d (unsupported)\n",
					  *memory_lookup(machine->mem, &name[1]));
					exit(1);
				}
				break;
				
			  default:
				fprintf(stderr, "Top-level name=%d (unsupported)\n",
				  *memory_lookup(machine->mem, name));
				exit(1);
			}
		}
		break;
		
		case 0x117:
#ifdef VERBOSESYSCALL
		fprintf(stderr, "syscall fstat(%d,%x) made (0x117)\n", ARG(0), ARG(1));
#endif
//		fstat((int)ARG(0),(struct stat*)ARG(1));
		PUT(0,0);   // successful...
		break;
		
		default:
		fprintf(stderr, "Unrecognized SWI! (%x)\n", sysno);
	}
}
#else

void fake_syscall(machineinfo* machine, uint5 number)
{
/*  switch (number)
	{
	  case 0:  // putchar
		putchar(reg->r[0]);
		break;
	}*/
  
  riscos_swi(riscosstate, machine->mem, machine->reg, number);
}	

#endif

