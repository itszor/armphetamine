#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "defs.h"
#include "pheta.h"
#include "phetadism.h"

static const char* opname[] = { "const", "constb", "fetch", "commit", "fexpect",
  "fcommit", "fensure", "setpred", "nfexpect", "nfcommit", "nfensure",
  "nsetpred", "fwrite", "xjmp", "lsl", "lsr", "asr", "ror", "rol", "rrx",
  "rlx", "mov", "not", "and", "or", "eor", "teq", "tst", "add", "adc", "sub",
  "sbc", "cmp", "cmn", "mul", "ldw", "ldb", "stw", "stb", "swi", "undef",
  "sync", "end" };

static const char* txtcc[]={"eq","ne","cs","cc","mi","pl","vs","vc",
														"hi","ls","ge","lt","gt","le","al","nv"};


void phetadism_chunk(pheta_chunk* chunk)
{
  list* walk = chunk->blocks;

  for (; walk; walk = walk->prev)
    phetadism_block((pheta_basicblock*) walk->data);
}

void phetadism_block(pheta_basicblock* blk)
{
  uint5 i;
  
  printf("-- block base: %x  (source start %x)\n", blk, blk->srcstart);
  
  for (i=0; i<blk->length; i++)
  {
    uint5 opcode = blk->base[i++];
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

      case ph_FEXPECT:
      case ph_FCOMMIT:
      case ph_FENSURE:
      case ph_NFEXPECT:
      case ph_NFCOMMIT:
      case ph_NFENSURE:
      {
        uint5 mask = blk->base[i++], first=1;
        char flags[10];
        
        flags[0] = 0;
        
        if (mask & ph_C) strcat(flags, (first=0, "c"));
        if (mask & ph_V) strcat(flags, first ? (first=0, "v") : ",v");
        if (mask & ph_N) strcat(flags, first ? (first=0, "n") : ",n");
        if (mask & ph_Z) strcat(flags, first ? (first=0, "z") : ",z");
        
        printf("%-10s%s\n", opname[opcode], flags);
      }
      break;

      case ph_SETPRED:
      case ph_NSETPRED:
      {
        uint5 pno = blk->base[i++];
        uint5 ptype = blk->base[i++];
        printf("%-10sp%d, %s\n", opname[opcode], pno, txtcc[ptype]);
      }
      break;

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

      case ph_SWI:
      case ph_SYNC:
      case ph_END:
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
  printf("-- block predicate: p%d%s  true block: %x  false block: %x\n\n",
         blk->predicate,
         blk->predicate==0 ? " (al)" :
         blk->predicate==255 ? " (unset)" : "",
         blk->trueblk, blk->falseblk);
}
