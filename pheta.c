#include <stdarg.h>
#include <assert.h>
#include <stdio.h>

#include "defs.h"
#include "pheta.h"
#include "cnew.h"
#include "machine.h"
#include "list.h"
#include "decode.h"
#include "pqueue.h"

// this should be moved somewhere sensible
extern uint5 setbits(uint5);

const uint3 pheta_instlength[] = {
  6,  /* const */
  3,  /* constb */
  3,  /* fetch */
  3,  /* commit */
  2,  /* fexpect */
  2,  /* fcommit */
  2,  /* fensure */
  3,  /* setpred */
  2,  /* nfexpect */
  2,  /* nfcommit */
  2,  /* nfensure */
  3,  /* nsetpred */
  3,  /* fwrite */
  5,  /* xjmp */
  4,  /* lsl */
  4,  /* lsr */
  4,  /* asr */
  4,  /* ror */
  4,  /* rol */
  3,  /* rrx */
  3,  /* rlx */
  3,  /* mov */
  3,  /* not */
  4,  /* and */
  4,  /* or */
  4,  /* eor */
  3,  /* teq */
  3,  /* tst */
  4,  /* add */
  4,  /* adc */
  4,  /* sub */
  4,  /* sbc */
  3,  /* cmp */
  3,  /* cmn */
  4,  /* mul */
  3,  /* ldw */
  3,  /* ldb */
  3,  /* stw */
  3,  /* stb */
  1,  /* swi */
  1,  /* undef */
  1,  /* sync */
  1   /* end */
};

pheta_chunk* pheta_newchunk(uint5 start, uint5 length)
{
  pheta_chunk* p = cnew(pheta_chunk);
  p->blocks = 0;
  p->start = start;
  p->length = length;
  p->tempno = 0;
  p->predno = 1;
  p->force = -1;
  p->currentblock = 0;
    
  return p;
}

// Generate a new basic block, also make it current
pheta_basicblock* pheta_newbasicblock(pheta_chunk* c, uint5 startaddr)
{
  int i;
  pheta_basicblock* b;

  if (c->currentblock) pheta_lsync(c);

  list_add(&c->blocks);
  c->blocks->data = b = cnew(pheta_basicblock);
  b->base = calloc(sizeof(char), b->size = 8);
  b->length = 0;
  b->predicate = 255;
  b->trueblk = 0;
  b->falseblk = 0;
  b->parent = 0;
  b->srcstart = startaddr;
  b->live = pqueue_new();
  
  c->currentblock = b;

  for (i=0; i<ph_NUMREG; i++)
  {
    b->lbuf[i] = -1;
    b->dirtybuf[i] = 0;
  }

  return b;
}

/*

This function now does its own decoding of branch instructions to work out how to split chunks into basic blocks. This probably duplicates some work, which is bad.

*/

pheta_chunk* pheta_translatechunk(machineinfo* machine, uint5 base,
                                  uint5 length)
{
  uint5 i;
  hashtable* leaders = hash_new(8);
  pheta_chunk* chunk = pheta_newchunk(base, length);
  hashentry* p;
  
  chunk->leaders = leaders;
  
  // pre-scan to find internal branch destinations
  // generate a basic block at each of those locations
  for (i=0; i<length; i++)
  {
    instructionformat inst;
    inst.instruction = memory_readinstword(machine->mem, base+i*sizeof(uint5));
    
    if (inst.bra.ident == 5)  // should be sufficient to determine branches
    {
      sint5 offset = (sint5)(inst.bra.offset<<8)>>8;
      sint5 dest = i+offset+2;  // pipeline offset

      if (!hash_lookup(leaders, i+1))
      {
        p = hash_insert(leaders, i+1);
        p->data = pheta_newbasicblock(chunk, base+i*4+4);
      }

//      fprintf(stderr, "prescan: dest=%d\n", dest);

      if (dest>=0 && dest<length && !hash_lookup(leaders, i))
      {
        p = hash_insert(leaders, dest);
        p->data = pheta_newbasicblock(chunk, base+dest*4);
      }
    }
  }

  p = hash_insert(leaders, 0);
  chunk->root = p->data = pheta_newbasicblock(chunk, base);
  chunk->currentblock = 0;
  fprintf(stderr, "Chunk root=%x\n", chunk->root);
  
  for (i=0; i<length; i++)
  {
    instructionformat inst;
    hashentry* blockstart;
    
    chunk->virtualaddress = base+i*sizeof(uint5);
    inst.instruction = memory_readinstword(machine->mem, chunk->virtualaddress);

    if ((blockstart = hash_lookup(leaders, i)))
    {
      // chain adjacent blocks together
      if (chunk->currentblock)
      {
        pheta_basicblock* prev = chunk->currentblock;
        pheta_lsync(chunk);  // synchronise registers
        chunk->currentblock = blockstart->data;
        assert(prev != chunk->currentblock);
        if (prev->predicate==255)
          pheta_link(prev, ph_ALWAYS, chunk->currentblock, 0);
      }
      else chunk->currentblock = chunk->root;
    }

    // ew, this is pretty shaky...    
    if (inst.generic.cond==cc_AL || inst.bra.ident==5)
    {
      dispatch(machine, inst, &phet4, chunk);
    }
    else
    {
      pheta_basicblock* prev = chunk->currentblock;
      pheta_basicblock* thisinst = pheta_newbasicblock(chunk, base+i*4);
      pheta_basicblock* rest;
      hashentry* blk;

      if (!(blk = hash_lookup(leaders, i)))
        thisinst = pheta_newbasicblock(chunk, base+i*4);
      else
        thisinst = blk->data;

      dispatch(machine, inst, &phet4, chunk);
      
      if (!(blk = hash_lookup(leaders, i+1)))
        rest = pheta_newbasicblock(chunk, base+i*4+4);
      else
        rest = blk->data;

      pheta_link(thisinst, ph_AL, rest, 0);
      pheta_link(prev, inst.generic.cond, thisinst, rest);
    }
  }

  pheta_lsync(chunk);

  // this is a memory leak waiting to happen
  hash_nuke(leaders, NULL);

  pheta_dfs(chunk);

  return chunk;
}

void pheta_link(pheta_basicblock* from, uint5 pred,
                pheta_basicblock* condtrue, pheta_basicblock* condfalse)
{
  assert(pred<255);
  from->predicate = pred;
  from->trueblk = condtrue;
  from->falseblk = condfalse;
}

static void emitbyte(pheta_basicblock* block, uint5* written, uint3 byte)
{
  if (++block->length == block->size)
    block->base = realloc(block->base, block->size*=2);

  block->base[block->length-1] = byte;
  (*written)++;
}

// sometimes (ie complicated shifts) we need to use the same output
// for more than one ph2 instruction. This is the nicest solution I
// can think of for doing this, but it's *horrible* nonetheless.
// !!! (not used any more)
void pheta_forceoutput(pheta_chunk* chunk, uint5 reg)
{
  chunk->force = reg;
}

// !!! also not used
void pheta_unforceoutput(pheta_chunk* chunk)
{
  chunk->force = -1;
}

uint5 pheta_emit(pheta_chunk* chunk, pheta_opcode opcode, ...)
{
  va_list ap;
  uint5 dest=0, written=0;
  pheta_basicblock* block = chunk->currentblock;
  
  emitbyte(block, &written, opcode);

  va_start(ap, opcode);
  switch (opcode)
  {
    case ph_CONST:
    {
      uint5 word = va_arg(ap, uint5);
      if (chunk->force != -1)
        emitbyte(block, &written, dest = chunk->force);
      else
        emitbyte(block, &written, (dest = chunk->tempno++));
      emitbyte(block, &written, word & 0xff);
      emitbyte(block, &written, (word>>8) & 0xff);
      emitbyte(block, &written, (word>>16) & 0xff);
      emitbyte(block, &written, (word>>24) & 0xff);
    }
    break;
    
    case ph_CONSTB:
    {
      uint5 word = va_arg(ap, uint5);
      assert(word<0x100);
      if (chunk->force != -1)
        emitbyte(block, &written, dest = chunk->force);
      else
        emitbyte(block, &written, (dest = chunk->tempno++));
      emitbyte(block, &written, word & 0xff);
    }
    break;
    
    case ph_FETCH:
    if (chunk->force != -1)
      emitbyte(block, &written, dest = chunk->force);
    else
      emitbyte(block, &written, (dest = chunk->tempno++));
    emitbyte(block, &written, va_arg(ap, uint5));
    break;
    
    case ph_COMMIT:
    emitbyte(block, &written, va_arg(ap, uint5));
    emitbyte(block, &written, va_arg(ap, uint5));
    break;
    
    case ph_FEXPECT:
    case ph_FCOMMIT:
    case ph_FENSURE:
    case ph_NFEXPECT:
    case ph_NFCOMMIT:
    case ph_NFENSURE:
    emitbyte(block, &written, va_arg(ap, uint5));
    break;

    case ph_SETPRED:
    case ph_NSETPRED:
    emitbyte(block, &written, (dest = chunk->predno++));
    emitbyte(block, &written, va_arg(ap, uint5));
    break;
    
    case ph_XJMP:
    {
      uint5 word = va_arg(ap, uint5);
      dest = block->length;  // for patchbacking
      emitbyte(block, &written, word & 0xff);
      emitbyte(block, &written, (word>>8) & 0xff);
      emitbyte(block, &written, (word>>16) & 0xff);
      emitbyte(block, &written, (word>>24) & 0xff);
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
      if (chunk->force != -1)
        emitbyte(block, &written, dest = chunk->force);
      else
        emitbyte(block, &written, (dest = chunk->tempno++));
      emitbyte(block, &written, va_arg(ap, uint5));
      emitbyte(block, &written, va_arg(ap, uint5));
    }
    break;
    
    case ph_RRX:
    case ph_RLX:
    case ph_MOV:
    case ph_NOT:
    case ph_LDW:
    case ph_LDB:
    case ph_FWRITE:
    {
      if (chunk->force != -1)
        emitbyte(block, &written, dest = chunk->force);
      else
        emitbyte(block, &written, (dest = chunk->tempno++));
      emitbyte(block, &written, va_arg(ap, uint5));
    }
    break;
    
    case ph_TEQ:
    case ph_TST:
    case ph_CMP:
    case ph_CMN:
    case ph_STW:
    case ph_STB:
    {
      emitbyte(block, &written, va_arg(ap, uint5));
      emitbyte(block, &written, va_arg(ap, uint5));
    }
    break;
    
    default:
    break;
  }
  va_end(ap);
  
  emitbyte(block, &written, written+1);
  
  return dest;
}

// return the intermediate registers used by an instruction
void pheta_getused(uint3* base, int index, uint5* numdest, uint5 dest[],
  uint5* numsrc, uint5 src[])
{
  *numdest = 0;
  *numsrc = 0;
  switch (base[index++])
  {
    case ph_CONST:
    {
      dest[(*numdest)++] = base[index];
    }
    break;

    case ph_CONSTB:
    {
      dest[(*numdest)++] = base[index];
    }
    break;

    case ph_FETCH:
    {
      dest[(*numdest)++] = base[index];
    }
    break;

    case ph_COMMIT:
    {
      src[(*numsrc)++] = base[index+1];
    }
    break;

    case ph_FWRITE:
    {
      src[(*numsrc)++] = base[index+1];
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
      dest[(*numdest)++] = base[index];
      src[(*numsrc)++] = base[index+1];
      src[(*numsrc)++] = base[index+2];
    }
    break;

    case ph_RRX:
    case ph_RLX:
    case ph_MOV:
    case ph_NOT:
    case ph_LDW:
    case ph_LDB:
    {
      dest[(*numdest)++] = base[index];
      src[(*numsrc)++] = base[index+1];
    }
    break;

    case ph_TEQ:
    case ph_TST:
    case ph_CMP:
    case ph_CMN:
    case ph_STW:
    case ph_STB:
    {
      src[(*numsrc)++] = base[index];
      src[(*numsrc)++] = base[index+1];
    }
    break;
  }
}

void pheta_dfs(pheta_chunk* chunk)
{
  list* scanblock;

  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
  {
    pheta_basicblock* blk = (pheta_basicblock*) scanblock->data;
    blk->marker = col_WHITE;
  }

  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
  {
    pheta_basicblock* blk = (pheta_basicblock*)scanblock->data;
    if (blk->marker==col_WHITE) pheta_dfs_visit(blk);
  }
}
  
void pheta_dfs_visit(pheta_basicblock* blk)
{
  blk->marker = col_GREY;
  if (blk->trueblk && blk->trueblk->marker==col_WHITE)
  {
    blk->trueblk->parent = blk;
    pheta_dfs_visit(blk->trueblk);
  }
  if (blk->falseblk && blk->falseblk->marker==col_WHITE)
  {
    blk->falseblk->parent = blk;
    pheta_dfs_visit(blk->falseblk);
  }
  blk->marker = col_BLACK;
}

uint5 pheta_lfetch(pheta_chunk* chunk, uint5 regno)
{
  if (chunk->currentblock->lbuf[regno] != -1)
    return chunk->currentblock->lbuf[regno];
  else
    return (chunk->currentblock->lbuf[regno] =
            pheta_emit(chunk, ph_FETCH, regno));
}

void pheta_lcommit(pheta_chunk* chunk, uint5 regno, uint5 tempno)
{
  chunk->currentblock->lbuf[regno] = tempno;
  chunk->currentblock->dirtybuf[regno] = 1;
}

void pheta_lsync(pheta_chunk* chunk)
{
  int i;
  
  for (i=0; i<ph_NUMREG; i++)
  {
    if (chunk->currentblock->lbuf[i] != -1 && chunk->currentblock->dirtybuf[i])
    {
      pheta_emit(chunk, ph_COMMIT, i, chunk->currentblock->lbuf[i]);
      chunk->currentblock->lbuf[i] = -1;
    }
  }
}

void pheta_dp(machineinfo* machine, instructionformat inst, void* chunk)
{
  uint5 temp = inst.dp.operand2;
  uint5 shifttype = (temp>>5)&3;
  uint3 regshift = (temp & 16) ? 1 : 0;
  const uint5 logic = 0xf303;
  int islogic = logic & (1<<inst.dp.opcode);
  uint5 shiftreg;
  uint5 op2, rm = temp&15;
  pheta_basicblock* original = ((pheta_chunk*)chunk)->currentblock;
  
  op2 = pheta_lfetch(chunk, rm);
  
  if (regshift)
  {
    shiftreg = temp>>8;
    shiftreg = pheta_lfetch(chunk, shiftreg);
    
    if (shifttype != 3)
    {
      uint5 lowbyte = pheta_emit(chunk, ph_CONSTB, 0xff);
      shiftreg = pheta_emit(chunk, ph_AND, shiftreg, lowbyte);
    }
    
    switch (shifttype)
    {
      case 0:  // lsl / no shift
      {
        pheta_basicblock* testatshiftlimit;
        pheta_basicblock* undershiftlimit;
        pheta_basicblock* atshiftlimit;
        pheta_basicblock* overshiftlimit;
        pheta_basicblock* rest;
        int gtpred, shlimpred;

        pheta_emit(chunk, ph_NFEXPECT, ph_C | ph_V | ph_N | ph_Z);
        pheta_emit(chunk, ph_CMP, shiftreg, pheta_emit(chunk, ph_CONSTB, 31));
        pheta_emit(chunk, ph_NFCOMMIT, ph_C | ph_V | ph_N | ph_Z);
        gtpred = pheta_emit(chunk, ph_NSETPRED, ph_GT);
//        pheta_emit(chunk, ph_NFENSURE, ph_C | ph_V | ph_N | ph_Z);

        if (islogic && inst.dp.s)
        {
          undershiftlimit = pheta_newbasicblock(chunk, -1);
          
          pheta_emit(chunk, ph_FEXPECT, ph_C);
          op2 = pheta_emit(chunk, ph_LSL, op2, shiftreg);
          pheta_emit(chunk, ph_FCOMMIT, ph_C);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          testatshiftlimit = pheta_newbasicblock(chunk, -1);
          
          pheta_emit(chunk, ph_NFEXPECT, ph_C | ph_V | ph_N | ph_Z);
          pheta_emit(chunk, ph_CMP, shiftreg, pheta_emit(chunk, ph_CONSTB, 32));
          pheta_emit(chunk, ph_NFCOMMIT, ph_C | ph_V | ph_N | ph_Z);
          shlimpred = pheta_emit(chunk, ph_NSETPRED, ph_EQ);
          
          atshiftlimit = pheta_newbasicblock(chunk, -1);
          
          pheta_emit(chunk, ph_FEXPECT, ph_C);
          pheta_emit(chunk, ph_LSR, op2, pheta_emit(chunk, ph_CONSTB, 1));
          pheta_emit(chunk, ph_FCOMMIT, ph_C);
          
          op2 = pheta_emit(chunk, ph_CONSTB, 0);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          overshiftlimit = pheta_newbasicblock(chunk, -1);

          op2 = pheta_emit(chunk, ph_CONSTB, 0);
          pheta_emit(chunk, ph_FWRITE, ph_C, pheta_emit(chunk, ph_CONSTB, 0));
          pheta_dp_guts(machine, inst, chunk, op2);
          
          rest = pheta_newbasicblock(chunk, -1);
          
          pheta_link(original, gtpred, testatshiftlimit, undershiftlimit);
          pheta_link(undershiftlimit, ph_ALWAYS, rest, 0);
          pheta_link(testatshiftlimit, shlimpred, atshiftlimit, overshiftlimit);
          pheta_link(atshiftlimit, ph_ALWAYS, rest, 0);
          pheta_link(overshiftlimit, ph_ALWAYS, rest, 0);
        }
        else  // no flag nonsense
        {
          undershiftlimit = pheta_newbasicblock(chunk, -1);
          
          op2 = pheta_emit(chunk, ph_LSL, op2, shiftreg);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          overshiftlimit = pheta_newbasicblock(chunk, -1);
          
          op2 = pheta_emit(chunk, ph_CONSTB, 0);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          rest = pheta_newbasicblock(chunk, 
            ((pheta_chunk*)chunk)->virtualaddress+4);
          
          pheta_link(original, gtpred, overshiftlimit, undershiftlimit);
          pheta_link(undershiftlimit, ph_ALWAYS, rest, 0);
          pheta_link(overshiftlimit, ph_ALWAYS, rest, 0);
        }
      }
      break;
      
      case 1:  // lsr
      {
        pheta_basicblock* testatshiftlimit;
        pheta_basicblock* undershiftlimit;
        pheta_basicblock* atshiftlimit;
        pheta_basicblock* overshiftlimit;
        pheta_basicblock* rest;

        pheta_emit(chunk, ph_NFEXPECT, ph_C | ph_V | ph_N | ph_Z);
        pheta_emit(chunk, ph_CMP, shiftreg, pheta_emit(chunk, ph_CONSTB, 31));
        pheta_emit(chunk, ph_NFCOMMIT, ph_C | ph_V | ph_N | ph_Z);
//        pheta_emit(chunk, ph_NFENSURE, ph_C | ph_V | ph_N | ph_Z);

        if (islogic && inst.dp.s)
        {
          undershiftlimit = pheta_newbasicblock(chunk, -1);
          
          pheta_emit(chunk, ph_FEXPECT, ph_C);
          op2 = pheta_emit(chunk, ph_LSR, op2, shiftreg);
          pheta_emit(chunk, ph_FCOMMIT, ph_C);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          testatshiftlimit = pheta_newbasicblock(chunk, -1);
          
          pheta_emit(chunk, ph_NFEXPECT, ph_C | ph_V | ph_N | ph_Z);
          pheta_emit(chunk, ph_CMP, shiftreg, pheta_emit(chunk, ph_CONSTB, 32));
          pheta_emit(chunk, ph_NFCOMMIT, ph_C | ph_V | ph_N | ph_Z);
          
          atshiftlimit = pheta_newbasicblock(chunk, -1);
          
          pheta_emit(chunk, ph_FEXPECT, ph_C);
          pheta_emit(chunk, ph_LSL, op2, pheta_emit(chunk, ph_CONSTB, 1));
          pheta_emit(chunk, ph_FCOMMIT, ph_C);
          
          op2 = pheta_emit(chunk, ph_CONSTB, 0);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          overshiftlimit = pheta_newbasicblock(chunk, -1);

          op2 = pheta_emit(chunk, ph_CONSTB, 0);
          pheta_emit(chunk, ph_FWRITE, ph_C, pheta_emit(chunk, ph_CONSTB, 0));
          pheta_dp_guts(machine, inst, chunk, op2);
          
          rest = pheta_newbasicblock(chunk, -1);
          
          pheta_link(original, ph_GT, testatshiftlimit, undershiftlimit);
          pheta_link(undershiftlimit, 0, rest, 0);
          pheta_link(testatshiftlimit, ph_EQ, atshiftlimit, overshiftlimit);
          pheta_link(atshiftlimit, 0, rest, 0);
          pheta_link(overshiftlimit, 0, rest, 0);
        }
        else  // no flag nonsense
        {
          undershiftlimit = pheta_newbasicblock(chunk, -1);
          
          op2 = pheta_emit(chunk, ph_LSR, op2, shiftreg);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          overshiftlimit = pheta_newbasicblock(chunk, -1);
          
          op2 = pheta_emit(chunk, ph_CONSTB, 0);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          rest = pheta_newbasicblock(chunk, 
            ((pheta_chunk*)chunk)->virtualaddress+4);
          
          pheta_link(original, ph_GT, overshiftlimit, undershiftlimit);
          pheta_link(undershiftlimit, ph_AL, rest, 0);
          pheta_link(overshiftlimit, ph_AL, rest, 0);
        }
      }
      break;

      case 2:  // asr
      {
        uint5 shiftlimit = pheta_emit(chunk, ph_CONSTB, 31);
        pheta_basicblock* undershiftlimit;
        pheta_basicblock* overshiftlimit;
        pheta_basicblock* rest;

        pheta_emit(chunk, ph_NFEXPECT, ph_C | ph_V | ph_N | ph_Z);
        pheta_emit(chunk, ph_CMP, shiftreg, shiftlimit);
        pheta_emit(chunk, ph_NFCOMMIT, ph_C | ph_V | ph_N | ph_Z);
//        pheta_emit(chunk, ph_NFENSURE, ph_C | ph_V | ph_N | ph_Z);

        if (islogic && inst.dp.s)
        {
          undershiftlimit = pheta_newbasicblock(chunk, -1);
          
          pheta_emit(chunk, ph_FEXPECT, ph_C);
          op2 = pheta_emit(chunk, ph_ASR, op2, shiftreg);
          pheta_emit(chunk, ph_FCOMMIT, ph_C);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          overshiftlimit = pheta_newbasicblock(chunk, -1);
          
          op2 = pheta_emit(chunk, ph_ASR, op2, shiftlimit);
          pheta_emit(chunk, ph_FEXPECT, ph_C);
          pheta_emit(chunk, ph_ASR, op2, pheta_emit(chunk, ph_CONSTB, 1));
          pheta_emit(chunk, ph_FCOMMIT, ph_C);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          rest = pheta_newbasicblock(chunk, -1);
          
          pheta_link(original, ph_GT, overshiftlimit, undershiftlimit);
          pheta_link(undershiftlimit, ph_AL, rest, 0);
          pheta_link(overshiftlimit, ph_AL, rest, 0);
        }
        else  // no flags
        {
          undershiftlimit = pheta_newbasicblock(chunk, -1);
          
          op2 = pheta_emit(chunk, ph_ASR, op2, shiftreg);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          overshiftlimit = pheta_newbasicblock(chunk, -1);
          
          op2 = pheta_emit(chunk, ph_ASR, op2, shiftlimit);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          rest = pheta_newbasicblock(chunk, -1);
        }

        pheta_link(original, ph_GT, overshiftlimit, undershiftlimit);
        pheta_link(undershiftlimit, ph_AL, rest, 0);
        pheta_link(overshiftlimit, ph_AL, rest, 0);
      }
      break;
      
      case 3:  // ror
      {
        uint5 shiftmask = pheta_emit(chunk, ph_CONSTB, 31);
        
        if (islogic && inst.dp.s)
        {
          pheta_basicblock* notzero;
          pheta_basicblock* zero;
          pheta_basicblock* rest;
          
          pheta_emit(chunk, ph_NFEXPECT, ph_Z | ph_N);
          shiftreg = pheta_emit(chunk, ph_AND, shiftreg, shiftmask);
          pheta_emit(chunk, ph_NFCOMMIT, ph_Z | ph_N);
//          pheta_emit(chunk, ph_NFENSURE, ph_Z | ph_N);

          if (islogic && inst.dp.s)
          {
            int op2b;
            notzero = pheta_newbasicblock(chunk, -1);
            
            pheta_emit(chunk, ph_FEXPECT, ph_C);
            op2b = pheta_emit(chunk, ph_ROR, op2, shiftreg);
            pheta_emit(chunk, ph_FCOMMIT, ph_C);
            pheta_dp_guts(machine, inst, chunk, op2b);
            
            zero = pheta_newbasicblock(chunk, -1);
            
            pheta_dp_guts(machine, inst, chunk, op2);
            
            rest = pheta_newbasicblock(chunk, -1);
            
            pheta_link(original, ph_EQ, zero, notzero);
            pheta_link(notzero, ph_AL, rest, 0);
            pheta_link(zero, ph_AL, rest, 0);
          }
          else  // no flags
          {
            op2 = pheta_emit(chunk, ph_ROR, shiftreg);
            pheta_dp_guts(machine, inst, chunk, op2);
          }
        }
        else
        {
          shiftreg = pheta_emit(chunk, ph_AND, shiftreg, shiftmask);
          op2 = pheta_emit(chunk, ph_ROR, op2, shiftreg);
          pheta_dp_guts(machine, inst, chunk, op2);
        }
      }
      break;
    }  // switch (shifttype)
  }
  else  // shift by immediate
  {
    int amount = temp >> 7;
    int amountreg = -1;
    
    if (shifttype!=0 || amount!=0)
    {
      amountreg = pheta_emit(chunk, ph_CONSTB, amount);
    }
    
    switch (shifttype)
    {
      case 0:  // lsl/no shift
      {
        if (amount)
        {
          if (islogic && inst.dp.s)
          {
            pheta_emit(chunk, ph_FEXPECT, ph_C);
            op2 = pheta_emit(chunk, ph_LSL, op2, amountreg);
            pheta_emit(chunk, ph_FCOMMIT, ph_C);
          }
          else
            op2 = pheta_emit(chunk, ph_LSL, op2, amountreg);
        }
        pheta_dp_guts(machine, inst, chunk, op2);
      }
      break;
      
      case 1:  // lsr
      {
        if (amount)
        {
          if (islogic && inst.dp.s)
          {
            pheta_emit(chunk, ph_FEXPECT, ph_C);
            op2 = pheta_emit(chunk, ph_LSR, op2, amountreg);
            pheta_emit(chunk, ph_FCOMMIT, ph_C);
          }
          else
          {
            op2 = pheta_emit(chunk, ph_LSR, op2, amountreg);
          }
        }
        else  // amount==0 means right-shift by 32
        {
          if (islogic && inst.dp.s)
          {
            int one = pheta_emit(chunk, ph_CONSTB, 1);
            pheta_emit(chunk, ph_FEXPECT, ph_C);
            pheta_emit(chunk, ph_LSL, op2, one);  // shift top bit into flag
            pheta_emit(chunk, ph_FCOMMIT, ph_C);
          }
          op2 = pheta_emit(chunk, ph_CONSTB, 0);
        }
        pheta_dp_guts(machine, inst, chunk, op2);
      }
      break;
      
      case 2:  // asr
      {
        if (amount)
        {
          if (islogic && inst.dp.s)
          {
            pheta_emit(chunk, ph_FEXPECT, ph_C);
            op2 = pheta_emit(chunk, ph_ASR, op2, amountreg);
            pheta_emit(chunk, ph_FCOMMIT, ph_C);
          }
          else
          {
            op2 = pheta_emit(chunk, ph_ASR, op2, amountreg);
          }
        }
        else  // amount==0 means right-shift by 32
        {
          int thirtyone = pheta_emit(chunk, ph_CONSTB, 31);
          op2 = pheta_emit(chunk, ph_ASR, op2, thirtyone);

          if (islogic && inst.dp.s)
          {
            pheta_emit(chunk, ph_FEXPECT, ph_C);
            pheta_emit(chunk, ph_ASR, op2, pheta_emit(chunk, ph_CONSTB, 1));
            pheta_emit(chunk, ph_FCOMMIT, ph_C);
          }
        }
        pheta_dp_guts(machine, inst, chunk, op2);
      }
      break;
      
      case 3:  // ror/rrx
      {
        if (amount)  // ror
        {
          if (islogic && inst.dp.s)
          {
            pheta_emit(chunk, ph_FEXPECT, ph_C);
            op2 = pheta_emit(chunk, ph_ROR, op2, amountreg);
            pheta_emit(chunk, ph_FCOMMIT, ph_C);
          }
          else
          {
            op2 = pheta_emit(chunk, ph_ROR, op2, amountreg);
          }
        }
        else
        {
          pheta_emit(chunk, ph_FENSURE, ph_C);

          if (islogic && inst.dp.s)
          {
            pheta_emit(chunk, ph_FEXPECT, ph_C);
            op2 = pheta_emit(chunk, ph_RRX, op2,
                             pheta_emit(chunk, ph_CONSTB, 1));
            pheta_emit(chunk, ph_FCOMMIT, ph_C);
          }
          else
          {
            op2 = pheta_emit(chunk, ph_RRX, op2,
                             pheta_emit(chunk, ph_CONSTB, 1));
          }
        }
        pheta_dp_guts(machine, inst, chunk, op2);
      }
      break;
    }  // shift type
  }  // register/immediate
}

void pheta_dp_imm(machineinfo* machine, instructionformat inst, void* chunk)
{
  uint5 temp = inst.dp.operand2;
  uint5 value = temp&255, amount = (temp>>8)*2, op2;
  const uint5 logic = 0xf303;
  int islogic = logic & (1<<inst.dp.opcode);

  if (amount==0)
  {
    op2 = pheta_emit(chunk, ph_CONSTB, value);
  }
  else
  {
    op2 = pheta_emit(chunk, ph_CONST, ROR(value, amount));

    if (inst.dp.s && islogic)
    {
      pheta_emit(chunk, ph_FWRITE, ph_C,
        pheta_emit(chunk, ph_CONSTB, (value & (1U<<(amount-1))) ? 1 : 0));
    }
  }
  
  pheta_dp_guts(machine, inst, chunk, op2);
}

void pheta_dp_guts(machineinfo* machine, instructionformat inst,
                   pheta_chunk* chunk, uint5 op2)
{
  sint5 dest = -1;
  sint5 op1 = -1;
  const uint5 needop1mask = 0x5fff;
  
  if (needop1mask & (1<<inst.dp.opcode))
    op1 = pheta_lfetch(chunk, inst.dp.rn);
  
  switch (inst.dp.opcode)
  {
    case dp_AND:
    {
      if (inst.dp.s)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_AND, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_N | ph_Z);
      }
      else
      {
        dest = pheta_emit(chunk, ph_AND, op1, op2);
      }
    }
    break;

    case dp_EOR:
    {
      if (inst.dp.s)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_EOR, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_N | ph_Z);
      }
      else
      {
        dest = pheta_emit(chunk, ph_EOR, op1, op2);
      }
    }
    break;

    case dp_SUB:
    {
      if (inst.dp.s)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_C | ph_V | ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_SUB, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_C | ph_V | ph_N | ph_Z);
      }
      else
      {
        dest = pheta_emit(chunk, ph_SUB, op1, op2);
      }
    }
    break;

    case dp_RSB:
    {
      if (inst.dp.s)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_C | ph_V | ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_SUB, op2, op1);
        pheta_emit(chunk, ph_FCOMMIT, ph_C | ph_V | ph_N | ph_Z);
      }
      else
      {
        dest = pheta_emit(chunk, ph_SUB, op2, op1);
      }
    }
    break;

    case dp_ADD:
    {
      if (inst.dp.s)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_C | ph_V | ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_ADD, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_C | ph_V | ph_N | ph_Z);
      }
      else
      {
        dest = pheta_emit(chunk, ph_ADD, op1, op2);
      }
    }
    break;

    case dp_ADC:
    {
      pheta_emit(chunk, ph_FENSURE, ph_C);
      if (inst.dp.s)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_C | ph_V | ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_ADC, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_C | ph_V | ph_N | ph_Z);
      }
      else
      {
        dest = pheta_emit(chunk, ph_ADC, op1, op2);
      }
    }
    break;

    case dp_SBC:
    {
      pheta_emit(chunk, ph_FENSURE, ph_C);
      if (inst.dp.s)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_C | ph_V | ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_SBC, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_C | ph_V | ph_N | ph_Z);
      }
      else
      {
        dest = pheta_emit(chunk, ph_SBC, op1, op2);
      }
    }
    break;

    case dp_RSC:
    {
      pheta_emit(chunk, ph_FENSURE, ph_C);
      if (inst.dp.s)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_C | ph_V | ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_SBC, op2, op1);
        pheta_emit(chunk, ph_FCOMMIT, ph_C | ph_V | ph_N | ph_Z);
      }
      else
      {
        dest = pheta_emit(chunk, ph_SBC, op2, op1);
      }
    }
    break;
    
    case dp_TST:
    {
      if (inst.dp.s)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_N | ph_Z);
        pheta_emit(chunk, ph_TST, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_N | ph_Z);
      }
      else
        fprintf(stderr, "Warning: TST with clear S flag!\n");
    }
    break;
    
    case dp_TEQ:
    {
      if (inst.dp.s)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_N | ph_Z);
        pheta_emit(chunk, ph_TEQ, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_N | ph_Z);
      }
      else
        fprintf(stderr, "Warning: TEQ with clear S flag!\n");
    }
    break;
    
    case dp_CMP:
    {
      if (inst.dp.s)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_C | ph_V | ph_N | ph_Z);
        pheta_emit(chunk, ph_CMP, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_C | ph_V | ph_N | ph_Z);
      }
      else
        fprintf(stderr, "Warning: CMP with clear S flag!\n");
    }
    break;
    
    case dp_CMN:
    {
      if (inst.dp.s)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_C | ph_V | ph_N | ph_Z);
        pheta_emit(chunk, ph_CMN, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_C | ph_V | ph_N | ph_Z);
      }
      else
        fprintf(stderr, "Warning: CMN with clear S flag!\n");
    }
    break;
    
    case dp_ORR:
    {
      if (inst.dp.s)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_OR, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_N | ph_Z);
      }
      else
      {
        dest = pheta_emit(chunk, ph_OR, op1, op2);
      }
    }
    break;
    
    case dp_MOV:
    {
      if (inst.dp.s)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_MOV, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_N | ph_Z);
      }
      else
      {
        dest = pheta_emit(chunk, ph_MOV, op2);
      }
    }
    break;
    
    case dp_BIC:
    {
      op2 = pheta_emit(chunk, ph_NOT, op2);
      if (inst.dp.s)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_AND, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_N | ph_Z);
      }
      else
      {
        dest = pheta_emit(chunk, ph_AND, op1, op2);
      }
    }
    break;
    
    case dp_MVN:
    {
      if (inst.dp.s)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_NOT, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_N | ph_Z);
      }
      else
      {
        dest = pheta_emit(chunk, ph_NOT, op2);
      }
    }
    break;
  }
  
  if (dest != -1)
  {
    pheta_lcommit(chunk, inst.dp.rd, dest);
  }
}

void pheta_bra(machineinfo* machine, instructionformat inst, void* data)
{
  sint5 offset = ((sint5)(inst.bra.offset<<8))>>8;
  pheta_chunk* chunk = (pheta_chunk*)data;
  uint5 index = ((chunk->virtualaddress-chunk->start)>>2);
  uint5 dest = index+offset+2;
  uint5 next = index+1;
    
  if (inst.bra.l)
  {
    uint5 destpc = pheta_emit(chunk, ph_CONST, chunk->virtualaddress-4);
    pheta_lcommit(chunk, ph_R14, destpc);
  }
  
  if (dest>=0 && dest<chunk->length)
  {
    hashentry* taken = hash_lookup(chunk->leaders, dest);
    hashentry* nottaken = hash_lookup(chunk->leaders, next);
    uint5 blkpred;

//  pheta_emit(chunk, ph_FEXPECT, ph_C|ph_V|ph_N|ph_Z);
/*    fprintf(stderr, "linking %x to %x,%x with %d\n", chunk->currentblock,
      taken->data, nottaken->data, inst.bra.cond);*/
    blkpred = pheta_emit(chunk, ph_SETPRED, inst.bra.cond);
    pheta_link(chunk->currentblock, blkpred, taken->data, nottaken->data);
  }
  else  // going outside - ack!
  {
    pheta_basicblock* previous = chunk->currentblock;
    pheta_basicblock* exitchunk = pheta_newbasicblock(chunk, -1);
    hashentry* nottaken = hash_lookup(chunk->leaders, next);
    pheta_lsync(chunk);
    pheta_emit(chunk, ph_SYNC);
    // deal with xjmp later
    pheta_emit(chunk, ph_END);
    pheta_link(previous, inst.bra.cond, exitchunk, nottaken->data);
  }
}

void pheta_mul(machineinfo* machine, instructionformat inst, void* chunk)
{
  uint5 op1 = pheta_lfetch(chunk, inst.mul.rm);
  uint5 op2 = pheta_lfetch(chunk, inst.mul.rs);
  uint5 dest;

  if (inst.mul.a)  // accumulate
  {
    uint5 temp = pheta_emit(chunk, ph_MUL, op1, op2);
    if (inst.mul.s)
    {
      pheta_emit(chunk, ph_FEXPECT, ph_N | ph_Z);
      dest = pheta_emit(chunk, ph_ADD, temp,
                        pheta_lfetch(chunk, inst.mul.rn));
      pheta_emit(chunk, ph_FCOMMIT, ph_N | ph_Z);
    }
    else
    {
      dest = pheta_emit(chunk, ph_ADD, temp,
                        pheta_lfetch(chunk, inst.mul.rn));
    }
  }
  else
  {
    if (inst.mul.s)
    {
      pheta_emit(chunk, ph_FEXPECT, ph_N | ph_Z);
      dest = pheta_emit(chunk, ph_MUL, op1, op2);
      pheta_emit(chunk, ph_FCOMMIT, ph_N | ph_Z);
    }
    else
    {
      dest = pheta_emit(chunk, ph_MUL, op1, op2);
    }
  }

  pheta_lcommit(chunk, inst.mul.rd, dest);
}

void pheta_sdt(machineinfo* machine, instructionformat inst, void* chunk)
{
  int basereg = pheta_lfetch(chunk, inst.sdt.rn);
  sint5 offsetreg = -1;  // -1 for zero/no offset
  
  if (inst.sdt.i)  // shifted register offset
  {
    uint5 temp = inst.sdt.offset;
    uint5 rm = temp&15, shifttype = (temp>>5)&3, amount = (temp>>7);
    
    switch (shifttype)
    {
      case 0:  // LSL / no shift
      {
        if (amount)
        {
          offsetreg = pheta_emit(chunk, ph_LSL,
            pheta_lfetch(chunk, rm),
            pheta_emit(chunk, ph_CONSTB, amount));
        }
        else
        {
          offsetreg = pheta_lfetch(chunk, rm);
        }
      }
      break;
      
      case 1:  // LSR
      {
        if (amount)
        {
          offsetreg = pheta_emit(chunk, ph_LSR,
            pheta_lfetch(chunk, rm),
            pheta_emit(chunk, ph_CONSTB, amount));
        }
        else
        {
          offsetreg = -1;
        }
      }
      break;
      
      case 2:  // ASR
      {
        if (amount)
        {
          offsetreg = pheta_emit(chunk, ph_ASR,
            pheta_lfetch(chunk, rm),
            pheta_emit(chunk, ph_CONSTB, amount));
        }
        else
        {
          // 31 is the same as 32 in this case (no carry out)
          offsetreg = pheta_emit(chunk, ph_ASR,
            pheta_lfetch(chunk, rm),
            pheta_emit(chunk, ph_CONSTB, 31));
        }
      }
      break;
      
      case 3:  // ROR/RRX
      {
        if (amount)
        {
          offsetreg = pheta_emit(chunk, ph_ROR,
            pheta_lfetch(chunk, rm),
            pheta_emit(chunk, ph_CONSTB, amount));
        }
        else
        {
          pheta_emit(chunk, ph_FENSURE, ph_C);
          offsetreg = pheta_emit(chunk, ph_RRX,
            pheta_lfetch(chunk, rm));
        }
      }
    }
    if (offsetreg != -1 && !inst.sdt.u)
    {
      offsetreg = pheta_emit(chunk, ph_SUB, pheta_emit(chunk, ph_CONSTB, 0),
        offsetreg);
    }
  }
  else  // 'i' bit zero, immediate offset
  {
    if (inst.sdt.u)
      offsetreg = pheta_emit(chunk, ph_CONST, inst.sdt.offset);
    else
      offsetreg = pheta_emit(chunk, ph_CONST, -inst.sdt.offset);
  }
  
  if (inst.sdt.p && offsetreg != -1)  // pre-indexed if 1
    basereg = pheta_emit(chunk, ph_ADD, basereg, offsetreg);
  
  if (inst.sdt.l)  // load
  {
    if (inst.sdt.b)  // it's a byte!
    {
      int load = pheta_emit(chunk, ph_LDB, basereg);
      pheta_lcommit(chunk, inst.sdt.rd, load);
    }
    else  // it's a word!
    {
      int load = pheta_emit(chunk, ph_LDW, basereg);
      pheta_lcommit(chunk, inst.sdt.rd, load);
    }
  }
  else  // save (pc+12 issue!)
  {
    if (inst.sdt.b) // byte
    {
      int store = pheta_lfetch(chunk, inst.sdt.rd);
      if (inst.sdt.rd==15)
      {
        store = pheta_emit(chunk, ph_ADD, chunk,
                           pheta_emit(chunk, ph_CONSTB, 4));
      }
      pheta_emit(chunk, ph_STB, store, basereg);
    }
    else  // word
    {
      int store = pheta_lfetch(chunk, inst.sdt.rd);
      if (inst.sdt.rd==15)
      {
        store = pheta_emit(chunk, ph_ADD, chunk,
                           pheta_emit(chunk, ph_CONSTB, 4));
      }
      pheta_emit(chunk, ph_STW, store, basereg);
    }
  }
  
  if (!inst.sdt.p && offsetreg != -1)  // post-indexed addressing
  {
    basereg = pheta_emit(chunk, ph_ADD, basereg, offsetreg);
    pheta_lcommit(chunk, inst.sdt.rn, basereg);
  }
  else if (inst.sdt.w)  // writeback address into base
  {
    pheta_lcommit(chunk, inst.sdt.rn, basereg);
  }
}

void pheta_bdt(machineinfo* machine, instructionformat inst, void* chunk)
{
  uint5 basereg = pheta_lfetch(chunk, inst.bdt.rn);
  uint5 fourreg = pheta_emit(chunk, ph_CONSTB, 4);
  
  if (inst.bdt.w)  // writeback (real ARM does this before the transfer)
  {
    uint5 offsetby;

    if (inst.bdt.u)  // upwards
      offsetby = pheta_emit(chunk, ph_ADD, basereg,
        pheta_emit(chunk, ph_CONSTB, 4*setbits(inst.bdt.reglist)));
    else
      offsetby = pheta_emit(chunk, ph_SUB, basereg,
        pheta_emit(chunk, ph_CONSTB, 4*setbits(inst.bdt.reglist)));

    pheta_lcommit(chunk, inst.bdt.rn, offsetby);
  }

  if (inst.bdt.u)  // transfer upwards
  {
    int i;
    for (i=0; i<16; i++)
    {
      if (inst.bdt.reglist & (1<<i))
      {
        if (inst.bdt.p)  // preincrement
          basereg = pheta_emit(chunk, ph_ADD, basereg, fourreg);

        if (inst.bdt.l)
        {
          int load = pheta_emit(chunk, ph_LDW, basereg);
          if (i==15)
          {
            pheta_lcommit(chunk, inst.bdt.s ? ph_R15_FULL : ph_R15_ADDR, load);
          }
          else
            pheta_lcommit(chunk, i, load);
        }
        else
        {
          // where 'i' is 15, will use ph_R15_FULL
          int store = pheta_lfetch(chunk, i);
          pheta_emit(chunk, ph_STW, store, basereg);
        }
        if (!inst.bdt.p)  // postincrement
          basereg = pheta_emit(chunk, ph_ADD, basereg, fourreg);
      }
    }
  }
  else  // transfer registers downwards
  {
    int i;
    for (i=15; i>=0; i--)
    {
      if (inst.bdt.reglist & (1<<i))
      {
        if (inst.bdt.p)  // predecrement
          basereg = pheta_emit(chunk, ph_SUB, basereg, fourreg);

        if (inst.bdt.l)
        {
          int load = pheta_emit(chunk, ph_LDW, basereg);
          if (i==15)
          {
            pheta_lcommit(chunk, inst.bdt.s ? ph_R15_FULL : ph_R15_ADDR, load);
          }
          else
            pheta_lcommit(chunk, i, load);
        }
        else
        {
          // where 'i' is 15, will use ph_R15_FULL
          int store = pheta_lfetch(chunk, i);
          pheta_emit(chunk, ph_STW, store, basereg);
        }
        if (!inst.bdt.p)  // postdecrement
          basereg = pheta_emit(chunk, ph_SUB, basereg, fourreg);
      }
    }
  }
}

void pheta_swi(machineinfo* machine, instructionformat inst, void* chunk)
{
  pheta_lsync(chunk);
  pheta_emit(chunk, ph_SYNC);
  pheta_emit(chunk, ph_SWI);
}

void pheta_cdt(machineinfo* machine, instructionformat inst, void* chunk)
{
  pheta_lsync(chunk);
  pheta_emit(chunk, ph_SYNC);
  pheta_emit(chunk, ph_UNDEF);
}

void pheta_cdo(machineinfo* machine, instructionformat inst, void* chunk)
{
  pheta_lsync(chunk);
  pheta_emit(chunk, ph_SYNC);
  pheta_emit(chunk, ph_UNDEF);
}

void pheta_crt(machineinfo* machine, instructionformat inst, void* chunk)
{
  pheta_lsync(chunk);
  pheta_emit(chunk, ph_SYNC);
  pheta_emit(chunk, ph_UNDEF);
}

void pheta_sds(machineinfo* machine, instructionformat inst, void* chunk)
{
  fprintf(stderr, "Unimplemented instruction: SWP\n");
  exit(1);
}

void pheta_und(machineinfo* machine, instructionformat inst, void* chunk)
{
  pheta_lsync(chunk);
  pheta_emit(chunk, ph_SYNC);
  pheta_emit(chunk, ph_UNDEF);
}
