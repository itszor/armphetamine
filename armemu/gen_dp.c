#include <stdio.h>

#include "defs.h"
#include "machine.h"
#include "memory.h"
#include "registers.h"
#include "processor.h"
#include "execute.h"
#include "fakesys.h"
#include "decode.h"

// generate nice assembly language leaf functions to deal with all that
// data-processing flag crap
void gen_dp(FILE* stream)
{
  int op;
  const char* iname[] =
    {"and", "eor", "sub", "rsb", "add", "adc", "sbc", "rsc",
     "tst", "teq", "cmp", "cmn", "orr", "mov", "bic", "mvn"};
  const char* sname[] =
    {"lsl", "lsr", "asr", "ror"};
  const uint5 logic = 0xf303;
  
  fprintf(stream, ".text\n");
  
  for (op=0; op<16; op++)
  {
    int setflags, islogic = logic & (1<<op);
    for (setflags=0; setflags<2; setflags++)
    {
      int shifttype;
      for (shifttype=0; shifttype<4; shifttype++)
      {
        int regshift;
        for (regshift=0; regshift<2; regshift++)
        {
          int invc = 0, needtest = 0;
#define fnid iname[op], setflags?"s":"ns", sname[shifttype], \
             regshift?"reg":"imm"
#define ref(N) "%s_%s_%s_%s_" #N "\n\t", fnid
#define label(N) fprintf(stream, "%s_%s_%s_%s_" #N ":\n\t", fnid);
          fprintf(stream, ".p2align 4,,7\n\t");
          fprintf(stream, ".globl %s_%s_%s_%s\n\t", fnid);
          fprintf(stream, ".type %s_%s_%s_%s,@function\n", fnid);
          fprintf(stream, "%s_%s_%s_%s:\n\t", fnid);

// flags go:
//   bit 31: n
//   bit 30: z
//   bit 29: c
//   bit 28: v

          if (regshift)
          {
            switch (shifttype)
            {
              case 0:  // lsl
              fprintf(stream, "movzbl 12(%%esp),%%ecx\n\t");
              fprintf(stream, "movl 8(%%esp),%%edx\n\t");
              fprintf(stream, "cmp $31,%%cl\n\t");
              fprintf(stream, "jg "ref(1));
              fprintf(stream, "shll %%cl,%%edx\n\t");
              if (setflags && islogic)
              {
                fprintf(stream, "setc %%cl\n\t");
                fprintf(stream, "shll $29,%%ecx\n\t");
                fprintf(stream, "movl 16(%%esp),%%eax\n\t");
                fprintf(stream, "btr $29,(%%eax)\n\t");
                fprintf(stream, "orl %%ecx,(%%eax)\n\t");
              }
              fprintf(stream, "jmp "ref(2));
              label(1);
              fprintf(stream, "cmp $32,%%cl\n\t");
              fprintf(stream, "je "ref(3));
              // greater than 32-bit shift
              if (setflags && islogic)
              {
                fprintf(stream, "btr $29,(%%eax)\n\t"); // clear carry
              }
              fprintf(stream, "xorl %%edx,%%edx\n\t");  // clear edx
              fprintf(stream, "jmp "ref(2));
              label(3);
              if (setflags && islogic)
              {
                fprintf(stream, "bt $31,%%edx\n\t");
                fprintf(stream, "setc %%cl\n\t");
                fprintf(stream, "shll $29,%%ecx\n\t");
                fprintf(stream, "btr $29,(%%eax)\n\t");
                fprintf(stream, "orl %%ecx,(%%eax)\n\t");
              }
              fprintf(stream, "xorl %%edx,%%edx\n\t");
              label(2);
              break;
              
              case 1:  // lsr
              fprintf(stream, "movzbl 12(%%esp),%%ecx\n\t");
              fprintf(stream, "movl 8(%%esp),%%edx\n\t");
              fprintf(stream, "cmp $31,%%cl\n\t");
              fprintf(stream, "jg "ref(1));
              fprintf(stream, "shrl %%cl,%%edx\n\t");
              if (setflags && islogic)
              {
                fprintf(stream, "setc %%cl\n\t");
                fprintf(stream, "shll $29,%%ecx\n\t");
                fprintf(stream, "movl 16(%%esp),%%eax\n\t");
                fprintf(stream, "btr $29,(%%eax)\n\t");
                fprintf(stream, "orl %%ecx,(%%eax)\n\t");
              }
              fprintf(stream, "jmp "ref(2));
              label(1);
              fprintf(stream, "cmp $32,%%cl\n\t");
              fprintf(stream, "je "ref(3));
              // greater than 32-bit shift
              if (setflags && islogic)
              {
                fprintf(stream, "btr $29,(%%eax)\n\t"); // clear carry
              }
              fprintf(stream, "xorl %%edx,%%edx\n\t");  // clear edx
              fprintf(stream, "jmp "ref(2));
              label(3);
              if (setflags && islogic)
              {
                fprintf(stream, "bt $0,%%edx\n\t");
                fprintf(stream, "setc %%cl\n\t");
                fprintf(stream, "shll $29,%%ecx\n\t");
                fprintf(stream, "btr $29,(%%eax)\n\t");
                fprintf(stream, "orl %%ecx,(%%eax)\n\t");
              }
              fprintf(stream, "xorl %%edx,%%edx\n\t");
              label(2);
              break;
              
              case 2:  // asr
              fprintf(stream, "movzbl 12(%%esp),%%ecx\n\t");
              fprintf(stream, "movl 8(%%esp),%%edx\n\t");
              fprintf(stream, "cmp $31,%%cl\n\t");
              fprintf(stream, "jg "ref(1));
              fprintf(stream, "sarl %%cl,%%edx\n\t");
              if (setflags && islogic)
              {
                fprintf(stream, "setc %%cl\n\t");
                fprintf(stream, "shll $29,%%ecx\n\t");
                fprintf(stream, "movl 16(%%esp),%%eax\n\t");
                fprintf(stream, "btr $29,(%%eax)\n\t");
                fprintf(stream, "orl %%ecx,(%%eax)\n\t");
              }
              fprintf(stream, "jmp "ref(2));
              label(1);
              // greater than 31-bit shift
              fprintf(stream, "bt $31,%%edx\n\t");
              fprintf(stream, "xorl %%edx,%%edx\n\t");  // clear edx
              fprintf(stream, "sbbl $0,%%edx\n\t");
              if (setflags && islogic)
              {
                fprintf(stream, "movl %%edx,%%eax\n\t");
                fprintf(stream, "andl $0x20000000,%%eax\n\t");
                fprintf(stream, "btr $29,(%%eax)\n\t");
                fprintf(stream, "orl %%eax,(%%eax)\n\t");
              }
              label(2);
              break;
              
              case 3:  // ror
              fprintf(stream, "movzbl 12(%%esp),%%ecx\n\t");
              fprintf(stream, "movl 8(%%esp),%%edx\n\t");
              // register-valued ROR of zero doesn't seem to affect C
              if (setflags && islogic)
              {
                fprintf(stream, "cmp $0,%%cl\n\t");
                fprintf(stream, "je "ref(1));
              }
              fprintf(stream, "rorl %%cl,%%edx\n\t");
              if (setflags && islogic)
              {
                fprintf(stream, "setc %%cl\n\t");
                fprintf(stream, "shll $29,%%ecx\n\t");
                fprintf(stream, "movl 16(%%esp),%%eax\n\t");
                fprintf(stream, "btr $29,(%%eax)\n\t");
                fprintf(stream, "orl %%ecx,(%%eax)\n\t");
                label(1);
              }
              break;
            }
          }
          else  // immediate shift
          {
            switch (shifttype)
            {
              case 0:  // lsl
              fprintf(stream, "movl 12(%%esp),%%ecx\n\t");
              fprintf(stream, "movl 8(%%esp),%%edx\n\t");
              fprintf(stream, "shll %%cl,%%edx\n\t");
              if (setflags && islogic)
              {
                fprintf(stream, "setc %%cl\n\t");
                fprintf(stream, "shll $29,%%ecx\n\t");
                fprintf(stream, "movl 16(%%esp),%%eax\n\t");
                fprintf(stream, "btr $29,(%%eax)\n\t");
                fprintf(stream, "orl %%ecx,(%%eax)\n\t");
              }
              break;
              
              case 1:  // lsr
              if (setflags && islogic)
              {
                fprintf(stream, "movl 12(%%esp),%%ecx\n\t");
                fprintf(stream, "cmpl $0,%%ecx\n\t");
                fprintf(stream, "je "ref(1));
                fprintf(stream, "movl 8(%%esp),%%edx\n\t");
                fprintf(stream, "shrl %%cl,%%edx\n\t");
                fprintf(stream, "setc %%cl\n\t");
                fprintf(stream, "shll $29,%%ecx\n\t");
                fprintf(stream, "movl 16(%%esp),%%eax\n\t");
                fprintf(stream, "btr $29,(%%eax)\n\t");
                fprintf(stream, "orl %%ecx,(%%eax)\n\t");
                fprintf(stream, "jmp "ref(2));
                label(1);
                fprintf(stream, "bt $31,%%edx\n\t");
                fprintf(stream, "setc %%cl\n\t");
                fprintf(stream, "shll $29,%%ecx\n\t");
                fprintf(stream, "btr $29,(%%eax)\n\t");
                fprintf(stream, "orl %%ecx,(%%eax)\n\t");
                label(2);
              }
              else
              {
                fprintf(stream, "movl 12(%%esp),%%ecx\n\t");
                fprintf(stream, "cmpl $0,%%ecx\n\t");
                fprintf(stream, "je "ref(1));
                fprintf(stream, "movl 8(%%esp),%%edx\n\t");
                fprintf(stream, "shrl %%cl,%%edx\n\t");
                fprintf(stream, "jmp "ref(2));
                label(1);
                fprintf(stream, "movl 4(%%esp),%%eax\n\t");
                fprintf(stream, "ret\n");
                label(2);
              }
              break;
              
              case 2:  // asr
              if (setflags && islogic)
              {
                fprintf(stream, "movl 12(%%esp),%%ecx\n\t");
                fprintf(stream, "cmpl $0,%%ecx\n\t");
                fprintf(stream, "je "ref(1));
                fprintf(stream, "movl 8(%%esp),%%edx\n\t");
                fprintf(stream, "sarl %%cl,%%edx\n\t");
                fprintf(stream, "setc %%cl\n\t");
                fprintf(stream, "shll $29,%%ecx\n\t");
                fprintf(stream, "movl 16(%%esp),%%eax\n\t");
                fprintf(stream, "btr $29,(%%eax)\n\t");
                fprintf(stream, "orl %%ecx,(%%eax)\n\t");
                fprintf(stream, "jmp "ref(2));
                label(1);
                fprintf(stream, "bt $31,%%edx\n\t");
                fprintf(stream, "setc %%cl\n\t");
                fprintf(stream, "sarl $31,%%edx\n\t");
                fprintf(stream, "shll $29,%%ecx\n\t");
                fprintf(stream, "btr $29,(%%eax)\n\t");
                fprintf(stream, "orl %%ecx,(%%eax)\n\t");
                label(2);
              }
              else
              {
                fprintf(stream, "movl 12(%%esp),%%ecx\n\t");
                fprintf(stream, "cmpl $0,%%ecx\n\t");
                fprintf(stream, "je "ref(1));
                fprintf(stream, "movl 8(%%esp),%%edx\n\t");
                fprintf(stream, "sarl %%cl,%%edx\n\t");
                fprintf(stream, "jmp "ref(2));
                label(1);
                fprintf(stream, "movl 4(%%esp),%%eax\n\t");
                fprintf(stream, "ret\n");
                label(2);
              }
              break;
              
              case 3:  // ror
              fprintf(stream, "movl 12(%%esp),%%ecx\n\t");
              fprintf(stream, "movl 8(%%esp),%%edx\n\t");
              fprintf(stream, "movl 16(%%esp),%%eax\n\t");
              fprintf(stream, "cmpl $0,%%ecx\n\t");
              fprintf(stream, "je "ref(1));
              fprintf(stream, "rorl %%cl,%%edx\n\t");
              fprintf(stream, "jmp "ref(2));
              label(1);
              fprintf(stream, "bt $29,(%%eax)\n\t");
              fprintf(stream, "rcrl $1,%%edx\n");
              label(2);
              // carry valid for either ROR or RRX operation
              if (setflags && islogic)
              {
                fprintf(stream, "setc %%cl\n\t");
                fprintf(stream, "shll $29,%%ecx\n\t");
                fprintf(stream, "btr $29,(%%eax)\n\t");
                fprintf(stream, "orl %%ecx,(%%eax)\n\t");
              }
              break;
            }
          }

          fprintf(stream, "movl 4(%%esp),%%eax\n\t");

          switch (op)
          {
            case dp_AND:
            fprintf(stream, "andl %%edx,%%eax\n\t");
            break;
            
            case dp_EOR:
            fprintf(stream, "xorl %%edx,%%eax\n\t");
            break;
            
            case dp_SUB:
            fprintf(stream, "subl %%edx,%%eax\n\t");
            invc = 1;
            break;
            
            case dp_RSB:
            fprintf(stream, "xchgl %%edx,%%eax\n\t");
            fprintf(stream, "subl %%edx,%%eax\n\t");
            invc = 1;
            break;
            
            case dp_ADD:
            fprintf(stream, "addl %%edx,%%eax\n\t");
            break;
            
            case dp_ADC:
            fprintf(stream, "movl 16(%%esp),%%ecx\n\t");
            fprintf(stream, "bt $29,(%%ecx)\n\t");
            fprintf(stream, "adcl %%edx,%%eax\n\t");
            break;
            
            case dp_SBC:
            fprintf(stream, "movl 16(%%esp),%%ecx\n\t");
            fprintf(stream, "bt $29,(%%ecx)\n\t");
            fprintf(stream, "cmc\n\t");
            fprintf(stream, "sbbl %%edx,%%eax\n\t");
            invc = 1;
            break;
            
            case dp_RSC:
            fprintf(stream, "movl 16(%%esp),%%ecx\n\t");
            fprintf(stream, "xchgl %%edx,%%eax\n\t");
            fprintf(stream, "bt $29,(%%ecx)\n\t");
            fprintf(stream, "cmc\n\t");
            fprintf(stream, "sbbl %%edx,%%eax\n\t");
            invc = 1;
            break;
            
            case dp_TST:
            fprintf(stream, "testl %%edx,%%eax\n\t");
            break;
            
            case dp_TEQ:
            // leaving eax intact!
            fprintf(stream, "movl %%eax,%%ecx\n\t");
            fprintf(stream, "xorl %%edx,%%ecx\n\t");
            break;
            
            case dp_CMP:
            fprintf(stream, "cmpl %%edx,%%eax\n\t");
            invc = 1;
            break;
            
            case dp_CMN:
            fprintf(stream, "movl %%eax,%%ecx\n\t");
            fprintf(stream, "addl %%edx,%%ecx\n\t");
            break;
            
            case dp_ORR:
            fprintf(stream, "orl %%edx,%%eax\n\t");
            break;
            
            case dp_MOV:
            fprintf(stream, "movl %%edx,%%eax\n\t");
            needtest = 1;
            break;
            
            case dp_BIC:
            fprintf(stream, "notl %%edx\n\t");
            fprintf(stream, "andl %%edx,%%eax\n\t");
            break;
            
            case dp_MVN:
            fprintf(stream, "notl %%edx\n\t");
            fprintf(stream, "movl %%edx,%%eax\n\t");
            needtest = 1;
            break;
          }
          if (setflags)
          {
            if (islogic)
            {
              if (needtest)
              {
                fprintf(stream, "testl %%eax,%%eax\n\t");
              }
              fprintf(stream, "sete %%dl\n\t");
              fprintf(stream, "sets %%dh\n\t");
              fprintf(stream, "shl $1,%%dh\n\t");
              fprintf(stream, "or %%dh,%%dl\n\t");
              fprintf(stream, "shl $30,%%edx\n\t");
              fprintf(stream, "movl 16(%%esp),%%ecx\n\t");
              fprintf(stream, "andl $0x3fffffff,(%%ecx)\n\t");
              fprintf(stream, "orl %%edx,(%%ecx)\n\t");
            }
            else  // ALU instruction; all flags set
            {
              fprintf(stream, "seto %%dl\n\t");     // V
              if (invc)
                fprintf(stream, "setnc %%dh\n\t");  // !C
              else
                fprintf(stream, "setc %%dh\n\t");   // C
              fprintf(stream, "sete %%cl\n\t");     // Z
              fprintf(stream, "sets %%ch\n\t");     // N
              fprintf(stream, "shlb $1,%%dh\n\t");
              fprintf(stream, "shlb $2,%%cl\n\t");
              fprintf(stream, "shlb $3,%%ch\n\t");
              fprintf(stream, "orb %%dh,%%dl\n\t");
              fprintf(stream, "orb %%cl,%%dl\n\t");
              fprintf(stream, "orb %%ch,%%dl\n\t");
              fprintf(stream, "shll $28,%%edx\n\t");
              fprintf(stream, "movl 16(%%esp),%%ecx\n\t");
              fprintf(stream, "andl $0x0fffffff,(%%ecx)\n\t");
              fprintf(stream, "orl %%edx,(%%ecx)\n\t");
            }
          }
          fprintf(stream, "ret\n\n");
        }
      }
    }
  }
}

int main(void)
{
  gen_dp(stdout);

  return 0;
}
