#include <stdio.h>

#include "defs.h"
#include "machine.h"
#include "pseudo.h"
#include "pseudodism.h"

const char* regname[] =
{
  "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
	"r8", "r9", "r10", "r11", "r12", "r13", "r14", "pc",
	"t0", "t1", "t2", "t3", "t4", "t5", "cpsr-all", "spsr-current",
  "cpsr-flags", "cpsr-c", "cpsr-v", "cpsr-z", "cpsr-n", "cpsr-mode",
  "cpsr-intmask", "unused"
};

const char* opname[] =
{
 /* "subc", "addc", "sbcc", "adcc", "subv", "addv", "sbcv", "adcv",
	"eqz ", "neg ",
	"lslc", "lsrc", "asrc", "rorc", "rrxc ",*/
	"beq", "bne", "bcs", "bcc", "bmi", "bpl", "bvs", "bvc",
	"bhi", "bls", "bge", "blt", "bgt", "ble", "bal",
	"<<", ">>", ">>>", "ror", "rrx ", "&", "^", "-", "- !carry -", "+",
	"+ carry +", "|", "", "~", "cmp", "cmn", "teq", "tst",
  "*", "#", "ldw", "ldb", "stw", "stb", "swi",
	"xjmp", "pc-set", "pc-reconstruct", "end"
};

const char* iname[] =
{
  "eax", "ebx", "ecx", "edx", "esi", "edi"
};

void pseudodism_block(psblock* psb)
{
  sint5 i;
  imapinfo* imap = psb->info;
	
	fprintf(stderr, "Code start=%x, length=%x\n", psb->base, psb->length);
	
	for (i=0; i<psb->length; i++)
	{
	  pseudoformat inst;
    uint5 j=i, k;
		inst.value = psb->base[i];
		
		fprintf(stderr, "%x: ", i);
		
	  switch (inst.parts.opcode)
		{
/*	  	case op_Z:     // unops
			case op_N:
			case op_RRXC:*/
			case op_RRX:
			case op_MOV:
			case op_MVN:
			fprintf(stderr, "%s <- %s%s", regname[inst.parts.rd],
			  opname[inst.parts.opcode], regname[inst.parts.rm]);
			break;

			case op_LDW:
			fprintf(stderr, "%s <- !%s", regname[inst.parts.rd], regname[inst.parts.rm]);
			break;
			
			case op_LDB:
			fprintf(stderr, "%s <- ?%s", regname[inst.parts.rd], regname[inst.parts.rm]);
			break;
			
			case op_STW:
			fprintf(stderr, "!%s <- %s", regname[inst.parts.rm], regname[inst.parts.rn]);
			break;
			
			case op_STB:
			fprintf(stderr, "?%s <- %s", regname[inst.parts.rm], regname[inst.parts.rn]);
			break;

			case op_BEQ:
			case op_BNE:
			case op_BCS:
			case op_BCC:
			case op_BMI:
			case op_BPL:
			case op_BVS:
      case op_BVC:
			case op_BHI:
			case op_BLS:
			case op_BGE:
			case op_BLT:
			case op_BGT:
			case op_BLE:
			case op_BAL:
			case op_SWI:    // immediate follows
			case op_XJMP:
      fprintf(stderr, "%s 0x%x", opname[inst.parts.opcode], psb->base[++i]);
			break;

			case op_CONST:
      fprintf(stderr, "%s <- #0x%x", regname[inst.parts.rd], psb->base[++i]);
			break;
			
			case op_SETPC:
			case op_END:
			fprintf(stderr, "%s", opname[inst.parts.opcode]);
			break;

      case op_RECONSTRUCTPC:
			fprintf(stderr, "pc <- %s(#0x%x,...)",
              opname[inst.parts.opcode], psb->base[++i]);
      break;

      default:
			fprintf(stderr, "%s <- %s %s %s", regname[inst.parts.rd], regname[inst.parts.rm],
			  opname[inst.parts.opcode], regname[inst.parts.rn]);
		}

    if (imap)
    {
      if (imap[j].special.flag.leader)
        fprintf(stderr, " <leader>");
      if (imap[j].special.flag.allocated)
        fprintf(stderr, " <allocated>");
		  if (inst.parts.flags)
  		  fprintf(stderr, " [%s%s%s%s]\n", (inst.parts.flags&CFLAG)?"c":"",
			                          (inst.parts.flags&VFLAG)?"v":"",
															  (inst.parts.flags&ZFLAG)?"z":"",
															  (inst.parts.flags&NFLAG)?"n":"");
		  else
		    fprintf(stderr, "\n");

      for (k=0; k<imap[j].mstate.num; k++)
      {
        fprintf(stderr, "   map %d: ", k);
        switch (imap[j].mstate.regset[k].parts.status)
        {
          case alloc_UNSET:
          fprintf(stderr, "unset");
          break;

          case alloc_VALID:
          fprintf(stderr, "valid (%s,%s)",
            iname[imap[j].mstate.regset[k].parts.mapnum], 
            regname[imap[j].mstate.regset[k].parts.original]);
          break;

          case alloc_INVALID:
          fprintf(stderr, "valid (%s,%s)",
            iname[imap[j].mstate.regset[k].parts.mapnum], 
            regname[imap[j].mstate.regset[k].parts.original]);
          break;

          case alloc_MEMORY:
          fprintf(stderr, "memory (%s)",
            regname[imap[j].mstate.regset[k].parts.original]);
          break;

          case alloc_HIDDEN:
          fprintf(stderr, "hidden (%s)",
            regname[imap[j].mstate.regset[k].parts.original]);
          break;
        }
      }
    }
    fprintf(stderr, "\n");
	}
}
