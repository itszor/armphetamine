#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "defs.h"
#include "pheta.h"
#include "phetadism.h"
#include "list.h"

const char* opname[] = { "const", "constb", "fetch", "commit", "assoc", 
  "spill", "reload", "phi", "fexpect", "fcommit", "fensure", "nfexpect", 
  "nfcommit", "nfensure", "fwrite", "lsl", "lsr", "asr", "ror", "rol", "rrx", 
  "rlx", "mov", "not", "and", "or", "eor", "teq", "tst", "add", "adc", "sub", 
  "sbc", "cmp", "cmn", "mul", "ldw", "ldb", "stw", "stb", "swi", "undef", 
  "state", "sync", "xjmp", "ukjmp", "cajmp", "cajmp26f", "rts", "address" };

const char* txtcc[]={"eq","ne","cs","cc","mi","pl","vs","vc",
                     "hi","ls","ge","lt","gt","le","al","nv"};

static const char* txtflag[] = {"z", "c", "n", "v", "i"};

const char* armreg[] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
  "r8", "r9", "r10", "r11", "r12", "sp", "lr", "pc_full", "pc_addr", "pc_flag",
  "cpsr_flag", "cpsr_all", "spsr", "clock" };

void phetadism_chunk(pheta_chunk* chunk)
{
  jt_list* walk = chunk->blocks;

  for (; walk; walk = walk->prev)
    phetadism_block((pheta_basicblock*) walk->data, 0);
}

static void writeflags(FILE* f, uint5 flags)
{
  uint5 j;
  
  for (j=0; j<5; j++)
    if (flags & (1<<j))
      fprintf(f, "%s", txtflag[j]);
}

void phetadism_instruction(FILE* f, pheta_instr* instr)
{
  uint5 opcode = instr->opcode;

  switch (opcode)
  {
    case ph_CONST:
    {
      uint5 dest = instr->data.con.dest;
      uint5 word = instr->data.con.imm;
      fprintf(f, "const     %%%d, #0x%.8x", dest, word);
    }
    break;

    case ph_CONSTB:
    {
      uint5 dest = instr->data.con.dest;
      uint5 byte = instr->data.con.imm;
      fprintf(f, "constb    %%%d, #0x%.2x", dest, byte);
    }
    break;

    case ph_FETCH:
    {
      uint5 dest = instr->data.op.dest;
      uint5 reg = instr->data.op.src1;
      fprintf(f, "fetch     %%%d, %s", dest, armreg[reg]);
    }
    break;

    case ph_COMMIT:
    case ph_ASSOC:
    {
      uint5 reg = instr->data.op.dest;
      uint5 src = instr->data.op.src1;
      fprintf(f, "%-10s%s, %%%d", opname[opcode], armreg[reg], src);
    }
    break;

    case ph_SPILL:
    case ph_UKJMP:
    {
      uint5 reg = instr->data.op.src1;
      fprintf(f, "%-10s%%%d", opname[opcode], reg);
    }
    break;

    case ph_RELOAD:
    {
      uint5 reg = instr->data.op.dest;
      fprintf(f, "%-10s%%%d", opname[opcode], reg);
    }
    break;

    case ph_FEXPECT:
    case ph_FENSURE:
    case ph_NFEXPECT:
    case ph_NFENSURE:
    {
      uint5 mask = instr->data.flag.need;

      fprintf(f, "%-10s[", opname[opcode]);
      writeflags(f, mask);
      fprintf(f, "]");

/*        if (mask & ph_C) strcat(flags, (first=0, "c"));
      if (mask & ph_V) strcat(flags, first ? (first=0, "v") : ",v");
      if (mask & ph_N) strcat(flags, first ? (first=0, "n") : ",n");
      if (mask & ph_Z) strcat(flags, first ? (first=0, "z") : ",z");*/

/*        fprintf(f, "%-10s%s\n", opname[opcode], flags);*/
    }
    break;

    case ph_FCOMMIT:
    case ph_NFCOMMIT:
    {
      uint5 have = instr->data.flag.have, first=1, j;
      uint5 need = instr->data.flag.need;
      uint5 pred = instr->data.flag.pred;

      fprintf(f, "%-10s[", opname[opcode]);
      writeflags(f, have);
      fprintf(f, "], [");
      writeflags(f, need);
      fprintf(f, "], [");
      for (j=0; j<16; j++)
      {
        if (pred & (1<<j))
        {
          fprintf(f, first ? "%s" : " %s", txtcc[j]);
          first = 0;
        }
      }
      fprintf(f, "]");
    }
    break;

/*      case ph_SETPRED:
    case ph_NSETPRED:
    {
      uint5 pno = blk->base[i++];
      uint5 ptype = blk->base[i++];
      fprintf(f, "%-10sp%d, %s\n", opname[opcode], pno, txtcc[ptype]);
    }
    break;*/

    case ph_FWRITE:
    {
      uint5 mask = instr->data.op.dest, first = 1;
      uint5 src = instr->data.op.src1;
      char flags[10];

      flags[0] = 0;

      if (mask & ph_C) strcat(flags, (first=0, "c"));
      if (mask & ph_V) strcat(flags, first ? "v" : (first=0, ",v"));
      if (mask & ph_N) strcat(flags, first ? "n" : (first=0, ",n"));
      if (mask & ph_Z) strcat(flags, first ? "z" : (first=0, ",z"));

      fprintf(f, "%-10s%s, %%%d", opname[opcode], flags, src);
    }
    break;

    case ph_XJMP:
    case ph_ADDRESS:
    {
      uint5 word = instr->data.imm;
      fprintf(f, "%-10s#0x%.8x", opname[opcode], word);
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
      uint5 dest = instr->data.op.dest;
      uint5 op1 = instr->data.op.src1;
      uint5 op2 = instr->data.op.src2;
      fprintf(f, "%-10s%%%d, %%%d, %%%d", opname[opcode], dest, op1, op2);
    }
    break;

    case ph_RRX:
    case ph_RLX:
    case ph_MOV:
    case ph_NOT:
    case ph_CAJMP:
    case ph_CAJMP26F:
    {
      uint5 op1 = instr->data.op.dest;
      uint5 op2 = instr->data.op.src1;
      fprintf(f, "%-10s%%%d, %%%d", opname[opcode], op1, op2);
    }
    break;

    case ph_TEQ:
    case ph_TST:
    case ph_CMP:
    case ph_CMN:
    {
      uint5 op1 = instr->data.op.src1;
      uint5 op2 = instr->data.op.src2;
      fprintf(f, "%-10s%%%d, %%%d", opname[opcode], op1, op2);
    }
    break;

    case ph_LDW:
    case ph_LDB:
    {
      uint5 op1 = instr->data.op.dest;
      uint5 op2 = instr->data.op.src1;
      fprintf(f, "%-10s%%%d, [%%%d]", opname[opcode], op1, op2);
    }
    break;

    case ph_STW:
    case ph_STB:
    {
      uint5 op1 = instr->data.op.src1;
      uint5 op2 = instr->data.op.src2;
      fprintf(f, "%-10s[%%%d], %%%d", opname[opcode], op1, op2);
    }
    break;

    case ph_STATE:
    {
      jt_list* scan;
      uint5 first = 1;

      fprintf(f, "%-10s(", opname[opcode]);

      for (scan=(jt_list*)instr->data.ptr; scan; scan=scan->prev)
      {
        pheta_rpair* rpair = scan->data;
        printf(first ? "%%%d" : " %%%d", rpair->ph);
        first = 0;
      }
      fprintf(f, ")");
    }
    break;

    case ph_PHI:
    {
      fprintf(f, "%-10s(format undecided)", opname[opcode]);
    }
    break;

    case ph_SWI:
    case ph_UNDEF:
    case ph_SYNC:
    case ph_RTS:
    {
      fprintf(f, "%-10s", opname[opcode]);
    }
    break;

    default:
    {
      fprintf(stderr, "Undefined opcode %d", opcode);
      exit(1);
    }
    break;
  }
}

void phetadism_block(pheta_basicblock* blk, uint5 startline)
{
  jt_clist* walk;
  uint5 i;
    
  printf("Block base: %p  (source start %x)\n", blk, blk->srcstart);
  printf("Time (discover): %d  time (finish): %d\n", blk->discovertime,
    blk->finishtime);
  
  for (walk=blk->base->next, i=0; walk->data; walk=walk->next, i++)
  {
    pheta_instr* instr = walk->data;
    printf("%4d: ", startline+i);
    phetadism_instruction(stdout, instr);
    printf("\n");
  }
  printf("Block predicate: %s\n  True block: %p\n  False block: %p\n"
         "  Parent block: %p\n",
         txtcc[blk->predicate], blk->trueblk, blk->falseblk, blk->parent);
  printf("Predecessors' finish times: ");
  for (walk=blk->predecessor->next; walk->data; walk=walk->next)
  {
    pheta_basicblock* foo = walk->data;
    printf("%d ", foo->finishtime);
  }
  printf("\nStrongly-connected subgraph: %p\n\n", blk->scsubgraph);
}
