#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <ctype.h>
#include <readline.h>
#include <history.h>

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
void riscos_stringcopy(meminfo* mem, uint5 to, uint5 from)
{
  uint3 byte;
  
  while (byte = memory_readbyte(mem, from++), byte!=0 && byte!=13)
    memory_writebyte(mem, to++, byte);
  
  memory_writebyte(mem, to, 0);
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
//  uint3* buffer = (uint3*)memory_lookup(mem, (uint5*)buf);
  uint3* rdbuf;
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

/*  while (!done)
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
  }*/
  
  putchar(8);  // not very nice...
  rdbuf = readline(">");  // ...but it works
//  strcpy(buffer, rdbuf);
  i = 0;
  while (rdbuf[i++]) memory_writebyte(mem, buf+i, rdbuf[i]);
  free(rdbuf);
  
//  fprintf(stderr, "Read a line: %s\n", buffer);
  FLAG_(c) = 0;
//  buffer[strlen(buffer)] = 13; // terminate the line?
  memory_writebyte(mem, buf+i, rdbuf[i]);
  reg->r[1] = ptr;
}

uint5 riscos_inkey(uint5 lo, uint5 hi)
{
  return getchar();
}


int STATE__running;
#define FALSE 0

void getstring(meminfo* mem, int src, char* buffer)
{
  int out;
  while (buffer[out++] = memory_readbyte(mem, src++));
}

void riscos_swi(osstateinfo* os, machineinfo* machine, uint5 num)
{
  int xform = (num & XOS_Bit) ? 1 : 0;
  meminfo* mem = machine->mem;
  registerinfo* reg = machine->reg;
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

      memory_writeword(mem, a, reg->r[15]);
      memory_writeword(mem, a+4, reg->r[0]);

      riscos_stringcopy(mem, a+8, reg->r[0]+4);

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
      while (ch = memory_readbyte(mem, addr++))
        riscos_vdu(os, ch);
    }
    break;

    // ew! STUFF LIKE WriteS means that *all* swis must be followed by a ret!!!
    case OS_WriteS:
    {
      uint5 addr = GET(15)-4, ch, len = 0;
      while (ch = memory_readbyte(mem, addr++))
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
        riscos_vdu(os, memory_readbyte(mem, addr++));
    }
    break;

    case OS_NewLine:
    riscos_vdu(os, 10);
    riscos_vdu(os, 13);
    break;

    case OS_GetEnv:
    {
      static char* env = "BASIC";
      int i = 0;
      reg->r[0] = 0x800;
      while (memory_writebyte(mem, reg->r[0]+i, env[i]), env[i++]);
//      strcpy((char*)memory_lookup(mem, (uint5*)reg->r[0]), env);
//      riscos_stringcopy(mem, (uint5*)reg->r[0], (uint5*)env);
//    memory limit (was 256*1024)
      reg->r[1] = 0x01000000 + 1024*1024;
      reg->r[2] = 0x808;
      break;
    }

    case OS_ConvertHex8:
    {
      static char buffer[8];
      int i;
      snprintf(&buffer[0], reg->r[2], "%08lX", reg->r[0]);
      for (i=0; i<8; i++) buffer[i] = memory_readbyte(mem, reg->r[1]+i);
    }
    break;

    case OS_CLI:
    {
      uint5 i=0;
      uint3 ch, buf[256];
  //    fprintf(stderr, "CLI called with:\n");

      while ((buf[i] = memory_readbyte(mem, reg->r[0]+i))>=32);

      buf[i-1] = 0;
      
      // ewww!!!
      if (!strcmp(buf, ".")) strcpy(buf, "ls");
      
      if (!strncmp(buf, "translate", 9))
      {
        int* start, *end;
        fprintf(stderr, "Attempting translation...\n");
        sscanf(&buf[9], "%x %x", &start, &end);
        fprintf(stderr, "start=%x end=%x\n", start, end);
        break;
      }

      system(buf);  // not very safe really?

//      fprintf(stderr, "\n");
    }
    break;

    case OS_Word:
    break;

    case OS_File:
    {
      char buf[256];
      int i=0;
      FILE* f;

      while ((buf[i++] = memory_readbyte(mem, reg->r[1]+i))>=32);

      buf[i-1] = 0;

      switch (reg->r[0])
      {
        case 0x00:
        case 0x0a:  // save files
        {
          f = fopen(buf, "wb");

          if (!f)
          {
            fprintf(stderr, "Can't open file %s for writing\n", buf);
          }
          else
          {
            char* m = malloc(reg->r[5]-reg->r[4]);
            int i;
            for (i=reg->r[4];
                 i<=reg->r[5];
                 m[i-reg->r[4]] = memory_readbyte(mem, i));
            fwrite(m, reg->r[5]-reg->r[4], 1, f);
            fclose(f);
            free(m);
          }
        }
        break;

        case 0x0c:
        case 0x0e:
        case 0x10:
        case 0xff:  // load files
        {
          struct stat fileinfo;
          stat(buf, &fileinfo);
          f = fopen(buf, "rb");

          if (!f)
          {
            fprintf(stderr, "Can't open file %s for reading\n", buf);
          }
          else
          {
            char* m = malloc(fileinfo.st_size);
            int i;
            if (reg->r[3] & 255) reg->r[2] = 0x8000;
            fread(m, fileinfo.st_size, 1, f);
            for (i=0; i<fileinfo.st_size; i++)
              memory_writebyte(mem, reg->r[2]+i, m[i]);
            fclose(f);
            free(m);
          }
        }
        break;
      }
    }
    break;

    case OS_Find:
    fprintf(stderr, "OS_Find not implemented\n");
    exit(0);
    break;

    case OS_SynchroniseCodeAreas:
	// This can be made to invalidate bits of recompiled code
    fprintf(stderr, "SynchroniseCodeAreas called!\n");
    flush_all(machine);
  	break;

    case BASICTrans_Error:
    // This causes default messages to be printed
    FLAG_(v) = 1;
    break;
    
    case BASICTrans_Message:
    {
      char* msg;
      int i;
      fprintf(stderr, "Attempting BASICTrans_Message, pc=%x\n", reg->r[15]);
      fprintf(stderr, "r3=%d\n", reg->r[3]);
      switch (reg->r[0])
      {
        case 0: msg="Unknown setting of exception control."; break;
        case 1: msg="Silly!"; break;
        case 2: msg="No room to do this renumber"; break;
        case 3: msg="Line numbers larger than 65279 would be generated by this "
                    "renumber"; break;
        case 4: msg="No room"; break;
        case 5: msg="Line too long"; break;
        case 6: msg="Stopped"; break;
        case 7: msg="Invalid LISTO option"; break;
        case 8: msg="Invalid TWINO option"; break;
        case 9: msg=""; break;
        case 10: msg="Error control status not found on stack for "
                     "RESTORE ERROR"; break;
        case 11: msg="Missing incore name"; break;
        case 12: msg="LIST/TWIN found line number reference"; break;
        case 13: msg="HELP has no information on this keyword"; break;
        case 14: msg="Incorrect in-core file description"; break;
        case 15: msg="INSTALL cannot be used in a program"; break;
        case 16: msg="No such mnemonic"; break;
        case 17: msg="No such suffix on EQU"; break;
        case 18: msg="Bad immediate constant"; break;
        case 19: msg="Bad address offset"; break;
        case 20: msg="Assembler limit reached"; break;
        case 21: msg="Bad shift"; break;
        case 22: msg="Bad register"; break;
        case 23: msg="Duplicate register in multiply"; break;
        case 24: msg="Missing ="; break;
        case 25: msg="Missing = in FOR statement"; break;
        case 26: msg="Mistake"; break;
        case 27: msg="Missing ,"; break;
        case 28: msg="Type mismatch: number needed"; break;
        case 29: msg="Type mismatch: numeric variable needed"; break;
        case 30: msg="Type mismatch: numeric array needed"; break;
        case 31: msg="Type mismatch: string needed"; break;
        case 32: msg="Type mismatch: string variable needed"; break;
        case 33: msg="Type mismatch: string array needed"; break;
        case 34: msg="Type mismatch: array needed"; break;
        case 35: msg="Type mismatch between arrays"; break;
        case 36: msg="Can't assign to array of this size"; break;
        case 37: msg="Array type mismatch as parameter"; break;
        case 38: msg="Can't SWAP arrays of different types"; break;
        case 39: msg="Not in a function"; break;
        case 40: msg="Too low a value for $<number>"; break;
        case 41: msg="Missing """; break;
        case 42: msg="DIM() function needs an array"; break;
        case 43: msg="No room to do matrix multiply with source(s) the same as "
                     "destination"; break;
        case 44: msg="Impossible dimension"; break;
        case 45: msg="No end of dimension list )"; break;
        case 46: msg="Bad DIM statement"; break;
        case 47: msg="Can't DIM negative amount"; break;
        case 48: msg="Arrays cannot be redimensioned"; break;
        case 49: msg="No room for this DIM"; break;
        case 50: msg="No room for this dimension"; break;
        case 51: msg="Attempt to allocate insufficient memory"; break;
        case 52: msg="Unreferenced local array in END="; break;
        case 53: msg="No room for program"; break;
        case 54: msg="Items can only be made local in a function or procedure"; 
                 break;
        case 55: msg="Not in a procedure"; break;
        case 56: msg="Reference array incorrect"; break;
        case 57: msg="Unknown array"; break;
        case 58: msg="Unknown array in DIM() function"; break;
        case 59: msg="Undimensioned array"; break;
        case 60: msg="Subscript out of range"; break;
        case 61: msg="Incorrect number of subscripts"; break;
        case 62: msg="Syntax error"; break;
        case 63: msg="Escape"; break;
        case 64: msg="Division by zero"; break;
        case 65: msg="String too long"; break;
        case 66: msg="Number too big"; break;
        case 67: msg="Number too big for arc Sine or arc Cosine"; break;
        case 68: msg="Negative root"; break;
        case 69: msg="Logarithm range"; break;
        case 70: msg="Accuracy lost in Sine/Cosine/Tangent"; break;
        case 71: msg="Exponent range"; break;
        case 72: msg="Unknown or missing variable"; break;
        case 73: msg="Can't use array reference here"; break;
        case 74: msg="Missing )"; break;
        case 75: msg="Missing ("; break;
        case 76: msg="Missing ]"; break;
        case 77: msg="Missing {"; break;
        case 78: msg="Missing }"; break;
        case 79: msg="Bad Hex"; break;
        case 80: msg="Hex number too large"; break;
        case 81: msg="Bad Binary"; break;
        case 82: msg="No such function/procedure"; break;
        case 83: msg="Bad call of function/procedure"; break;
        case 84: msg="Arguments of function/procedure incorrect"; break;
        case 85: msg="Invalid RETURN actual parameter"; break;
        case 86: msg="Invalid array actual parameter"; break;
        case 87: msg="Not in a FOR loop"; break;
        case 88: msg="Can't match FOR"; break;
        case 89: msg="Bad FOR control variable"; break;
        case 90: msg="The step cannot be zero"; break;
        case 91: msg="Missing TO"; break;
        case 92: msg="No room for function/procedure call"; break;
        case 93: msg="Not in a subroutine"; break;
        case 94: msg="ON syntax"; break;
        case 95: msg="ON range"; break;
        case 96: msg="No such line"; break;
        case 97: msg="Out of data"; break;
        case 98: msg="DATA pointer not found on stack for RESTORE DATA"; break;
        case 99: msg="Not in a REPEAT loop"; break;
        case 100: msg="Too many nested structures"; break;
        case 101: msg="Missing #"; break;
        case 102: msg="Not in a WHILE loop"; break;
        case 103: msg="Missing ENDCASE"; break;
        case 104: msg="OF missing from CASE statement"; break;
        case 105: msg="CASE..OF statement must be the last thing on a line"; 
                  break;
        case 106: msg="Missing ENDIF"; break;
        case 107: msg="Bad MOUSE variable"; break;
        case 108: msg="Too many input expressions for SYS"; break;
        case 109: msg="Too many output variables for SYS"; break;
        case 110: msg="Can't install library"; break;
        case 111: msg="Bad program used as function/procedure library"; break;
        case 112: msg="No room for library"; break;
        default: msg="Curious error?";
      }
      for (i=0; i<strlen(msg); i++)
        memory_writebyte(mem, reg->r[1]+i, msg[i]);
      memory_writebyte(mem, reg->r[1]+i, 13);

//      strcpy((char*)memory_lookup(mem, (uint5*)reg->r[1]), msg);

      FLAG_(v) = 1;
    }
    break;

    default:
    fprintf(stderr, "Unknown SWI %x called\n", num);
  }
}
