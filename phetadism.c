#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "defs.h"
#include "pheta.h"
#include "phetadism.h"
#include "list.h"

static const char* opname[] = { "const", "constb", "fetch", "commit", "spill",
  "reload", "fexpect", "fcommit", "fensure", "nfexpect", "nfcommit", "nfensure",
  "fwrite", "lsl", "lsr", "asr", "ror", "rol", "rrx", "rlx", "mov", "not",
  "and", "or", "eor", "teq", "tst", "add", "adc", "sub", "sbc", "cmp", "cmn",
  "mul", "ldw", "ldb", "stw", "stb", "swi", "undef", "state", "sync", "xjmp", 
  "ukjmp", "cajmp", "rts" };

static const char* txtcc[]={"eq","ne","cs","cc","mi","pl","vs","vc",
                            "hi","ls","ge","lt","gt","le","al","nv"};

static const char* txtflag[] = {"z", "c", "n", "v", "i"};

void phetadism_chunk(pheta_chunk* chunk)
{
  list* walk = chunk->blocks;

  for (; walk; walk = walk->prev)
    phetadism_block((pheta_basicblock*) walk->data);
}

static void writeflags(uint5 flags)
{
  uint5 j;
  
  for (j=0; j<5; j++)
    if (flags & (1<<j))
      printf("%s", txtflag[j]);
}

void phetadism_block(pheta_basicblock* blk)
{
  uint5 i;
  
  printf("Block base: %x  (source start %x)\n", blk, blk->srcstart);
  
  for (i=0; i<blk->length; i++)
  {
    uint5 opcode = blk->base[i++];
    printf("%4d: ", i-1);
    switch (opcode)
    {
      case ph_CONST:
      {
        uint5 dest = blk->base[i++];
        uint5 word = blk->base[i++];
        word |= blk->base[i++]<<8;
        word |= blk->base[i++]<<16;
        word |= blk->base[i++]<<24;
        printf("const     %%%d, #0x%.8x\n", dest, word);
      }
      break;

      case ph_CONSTB:
      {
        uint5 dest = blk->base[i++];
        uint5 byte = blk->base[i++];
        printf("constb    %%%d, #0x%.2x\n", dest, byte);
      }
      break;

      case ph_FETCH:
      {
        uint5 dest = blk->base[i++];
        uint5 reg = blk->base[i++];
        printf("fetch     %%%d, r%d\n", dest, reg);
      }
      break;

      case ph_COMMIT:
      {
        uint5 reg = blk->base[i++];
        uint5 src = blk->base[i++];
        printf("commit    r%d, %%%d\n", reg, src);
      }
      break;

      case ph_SPILL:
      case ph_RELOAD:
      {
        uint5 reg = blk->base[i++];
        printf("%-10s%%%d\n", opname[opcode], reg);
      }
      break;

      case ph_FEXPECT:
      case ph_FENSURE:
      case ph_NFEXPECT:
      case ph_NFENSURE:
      {
        uint5 mask = blk->base[i++], first=1;
        uint5 j;
        
        printf("%-10s[", opname[opcode]);
        writeflags(mask);
        printf("]\n");
        
/*        if (mask & ph_C) strcat(flags, (first=0, "c"));
        if (mask & ph_V) strcat(flags, first ? (first=0, "v") : ",v");
        if (mask & ph_N) strcat(flags, first ? (first=0, "n") : ",n");
        if (mask & ph_Z) strcat(flags, first ? (first=0, "z") : ",z");*/
        
/*        printf("%-10s%s\n", opname[opcode], flags);*/
      }
      break;

      case ph_FCOMMIT:
      case ph_NFCOMMIT:
      {
        uint5 have = blk->base[i++], first=1, j;
        uint5 need = blk->base[i++];
        uint5 pred = blk->base[i++];
        pred |= blk->base[i++]<<8;
        
        printf("%-10s[", opname[opcode]);
        writeflags(have);
        printf("], [");
        writeflags(need);
        printf("], [");
        for (j=0; j<16; j++)
        {
          if (pred & (1<<j))
          {
            printf(first ? "%s" : " %s", txtcc[j]);
            first = 0;
          }
        }
        printf("]\n");
      }
      break;

/*      case ph_SETPRED:
      case ph_NSETPRED:
      {
        uint5 pno = blk->base[i++];
        uint5 ptype = blk->base[i++];
        printf("%-10sp%d, %s\n", opname[opcode], pno, txtcc[ptype]);
      }
      break;*/

      case ph_FWRITE:
      {
        uint5 mask = blk->base[i++], first = 1;
        uint5 src = blk->base[i++];
        char flags[10];
        
        flags[0] = 0;
        
        if (mask & ph_C) strcat(flags, (first=0, "c"));
        if (mask & ph_V) strcat(flags, first ? "v" : (first=0, ",v"));
        if (mask & ph_N) strcat(flags, first ? "n" : (first=0, ",n"));
        if (mask & ph_Z) strcat(flags, first ? "z" : (first=0, ",z"));
        
        printf("%-10s%s, %%%d\n", opname[opcode], flags, src);
      }
      break;

      case ph_XJMP:
      {
        uint5 word = blk->base[i++];
        word |= blk->base[i++]<<8;
        word |= blk->base[i++]<<16;
        word |= blk->base[i++]<<24;
        printf("xjmp      #%.8x\n", word);
      }
      break;

      case ph_LSL:
      case ph_LSR:
      case ph_ASR:
      case ph_ROR:
      case ph_ROL:
      case ph_AND:
      case ph_OR:
      case ph_EOR:
      case ph_ADD:
      case ph_ADC:
      case ph_SUB:
      case ph_SBC:
      case ph_MUL:
      {
        uint5 dest = blk->base[i++];
        uint5 op1 = blk->base[i++];
        uint5 op2 = blk->base[i++];
        printf("%-10s%%%d, %%%d, %%%d\n", opname[opcode], dest, op1, op2);
      }
      break;

      case ph_RRX:
      case ph_RLX:
      case ph_MOV:
      case ph_NOT:
      case ph_LDW:
      case ph_LDB:
      case ph_TEQ:
      case ph_TST:
      case ph_CMP:
      case ph_CMN:
      case ph_STW:
      case ph_STB:
      {
        uint5 op1 = blk->base[i++];
        uint5 op2 = blk->base[i++];
        printf("%-10s%%%d, %%%d\n", opname[opcode], op1, op2);
      }
      break;

      case ph_STATE:
      {
        list* scan;
        uint5 start = blk->base[i++], first = 1;
        start |= blk->base[i++]<<8;
        start |= blk->base[i++]<<16;
        start |= blk->base[i++]<<24;
        
        printf("%-10s[", opname[opcode]);
        
        for (scan=(list*)start; scan; scan=scan->prev)
        {
          pheta_rpair* rpair = scan->data;
          printf(first ? "r%d:%%%d" : " r%d:%%%d", rpair->ph, rpair->arm);
          first = 0;
        }
        printf("]\n");
      }
      break;

      case ph_SWI:
      case ph_UNDEF:
      case ph_SYNC:
      case ph_UKJMP:
      case ph_CAJMP:
      case ph_RTS:
      {
        printf("%-10s\n", opname[opcode]);
      }
      break;

      default:
      {
        fprintf(stderr, "Undefined opcode %d\n", opcode);
        exit(1);
      }
      break;
    }
  }
  printf("Block predicate: %s\n  True block: %x\n  False block: %x\n"
         "  Parent block: %x\n\n",
         txtcc[blk->predicate], blk->trueblk, blk->falseblk, blk->parent);
}
