#include <stdio.h>
#include <signal.h>
#include <sys/times.h>

#include "defs.h"
#include "cnew.h"
#include "riscos.h"
#include "execute.h"

// This file borrows heavily from Edwin Dorre's armemu for DOS

osstateinfo* riscosstate;
uint5 nativehack = 0;

#define FLAG_(N) (nativehack ? reg->N##flag \
                             : (((pcinfo*)&reg->r[15])->flag.N))

void riscos_escapehandler(int x)
{
  riscosstate->escape = 1;
}

osstateinfo* riscos_initialise(void)
{
  uint5 i, j;
  osstateinfo* newstate = cnew(osstateinfo);

  for (i=0; i<18; i++)
    for (j=0; j<3; j++)
      newstate->environment[i][j] = 0;

  newstate->vdulen = 0;
  newstate->vduptr = 0;
  newstate->escape = 0;
  newstate->escapekey = 27;

  signal(SIGQUIT, riscos_escapehandler);

  return newstate;
}

// this is broken for non-flat address space
void riscos_stringcopy(meminfo* mem, uint5* to, uint5* from)
{
  uint3* fromptr = (uint3*)memory_lookup(mem, (uint5*)from);
  uint3* toptr = (uint3*)memory_lookup(mem, (uint5*)to);

  while (*fromptr && *fromptr!=13) *toptr++ = *fromptr++;
  *toptr = 0;
}

// limited VDU driver
void riscos_vdu(osstateinfo* os, uint3 ch)
{
  if (os->vdulen==0 && ch>=32)
  {
    putchar(ch);
    return;
  }

  os->vduqueue[os->vduptr++] = ch;

  switch (os->vduqueue[0])
  {
    case 1: os->vdulen = 2; break;
    case 17: os->vdulen = 2; break;
    case 18: os->vdulen = 3; break;
    case 19: os->vdulen = 6; break;
    case 22: os->vdulen = 2; break;
    case 23: os->vdulen = 10; break;
    case 24: os->vdulen = 9; break;
    case 25: os->vdulen = 6; break;
    case 28: os->vdulen = 5; break;
    case 29: os->vdulen = 5; break;
    case 31: os->vdulen = 3; break;
    default: os->vdulen = 1;
  }

  if (os->vduptr == os->vdulen)
  {
    os->vdulen = 0;
    switch (os->vduqueue[0])
    {
      case 0: // nothing
      case 1: // send next character to printer
      case 2: // printer on
      case 3: // printer off
      case 4: // join text & graphics cursors
      case 5: // split text & graphics cursors
      case 6: // enable screen output
      break;

      case 7: // beep
      case 8: // backspace
      case 9: // horizontal tab
      case 10: // line feed
      case 11: // vertical tab
      case 12: // formfeed/clear screen
      case 13: // carriage return
      putchar(os->vduqueue[0]);
      break;

      case 14: // page mode on
      case 15: // page mode off
      case 16: // clear graphics window
      case 17: // set text colour
      case 18: // set graphics colour
      case 19: // set palette
      case 20: // restore default colours
      case 21: // disable screen display
      break;

      case 22: // change display mode
      break;

      case 23: // miscellaneous commands
      case 24: // define graphics window
      break;

      case 25: // general plot command
      break;

      case 26: // restore default windows
      case 27: // no operation
      break;

      case 28: // define text window
      break;

      case 29: // define graphics origin
      break;

      case 30: // home text cursor
      break;

      case 31: // position text cursor
      break;
    }
    os->vduptr = 0;
  }
}

void riscos_readline(osstateinfo* os, meminfo* mem, registerinfo* reg)
{
  uint5 buf = reg->r[0] & 0x3fffffffU;
  uint3* buffer = (uint3*)memory_lookup(mem, (uint5*)buf);
  uint3 lo = reg->r[2], hi = reg->r[3], echo = reg->r[4];
  uint3 enteronly = buf	& 0x80000000U ? 1 : 0;
  uint3 obscure = buf & 0x40000000U ? 1 : 0;
  uint5 done = 0, ptr = 0, i;

  if (os->escape)
  {
    FLAG_(c) = 1;
    reg->r[0] = 0x1b;
    reg->r[1] = 0;
    return;
  }

  while (!done)
  {
    uint3 ch = getchar();
    switch (ch)
    {
      case 8:
      case 127:
      if (ptr) buffer[--ptr] = 0;
      break;

      case 10:
      case 13:
      done = 1;
      break;

      case 21:
      while (ptr)
      {
        buffer[--ptr] = 0;
        putchar(127);
      }
      break;

      default:
      if (ch>=lo && ch<=hi)
      {
        buffer[ptr++] = ch;
      }
    }
  }
//  fprintf(stderr, "Read a line: %s\n", buffer);
  FLAG_(c) = 0;
  buffer[ptr] = 13; // terminate the line?
  reg->r[1] = ptr;
}

uint5 riscos_inkey(uint5 lo, uint5 hi)
{
  return getchar();
}

void riscos_swi(osstateinfo* os, meminfo* mem, registerinfo* reg, uint5 num)
{
  int xform = (num & XOS_Bit) ? 1 : 0;
//  fprintf(stderr, "Calling SWI %x, r0=%d\n", num, reg->r[0]);

  if (num>=256 && num<=511)
  {
    riscos_vdu(os, num-256);
    return;
  }

  num &= ~XOS_Bit;

  switch (num)
  {
    case OS_Byte:
    fprintf(stderr, "OS_Byte %d called\n", reg->r[0]);
    switch (reg->r[0])
    {
      case 124:  // clear escape condition
      os->escape = 0;
      break;

      case 125:  // set escape condition
      os->escape = 1;
      break;

      case 126:  // acknowledge escape condition
      if (os->escape)
      {
        reg->r[1] = 255;
        os->escape = 0;
        printf("Escape\n");
      } else reg->r[1] = 0;
      break;

      case 128: // get buffer/mouse status
      reg->r[0] = reg->r[1] = reg->r[2] = 0;
      break;

      case 129:  // scan for a particular key
      if (reg->r[2] < 0x80)
      {
        if (os->escape)
        {
          reg->r[1] = 0;
          reg->r[2] = 0x1b;
        }
        else
        {
          reg->r[1] = riscos_inkey(reg->r[1], reg->r[2]);
          reg->r[2] = reg->r[1]==-1 ? 255 : 0;
        }
      }
      else
      {
        reg->r[1] = reg->r[2] = 0;
      }
      break;

      case 218:  // read/write bytes in VDU queue
      {
        uint5 was = os->vdulen - os->vduptr;
        // humm, something like this anyway
        os->vdulen = ((was ^ 0xff) & reg->r[2]) ^ reg->r[1];
        reg->r[1] = 0xff ^ was;
//        abort();
      }
      break;

      default:
      fprintf(stderr, "Unknown OS_Byte call: %d\n", reg->r[0]);
    }
    break;

    case OS_ReadMonotonicTime:
    {
      struct tms time_info;
      times(&time_info);
      reg->r[0] = time_info.tms_utime;
    }
    break;
 
    case OS_ChangeEnvironment:
    if (reg->r[0] < 17)
    {
      uint5 a = os->environment[reg->r[0]][0];
      uint5 b = os->environment[reg->r[0]][1];
      uint5 c = os->environment[reg->r[0]][2];
      if (reg->r[1]) os->environment[reg->r[0]][0] = reg->r[1];
      if (reg->r[2]) os->environment[reg->r[0]][1] = reg->r[2];
      if (reg->r[3]) os->environment[reg->r[0]][2] = reg->r[3];
      reg->r[1] = a;
      reg->r[2] = b;
      reg->r[3] = c;
    }
    break;

    case OS_Exit:
    if (os->environment[environment_Exit][0])
    {
      reg->r[15] = os->environment[environment_Exit][0]+8;
      reg->r[12] = os->environment[environment_Exit][1];
    }
    else
    {
      printf("OS_Exit with no handler\n");
      exit(0);
    }
    break;

    case OS_GenerateError:
    if (xform)
    {
      FLAG_(v) = 1;
      break;
    }
    if (os->environment[environment_Error][0])
    {
      uint5 a = os->environment[environment_Error][2];
      reg->r[15] = os->environment[environment_Error][0]+8;

      *memory_lookup(mem, (uint5*)a) = reg->r[15];
      *memory_lookup(mem, (uint5*)(a+4)) = reg->r[0];

      riscos_stringcopy(mem, (uint5*)(a+8), (uint5*)(reg->r[0]+4));

      reg->r[0] = os->environment[environment_Error][1];
    }
    else
    {
      printf("Error: %s\n", memory_lookup(mem, (uint5*)(reg->r[0]+4)));
    }
    break;

    case OS_Plot:
    break;

    case OS_ReadLine:
    riscos_readline(os, mem, reg);
    break;

    case OS_ReadC:
    if (os->escape)
    {
      FLAG_(c) = 1;
      reg->r[0] = 0x1b;
      break;
    }
    reg->r[0] = getchar();
    if (reg->r[0] == os->escapekey)
    {
      os->escape = 1;
      FLAG_(c) = 1;
      reg->r[0] = 0x1b;
    }
    else FLAG_(c) = 0;
    break;

    case OS_WriteC:
    riscos_vdu(os, reg->r[0]);
    break;

    case OS_Write0:
    {
      uint5 addr = reg->r[0], ch;
      while (ch = *(uint3*)memory_lookup(mem, (uint5*)(addr++)))
        riscos_vdu(os, ch);
    }
    break;

    // ew! STUFF LIKE WriteS means that *all* swis must be followed by a ret!!!
    case OS_WriteS:
    {
      uint5 addr = GET(15)-4, ch, len = 0;
      while (ch = *(uint3*)memory_lookup(mem, (uint5*)(addr++)))
      {
        riscos_vdu(os, ch);
        len++;
      }
      len = (len+4)&~3;
      reg->r[15] += len;
    }
    break;

    case OS_WriteN:
    {
      uint5 addr = reg->r[0];
      uint5 len;
      for (len=reg->r[1]; len>0; len--)
        riscos_vdu(os, *(uint3*)memory_lookup(mem, (uint5*)(addr++)));
    }
    break;

    case OS_NewLine:
    riscos_vdu(os, 10);
    riscos_vdu(os, 13);
    break;

    case OS_GetEnv:
    {
	  static char* env = "BASIC";
      reg->r[0] = 0x800;
      strcpy((char*)memory_lookup(mem, (uint5*)reg->r[0]), env);
//      riscos_stringcopy(mem, (uint5*)reg->r[0], (uint5*)env);
//    we've got 256k! wow!
      reg->r[1] = 256*1024;
      reg->r[2] = 0x808;
      break;
    }

    case OS_ConvertHex8:
    snprintf((char*)memory_lookup(mem, (uint5*)reg->r[1]), reg->r[2], "%08lX",
      reg->r[0]);
    break;

    case OS_CLI:
    {
      uint5 i=0;
      uint3 ch;
      fprintf(stderr, "CLI called with:\n");
      while ((ch = ((char*)memory_lookup(mem, (uint5*)(reg->r[0])))[i++])>=32)
        fprintf(stderr, "%c", ch);
      fprintf(stderr, "\n");
    }
    break;

    case OS_Word:
    break;

    case OS_File:
    fprintf(stderr, "OS_File called, unimplemented\n");
    break;

    case OS_Find:
    fprintf(stderr, "OS_Find not implemented\n");
    exit(0);
    break;

    case OS_SynchroniseCodeAreas:
	// This can be made to invalidate bits of recompiled code
  	break;

    case BASICTrans_Error:
    // This causes default messages to be printed
    FLAG_(v) = 1;
    break;
    
    case BASICTrans_Message:
    FLAG_(v) = 1;
    break;

    default:
    fprintf(stderr, "Unknown SWI %x called\n", num);
  }
}
