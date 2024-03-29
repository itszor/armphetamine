#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <limits.h>

#include "libjtype/defs.h"
#include "dynarec/pheta.h"
#include "libjtype/cnew.h"
#include "core/machine.h"
#include "libjtype/list.h"
#include "core/decode.h"
#include "libjtype/pqueue.h"
#include "dynarec/palloc.h"
#include "libjtype/clist.h"
#include "dynarec/phetadism.h"
#include "libjtype/bset.h"
#include "dynarec/arch/x86/genx86.h"

static const uint5 ccflags[] =
{
  /* eq */ ph_Z,
  /* ne */ ph_Z,
  /* cs */ ph_C,
  /* cc */ ph_C,
  /* mi */ ph_N,
  /* pl */ ph_N,
  /* vs */ ph_V,
  /* vc */ ph_V,
  /* hi */ ph_C | ph_Z,
  /* ls */ ph_C | ph_Z,
  /* ge */ ph_N | ph_V,
  /* lt */ ph_N | ph_V,
  /* gt */ ph_Z | ph_N | ph_V,
  /* le */ ph_Z | ph_N | ph_V,
  /* al */ 0,
  /* nv */ 0
};

pheta_chunk* pheta_newchunk(uint5 start, uint5 length)
{
  pheta_chunk* p = jt_new(pheta_chunk);
  p->blocks = 0;
  p->start = start;
  p->length = length;
  p->tempno = 0;
  p->predno = 1;
  p->currentblock = 0;
  p->assoc = 0;
  p->constantpool = jt_hash_new(32);
  p->registerpool = jt_hash_new(8);
  p->baseoffsetpool = jt_hash_new(32);
    
  return p;
}

void pheta_deletechunk(pheta_chunk* chunk)
{
  jt_delete(chunk);  // and lots more!
}

// Generate a new basic block, also make it current
pheta_basicblock* pheta_newbasicblock(pheta_chunk* c, uint5 startaddr)
{
  int i;
  pheta_basicblock* b;

  if (c->currentblock) pheta_lsync(c);

  jt_list_add(&c->blocks);
  c->blocks->data = b = jt_new(pheta_basicblock);
  b->base = jt_clist_new();
  b->gxbuffer = genx86_newbuffer();
  b->length = 0;
  b->predicate = 255;
  b->trueblk = 0;
  b->falseblk = 0;
  b->parent = 0;
  b->predecessor = jt_clist_new();
  b->scsubgraph = 0;
  b->srcstart = startaddr;
  b->cycles = 0;
  b->live = jt_pqueue_new();
  b->required = 0;
  b->set = 0;
  b->reloc = 0;
  
  c->currentblock = b;

  for (i=0; i<ph_NUMREG; i++)
  {
    b->lbuf[i] = -1;
    b->dirtybuf[i] = 0;
  }

  return b;
}

void pheta_destroybasicblock(pheta_basicblock* blk)
{
  jt_delete(blk->base);
  jt_delete(blk->predecessor);
  jt_pqueue_delete(blk->live);
  jt_delete(blk);
}

void pheta_cyclecount(pheta_chunk* chunk)
{
  uint5 cyc = pheta_emit(chunk, ph_FETCH, ph_CLOCK);
  pheta_emit(chunk, ph_NFEXPECT, ph_C|ph_V|ph_N|ph_Z);
  cyc = pheta_emit(chunk, ph_SUB, cyc, pheta_emit(chunk, ph_CONST, 
    chunk->currentblock->cycles));
  pheta_emit(chunk, ph_NFCOMMIT, ph_C|ph_V|ph_N|ph_Z, 0, bitcc_LE);
  pheta_emit(chunk, ph_COMMIT, ph_CLOCK, cyc);
}

/*

This function now does its own decoding of branch instructions to work out how to split chunks into basic blocks. This probably duplicates some work, which is bad.

*/

pheta_chunk* pheta_translatechunk(machineinfo* machine, uint5 base,
                                  uint5 length)
{
  uint5 i;
  jt_hash* leaders = jt_hash_new(8);
  pheta_chunk* chunk = pheta_newchunk(base, length);
  jt_hashentry* p;
  uint5 tail;
  uint5 synced = 0;
  
  chunk->leaders = leaders;
  
  // pre-scan to find internal branch destinations
  // generate a basic block at each of those locations
  for (i=0; i<length; i++)
  {
    instructionformat inst;
    inst.instruction = memory_readinstword(machine->mem, base+i*sizeof(uint5));

    fprintf(stderr, "%.8x : %.8x : ", base+i*4, inst.instruction);
    dispatch(machine, inst, &diss, (void*)(base+i*4));
    fprintf(stderr, "\n");
    
    if (inst.bra.ident == 5)  // should be sufficient to determine branches
    {
      sint5 offset = (sint5)(inst.bra.offset<<8)>>8;
      sint5 dest = i+offset+2;  // pipeline offset

      if (!jt_hash_lookup(leaders, i+1))
      {
        p = jt_hash_insert(leaders, i+1);
/*        p->data = pheta_newbasicblock(chunk, base+i*4+4);*/
      }

//      fprintf(stderr, "prescan: dest=%d\n", dest);

      if (dest>=0 && dest<(sint5)length && !jt_hash_lookup(leaders, i))
      {
        p = jt_hash_insert(leaders, dest);
/*        p->data = pheta_newbasicblock(chunk, base+dest*4);*/
      }
    }
  }

  p = jt_hash_insert(leaders, 0);
/*  chunk->root = p->data = pheta_newbasicblock(chunk, base);*/
  chunk->root = 0;
  chunk->currentblock = 0;
  fprintf(stderr, "Chunk root=%p\n", chunk->root);

  for (i=0; i<length; i++)
  {
    instructionformat inst;
    jt_hashentry* blockstart;

    chunk->virtualaddress = base+i*sizeof(uint5);
    inst.instruction = memory_readinstword(machine->mem, chunk->virtualaddress);

    if ((blockstart = jt_hash_lookup(leaders, i)))
    {
      // chain adjacent blocks together
      pheta_basicblock* prev = chunk->currentblock;

      if (!blockstart->data)
        blockstart->data = pheta_newbasicblock(chunk, i);

      if (!chunk->root)
        chunk->root = blockstart->data;

      // something screwy was going on here
      if (prev && prev->predicate==255)
        pheta_link(prev, ph_AL, chunk->currentblock, 0);

      chunk->currentblock = blockstart->data;
    }

    // ew, this is pretty shaky...
    if (inst.generic.cond==cc_AL || inst.bra.ident==5)
    {
      synced = dispatch(machine, inst, &phet4, chunk);
    }
    else
    {
      pheta_basicblock* prev = chunk->currentblock;
      pheta_basicblock* thisinst;
      pheta_basicblock* rest;

      thisinst = pheta_getbasicblock(chunk, i);

      dispatch(machine, inst, &phet4, chunk);

      rest = pheta_getbasicblock(chunk, i+1);      

      pheta_link(thisinst, ph_AL, rest, 0);
      pheta_link(prev, inst.generic.cond, thisinst, rest);
    }
  }

  // synchronise last block if just left hanging in the air
  if (!synced)
  {
    //pheta_emit(chunk, ph_SYNC);
    fprintf(stderr, "Not synchronised, doing now (block at %p)\n",
      chunk->currentblock);
    pheta_lsync(chunk);
  }

  /* enable instruction after last one known about */
  chunk->currentblock = pheta_getbasicblock(chunk, i);

  tail = pheta_emit(chunk, ph_CONST, base+i*sizeof(uint5));
  pheta_emit(chunk, ph_UKJMP, tail);

  if (chunk->currentblock) pheta_lsync(chunk);

  // this is a memory leak waiting to happen
  jt_hash_nuke(leaders, NULL);

  return chunk;
}

void pheta_link(pheta_basicblock* from, uint5 pred,
                pheta_basicblock* condtrue, pheta_basicblock* condfalse)
{
  assert((pred&31)==pred);
  from->predicate = pred;
  from->trueblk = condtrue;
  from->falseblk = condfalse;
}

/*
static void emitbyte(pheta_basicblock* block, uint5* written, uint3 byte)
{
  if (++block->length == block->size)
    block->base = realloc(block->base, block->size*=2);

  block->base[block->length-1] = byte;
  (*written)++;
}
*/

// sometimes (ie complicated shifts) we need to use the same output
// for more than one ph2 instruction. This is the nicest solution I
// can think of for doing this, but it's *horrible* nonetheless.
// !!! (not used any more)
/*
void pheta_forceoutput(pheta_chunk* chunk, uint5 reg)
{
  chunk->force = reg;
}

// !!! also not used
void pheta_unforceoutput(pheta_chunk* chunk)
{
  chunk->force = -1;
}
*/

uint5 pheta_emit(pheta_chunk* chunk, pheta_opcode opcode, ...)
{
  va_list ap;
  uint5 dest=0;
  pheta_instr* instr;
  jt_clist* item;
  pheta_basicblock* block = chunk->currentblock;
  
  item = jt_clist_append(block->base);
  instr = item->data = jt_new(pheta_instr);
  
  instr->opcode = opcode;
  
  va_start(ap, opcode);
  switch (opcode)
  {
    case ph_CONST:
    instr->data.con.dest = dest = chunk->tempno++;
    instr->data.con.imm = va_arg(ap, uint5);
    break;
    
    case ph_CONSTB:
    {
      uint5 word = va_arg(ap, uint5);
      assert(word<0x100);
      instr->data.con.dest = dest = chunk->tempno++;
      instr->data.con.imm = word;
    }
    break;
    
    case ph_FETCH:
    instr->data.op.dest = dest = chunk->tempno++;
    instr->data.op.src1 = va_arg(ap, uint5);
    break;
    
    case ph_COMMIT:
    case ph_ASSOC:
    instr->data.op.dest = va_arg(ap, uint5);
    instr->data.op.src1 = va_arg(ap, uint5);
    break;

    case ph_SPILL:
    case ph_UKJMP:
    instr->data.op.src1 = va_arg(ap, uint5);
    break;

    case ph_RELOAD:
    instr->data.op.dest = va_arg(ap, uint5);
    break;
    
    case ph_FEXPECT:
    case ph_FENSURE:
    case ph_NFEXPECT:
    case ph_NFENSURE:
    instr->data.flag.need = va_arg(ap, uint5);
    break;

    case ph_FCOMMIT:
    case ph_NFCOMMIT:
    instr->data.flag.have = va_arg(ap, uint5);
    instr->data.flag.need = va_arg(ap, uint5);
    instr->data.flag.pred = va_arg(ap, uint5);
    break;

/*    case ph_SETPRED:
    case ph_NSETPRED:
    emitbyte(block, &written, (dest = chunk->predno++));
    emitbyte(block, &written, va_arg(ap, uint5));
    break;*/
    
    case ph_XJMP:
    dest = (uint5) &instr->data.imm;  // for patchbacking
    instr->data.imm = va_arg(ap, uint5);
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
    instr->data.op.dest = dest = chunk->tempno++;
    instr->data.op.src1 = va_arg(ap, uint5);
    instr->data.op.src2 = va_arg(ap, uint5);
    break;
    
    case ph_RRX:
    case ph_RLX:
    case ph_MOV:
    case ph_NOT:
    case ph_LDW:
    case ph_LDB:
    instr->data.op.dest = dest = chunk->tempno++;
    instr->data.op.src1 = va_arg(ap, uint5);
    break;
    
    case ph_FWRITE:
    instr->data.op.dest = va_arg(ap, uint5);
    instr->data.op.src1 = va_arg(ap, uint5);
    break;
    
    case ph_TEQ:
    case ph_TST:
    case ph_CMP:
    case ph_CMN:
    case ph_STW:
    case ph_STB:
    instr->data.op.src1 = va_arg(ap, uint5);
    instr->data.op.src2 = va_arg(ap, uint5);
    break;
    
    case ph_STATE:
    case ph_PHI:
    instr->data.imm = va_arg(ap, uint5);
    break;

    case ph_CAJMP:
    case ph_CAJMP26F:
    instr->data.op.dest = dest = chunk->tempno++;
    instr->data.op.src1 = va_arg(ap, uint5);
    break;

    // this is a "#line" equivalent directive sort of
    case ph_ADDRESS:
    instr->data.imm = va_arg(ap, uint5);
    break;
    
    default:
    break;
  }
  va_end(ap);
  
 /* blk->length++;*/
    
  return dest;
}

// return the intermediate registers used by an instruction
void pheta_getused(pheta_instr* instr, uint5* numdest, uint5 dest[],
  uint5* numsrc, uint5 src[])
{
  *numdest = 0;
  *numsrc = 0;
  switch (instr->opcode)
  {
    case ph_CONST:
    {
      dest[(*numdest)++] = instr->data.con.dest;
    }
    break;

    case ph_CONSTB:
    {
      dest[(*numdest)++] = instr->data.con.dest;
    }
    break;

    case ph_FETCH:
    {
      dest[(*numdest)++] = instr->data.op.dest;
    }
    break;
    
    case ph_COMMIT:
    {
      src[(*numsrc)++] = instr->data.op.src1;
    }
    break;

    case ph_SPILL:
    case ph_UKJMP:
    {
      src[(*numsrc)++] = instr->data.op.src1;
    }
    break;

    case ph_RELOAD:
    {
      dest[(*numdest)++] = instr->data.op.dest;
    }
    break;

    case ph_FWRITE:
    {
      src[(*numsrc)++] = instr->data.op.src1;
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
      dest[(*numdest)++] = instr->data.op.dest;
      src[(*numsrc)++] = instr->data.op.src1;
      src[(*numsrc)++] = instr->data.op.src2;
    }
    break;

    case ph_RRX:
    case ph_RLX:
    case ph_MOV:
    case ph_NOT:
    case ph_LDW:
    case ph_LDB:
    case ph_CAJMP:
    case ph_CAJMP26F:
    {
      dest[(*numdest)++] = instr->data.op.dest;
      src[(*numsrc)++] = instr->data.op.src1;
    }
    break;

    case ph_TEQ:
    case ph_TST:
    case ph_CMP:
    case ph_CMN:
    case ph_STW:
    case ph_STB:
    {
      src[(*numsrc)++] = instr->data.op.src1;
      src[(*numsrc)++] = instr->data.op.src2;
    }
    break;
    
    case ph_ASSOC:
    {
      src[(*numsrc)++] = instr->data.op.src1;
    }
    break;
    
    case ph_FEXPECT:
    case ph_FCOMMIT:
    case ph_SYNC:
    case ph_ADDRESS:
    case ph_UNDEF:
    break;
    
    default:  // !!! things might not be fully handled...
    assert(!"Unhandled getused() case");
    break;
  }
}

void pheta_dfs(pheta_chunk* chunk)
{
  jt_list* scanblock;
  uint5 time = 0;

  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
  {
    pheta_basicblock* blk = (pheta_basicblock*) scanblock->data;
    blk->marker = col_WHITE;
  }

  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
  {
    pheta_basicblock* blk = (pheta_basicblock*)scanblock->data;
    if (blk->marker==col_WHITE) pheta_dfs_visit(blk, &time);
  }
}

void pheta_dfs_visit(pheta_basicblock* blk, uint5* time)
{
  blk->marker = col_GREY;
  blk->discovertime = ++*time;
  if (blk->trueblk && blk->trueblk->marker==col_WHITE)
  {
    blk->trueblk->parent = blk;
    pheta_dfs_visit(blk->trueblk, time);
  }
  if (blk->falseblk && blk->falseblk->marker==col_WHITE)
  {
    blk->falseblk->parent = blk;
    pheta_dfs_visit(blk->falseblk, time);
  }
  blk->marker = col_BLACK;
  blk->finishtime = ++*time;
}

void pheta_scc(pheta_chunk* chunk)
{
  jt_list* scanblock;
  
  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
  {
    pheta_basicblock* blk = (pheta_basicblock*) scanblock->data;
    blk->marker = col_WHITE;
  }
  
  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
  {
    pheta_basicblock* blk = (pheta_basicblock*) scanblock->data;
    if (blk->marker==col_WHITE) pheta_scc_visit(blk);
  }
}

void pheta_scc_visit(pheta_basicblock* blk)
{
  jt_clist* walk;
  jt_clist* decreasing = jt_clist_new(), *scan, *at;
  blk->marker = col_GREY;

  for (walk=blk->predecessor->next; walk->data; walk=walk->next)
  {
    pheta_basicblock* parent = walk->data;
    jt_clist* posn = decreasing;
    for (scan=decreasing->next; scan->data; scan=scan->next)
    {
      pheta_basicblock* here = scan->data;
      if (here->finishtime < parent->finishtime)
      {
        posn = scan;
        break;
      }
    }
    at = jt_clist_append(posn);
    at->data = parent;
  }

  for (walk=decreasing->next; walk->data; walk=walk->next)
  {
    pheta_basicblock* x = walk ? walk->data : 0;
    fprintf(stderr, "%d ", x ? x->finishtime : -1);
  }
  fprintf(stderr, "\n");
  
  for (walk=decreasing->next; walk->data; walk=walk->next)
  {
    pheta_basicblock* parent = walk->data;
    if (parent->marker==col_WHITE)
    {
      parent->scsubgraph = blk;
      pheta_scc_visit(parent);
    }
  }
  blk->marker = col_BLACK;
}

void pheta_predecessor(pheta_chunk* chunk)
{
  jt_list* scanblock;

  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
  {
    pheta_basicblock* blk = (pheta_basicblock*)scanblock->data;
    if (blk->trueblk)
    {
      jt_clist* seek, *insertat = blk->trueblk->predecessor;
      sint5 stime = -1;
      for (seek=blk->trueblk->predecessor->next; seek->data; seek=seek->next)
      {
        pheta_basicblock* sblk = seek->data;
        if (sblk->finishtime == (uint5)stime)
        {
          insertat = 0;
          break;
        }
        if ((sint5)sblk->finishtime > stime) insertat = seek;
      }
      if (insertat)
      {
        jt_clist* item = jt_clist_prepend(insertat);
        item->data = blk;
      }
    }
    if (blk->falseblk)
    {
      jt_clist* seek, *insertat = blk->falseblk->predecessor;
      sint5 stime = -1;
      for (seek=blk->falseblk->predecessor->next; seek->data; seek=seek->next)
      {
        pheta_basicblock* sblk = seek->data;
        if (sblk->finishtime == (uint5)stime)
        {
          insertat = 0;
          break;
        }
        if ((sint5)sblk->finishtime > stime) insertat = seek;
      }
      if (insertat)
      {
        jt_clist* item = jt_clist_prepend(insertat);
        item->data = blk;
      }
    }
  }
}

void pheta_dotprint(pheta_chunk* chunk, char* outfile)
{
  FILE* f = fopen(outfile, "w");
  jt_list* scan;
  
  fprintf(f, "digraph code {\n");
  
  fprintf(f, "  color = red\n");
  
  fprintf(f, "  node [\n");
  fprintf(f, "    shape = record\n");
  fprintf(f, "  ]\n");
  
  fprintf(f, "  edge [\n");
  fprintf(f, "     minlen = 1\n");
  fprintf(f, "  ]\n");
  
  fprintf(f, "  start -> n%p:entry;\n", chunk->root);

  for (scan=chunk->blocks; scan; scan=scan->prev)
  {
    pheta_basicblock* blk = scan->data;
    jt_clist* inst;
    uint5 i;
    extern const char* txtcc[];
    pheta_instr* prev = 0;
    
 /*   fprintf(f, "  node: {\n");
    fprintf(f, "  }\n");*/

    if (blk->srcstart != -1)
    {
      fprintf(f, "  n%p [shape=record, label=\"{<entry>", blk);
    }
    else
    {
      fprintf(f, "  n%p [shape=record, label=\"{{<src>src: %.8x}|<entry>", blk,
        blk->srcstart);
    }

    for (inst=blk->base->next, i=0; inst->data; inst=inst->next, i++)
    {
      pheta_instr* instr = inst->data;
      fprintf(f, "%.4d: ", i);
      phetadism_instruction(f, instr);
      fprintf(f, "\\l");

     // prev = instr;
    }
    
    fprintf(f, "|{<cond> %s%s|<true> true|<false> false}}\"];\n", 
      (blk->predicate&16)?"n":"", txtcc[blk->predicate&15]);

/*    fprintf(f, "  node: {\n");
    fprintf(f, "    title: \"%p:%s%s\"\n", blk,
      (blk->predicate&16)?"native-":"", txtcc[blk->predicate&15]);
    fprintf(f, "    shape: rhomboid\n");
    fprintf(f, "  }\n");

    if (i>0)
    {
      fprintf(f, "  edge: {\n");
      fprintf(f, "    thickness: 4\n");
      fprintf(f, "    sourcename: \"%p:%.4x:", blk, i-1);
      phetadism_instruction(f, prev);
      fprintf(f, "\"\n    targetname: \"%p:%s%s\"\n", blk,
        (blk->predicate&16)?"native-":"", txtcc[blk->predicate&15]);
      fprintf(f, "  }\n");
    }

    fprintf(f, "  }\n");*/

    if (blk->trueblk)
    {
      fprintf(f, "  n%p:true -> n%p:entry;\n", blk, blk->trueblk);
    }

    if (blk->falseblk)
    {
      fprintf(f, "  n%p:false -> n%p:entry;\n", blk, blk->falseblk);
    }

    if (!blk->trueblk && !blk->falseblk)
    {
      fprintf(f, "  n%p:true -> end;\n", blk);
    }

  /*  if (blk->parent)
    {
      fprintf(f, "  backedge: {\n");
      fprintf(f, "    thickness: 4\n");
      fprintf(f, "    sourcename: \"%p\"\n", blk);
      fprintf(f, "    targetname: \"%p\"\n", blk->parent);
      fprintf(f, "    label: \"parent\"\n");
      fprintf(f, "    color: blue\n");
      fprintf(f, "  }\n");
    }*/
    
 /*   for (pred=blk->predecessor->next; pred->data; pred=pred->next)
    {
      pheta_basicblock* target = pred->data;
      fprintf(f, "  backedge: {\n");
      fprintf(f, "    thickness: 4\n");
      fprintf(f, "    sourcename: \"%p\"\n", blk);
      fprintf(f, "    targetname: \"%p\"\n", target);
      fprintf(f, "    label: \"pre\"\n");
      fprintf(f, "    color: green\n");
      fprintf(f, "  }\n");
    }*/
  }
  
  fprintf(f, "}\n");
  fclose(f);
}

uint5 pheta_fixup_flags_inner(pheta_basicblock* blk, uint5 blktag,
  uint5 needpred, uint5 needflag)
{
  jt_clist* walk;
  uint5 succ = 0;
  
  if (needpred==ph_AL || needpred==ph_NV || needpred==255) needpred = -1;
  
  for (walk=blk->base->prev; walk->data; walk=walk->prev)
  {
    pheta_instr* instr = walk->data;
    uint5 opcode;
    
    switch (opcode = instr->opcode)
    {
      case ph_SYNC:
      needflag |= ph_C | ph_V | ph_N | ph_Z;
      break;
      
/*      case ph_FEXPECT:
      case ph_NFEXPECT:
      // these might chop some flags out but it'd only be a minor optimisation
      // in case some better code could be generated without setting flags,
      // eg LEA vs ADD or perhaps some RSB variants. Probably not worth the
      // effort though.
      break;*/
      
      case ph_FCOMMIT:
      {
        uint5 have = instr->data.flag.have;
        uint5 need = instr->data.flag.need;
        uint5 pred = instr->data.flag.pred;
        // flag stuff only for ARM state affecting variant
        need |= (needflag & have);
        needflag &= ~need;
        if (needpred != (uint5)-1 && needpred<16)
        {
          if ((ccflags[needpred] & have) == ccflags[needpred])
          {
            fprintf(stderr, "Fully satisfied predicate flags for %p\n", blk);
            pred |= 1<<needpred;
//            need |= ccflags[needpred];
            needpred = -1;
          }
          else
          {
            fprintf(stderr, "Partially satisfied predicate flags, fix it\n");
            abort();
          }
        }
        instr->data.flag.need = need;
        instr->data.flag.pred = pred;
      }
      break;

      case ph_NFCOMMIT: /* -- handle seperately */
      {
        if (needpred>=16) needpred = -1;
      }
      break;

      case ph_FENSURE:
      {
        uint5 mask = instr->data.flag.need;
        needflag |= mask;
      }
      break;
    
      default:
      // do nothing
      break;
    } // switch (opcode)
  }

  if (needflag != 0 || needpred != (uint5)-1)
  {
    jt_clist* l;
    
    fprintf(stderr, "Trying next level up\n");
    
    succ = 1;

    for (l=blk->predecessor->next; l->data; l=l->next)
    {
      pheta_basicblock* pre = l->data;
      fprintf(stderr, "-- trying %p\n", pre);
      // !!! not thought about this
      if (pre != blk)
        succ &= pheta_fixup_flags_inner(pre, blktag, needpred, needflag);
    }
  }
  else succ = 1;
  
  return succ;
}

void pheta_fixup_flags(pheta_chunk* chunk)
{
  jt_list* scanblock;
  uint5 tag=0;
  
  palloc_clearmarkers(chunk);
  
  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev, tag++)
  {
    pheta_basicblock* blk = (pheta_basicblock*)scanblock->data;
    uint5 succ;
    
    fprintf(stderr, "Fixup block %p\n", blk);
    succ = pheta_fixup_flags_inner(blk, tag, blk->predicate, 0);
    if (!succ)
    {
      fprintf(stderr, "Non-local flags, panic\n");
      abort();
    }
  }
}

void pheta_optimise_transitive_branch(pheta_chunk* chunk)
{
  jt_list* scanblock;
  uint5 change;
  
  do {
    change = 0;
    for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
    {
      pheta_basicblock* blk = scanblock->data;
      if (blk->trueblk)
      {
        if (blk->trueblk->base->next->data==0)
        {
          if (blk->predicate==ph_AL && !blk->falseblk)
          {
            blk->predicate = blk->trueblk->predicate;
            blk->falseblk = blk->trueblk->falseblk;
            blk->trueblk = blk->trueblk->trueblk;
            change = 1;
          }
          else if (blk->predicate==blk->trueblk->predicate && blk!=blk->trueblk)
          {
            blk->trueblk = blk->trueblk->trueblk;
            change = 1;
          }
          else if (blk->predicate==(blk->trueblk->predicate^1))
          {
            blk->trueblk = blk->trueblk->falseblk;
            change = 1;
          }
        }
      }
      
      if (blk->falseblk)
      {
        if (blk->falseblk->base->next->data==0)
        {
          if (blk->predicate==blk->falseblk->predicate && blk!=blk->falseblk)
          {
            blk->falseblk = blk->falseblk->falseblk;
            change = 1;
          }
          else if (blk->predicate==(blk->falseblk->predicate^1))
          {
            blk->falseblk = blk->falseblk->trueblk;
            change = 1;
          }
        }
      }
    }
  } while (change);
}

void pheta_cull_unused_nodes(pheta_chunk* chunk)
{
  jt_list* scanblock, *prev;
  palloc_clearmarkers(chunk);

  chunk->root->marker = col_BLACK;
  
  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
  {
    pheta_basicblock* blk = scanblock->data;
    if (blk->trueblk) blk->trueblk->marker = col_BLACK;
    if (blk->falseblk) blk->falseblk->marker = col_BLACK;
  }
  
  for (scanblock=chunk->blocks; scanblock;)
  {
    pheta_basicblock* blk = scanblock->data;
    prev = scanblock->prev;
    if (blk->marker==col_WHITE)
    {
      pheta_destroybasicblock(blk);
      jt_list_delinkitem(&chunk->blocks, scanblock);
    }
    scanblock = prev;
  }
}

uint5 pheta_lfetch(pheta_chunk* chunk, uint5 regno)
{
  if (regno==ph_R15_FULL || regno==ph_R15_ADDR)
    pheta_emit(chunk, ph_ADDRESS, chunk->virtualaddress);
  
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
  pheta_emit(chunk, ph_ASSOC, regno, tempno);
}

void pheta_lsync(pheta_chunk* chunk)
{
  int i;

  if (!chunk->currentblock)
  { 
    fprintf(stderr, "lsync: no current block?\n");
    return;
  }
  
  for (i=0; i<ph_NUMREG; i++)
  {
    if (chunk->currentblock->lbuf[i] != -1 && chunk->currentblock->dirtybuf[i])
    {
      pheta_emit(chunk, ph_COMMIT, i, chunk->currentblock->lbuf[i]);
      chunk->currentblock->lbuf[i] = -1;
    }
  }
}

void pheta_state(pheta_chunk* chunk)
{
/*  uint5 i;*/
  jt_list* alive = 0;
  
/*  for (i=0; i<ph_NUMREG; i++)
  {
    if (chunk->currentblock->lbuf[i] != -1 && chunk->currentblock->dirtybuf[i])
    {
      pheta_rpair* rpair;
      list_add(&alive);
      rpair = alive->data = cnew(pheta_rpair);
      rpair->ph = i;
      rpair->arm = chunk->currentblock->lbuf[i];
    }
  }*/
  pheta_emit(chunk, ph_STATE, alive);
}

void pheta_cycles(pheta_chunk* chunk, uint5 n)
{
  chunk->currentblock->cycles += n;
}

pheta_basicblock* pheta_getbasicblock(pheta_chunk* chunk, uint5 line)
{
  jt_hashentry* e;
  if ((e = jt_hash_lookup(chunk->leaders, line)))
  {
    if (!e->data)
      e->data = pheta_newbasicblock(chunk, line==-1u ? -1u : 
        chunk->start+line*4);
    else
    {
      pheta_lsync(chunk);
      // !!! not thought about this
      chunk->currentblock = e->data;
    }

    return e->data;
  }
  else
  {
    return pheta_newbasicblock(chunk, line==-1u ? -1u : chunk->start+line*4);
  }
}

int pheta_dp(machineinfo* machine, instructionformat inst, void* chunk)
{
  uint5 temp = inst.dp.operand2;
  uint5 shifttype = (temp>>5)&3;
  uint3 regshift = (temp & 16) ? 1 : 0;
  const uint5 logic = 0xf303;
  int islogic = logic & (1<<inst.dp.opcode);
  uint5 shiftreg;
  uint5 op2, rm = temp&15;
  pheta_basicblock* original = ((pheta_chunk*)chunk)->currentblock;
  uint5 line = 
    (((pheta_chunk*)chunk)->virtualaddress-((pheta_chunk*)chunk)->start)>>2;
  
  op2 = pheta_lfetch(chunk, rm==15 ? ph_R15_FULL : rm);
  
  if (regshift)
  {
    shiftreg = temp>>8;
    shiftreg = pheta_lfetch(chunk, shiftreg);

    pheta_cycles(chunk, 2);
    
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

        pheta_emit(chunk, ph_NFEXPECT, ph_C | ph_V | ph_N | ph_Z);
        pheta_emit(chunk, ph_CMP, shiftreg, pheta_emit(chunk, ph_CONSTB, 31));
        pheta_emit(chunk, ph_NFCOMMIT, ph_C | ph_V | ph_N | ph_Z, 0, bitcc_GT);
//        gtpred = pheta_emit(chunk, ph_NSETPRED, ph_GT);
//        pheta_emit(chunk, ph_NFENSURE, ph_C | ph_V | ph_N | ph_Z);

        if (islogic && inst.dp.s)
        {
          undershiftlimit = pheta_newbasicblock(chunk, -1);
          
          pheta_emit(chunk, ph_FEXPECT, ph_C);
          op2 = pheta_emit(chunk, ph_LSL, op2, shiftreg);
          pheta_emit(chunk, ph_FCOMMIT, ph_C, 0, 0);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          testatshiftlimit = pheta_newbasicblock(chunk, -1);
          
          pheta_emit(chunk, ph_NFEXPECT, ph_C | ph_V | ph_N | ph_Z);
          pheta_emit(chunk, ph_CMP, shiftreg, pheta_emit(chunk, ph_CONSTB, 32));
          pheta_emit(chunk, ph_NFCOMMIT, ph_C | ph_V | ph_N | ph_Z, 0, 
            bitcc_EQ);
//          shlimpred = pheta_emit(chunk, ph_NSETPRED, ph_EQ);
          
          atshiftlimit = pheta_newbasicblock(chunk, -1);
          
          pheta_emit(chunk, ph_FEXPECT, ph_C);
          pheta_emit(chunk, ph_LSR, op2, pheta_emit(chunk, ph_CONSTB, 1));
          pheta_emit(chunk, ph_FCOMMIT, ph_C, 0, 0);
          
          op2 = pheta_emit(chunk, ph_CONSTB, 0);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          overshiftlimit = pheta_newbasicblock(chunk, -1);

          op2 = pheta_emit(chunk, ph_CONSTB, 0);
          pheta_emit(chunk, ph_FWRITE, ph_C, pheta_emit(chunk, ph_CONSTB, 0));
          pheta_dp_guts(machine, inst, chunk, op2);
          
          rest = pheta_getbasicblock(chunk, line+1);
/*          rest = pheta_newbasicblock(chunk, -1);*/
          
          pheta_link(original, ph_GT, testatshiftlimit, undershiftlimit);
          pheta_link(undershiftlimit, ph_AL, rest, 0);
          pheta_link(testatshiftlimit, ph_EQ, atshiftlimit, overshiftlimit);
          pheta_link(atshiftlimit, ph_AL, rest, 0);
          pheta_link(overshiftlimit, ph_AL, rest, 0);
        }
        else  // no flag nonsense
        {
          undershiftlimit = pheta_newbasicblock(chunk, -1);
          
          op2 = pheta_emit(chunk, ph_LSL, op2, shiftreg);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          overshiftlimit = pheta_newbasicblock(chunk, -1);
          
          op2 = pheta_emit(chunk, ph_CONSTB, 0);
          pheta_dp_guts(machine, inst, chunk, op2);

          rest = pheta_getbasicblock(chunk, line+1);          
/*          rest = pheta_newbasicblock(chunk, 
            ((pheta_chunk*)chunk)->virtualaddress+4);*/
          
          pheta_link(original, ph_NAT|ph_GT, overshiftlimit, undershiftlimit);
          pheta_link(undershiftlimit, ph_AL, rest, 0);
          pheta_link(overshiftlimit, ph_AL, rest, 0);
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
        pheta_emit(chunk, ph_NFCOMMIT, ph_C | ph_V | ph_N | ph_Z, 0, bitcc_GT);
//        pheta_emit(chunk, ph_NFENSURE, ph_C | ph_V | ph_N | ph_Z);

        if (islogic && inst.dp.s)
        {
          undershiftlimit = pheta_newbasicblock(chunk, -1);
          
          pheta_emit(chunk, ph_FEXPECT, ph_C);
          op2 = pheta_emit(chunk, ph_LSR, op2, shiftreg);
          pheta_emit(chunk, ph_FCOMMIT, ph_C, 0, 0);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          testatshiftlimit = pheta_newbasicblock(chunk, -1);
          
          pheta_emit(chunk, ph_NFEXPECT, ph_C | ph_V | ph_N | ph_Z);
          pheta_emit(chunk, ph_CMP, shiftreg, pheta_emit(chunk, ph_CONSTB, 32));
          pheta_emit(chunk, ph_NFCOMMIT, ph_C | ph_V | ph_N | ph_Z, 0, 
            bitcc_EQ);
          
          atshiftlimit = pheta_newbasicblock(chunk, -1);
          
          pheta_emit(chunk, ph_FEXPECT, ph_C);
          pheta_emit(chunk, ph_LSL, op2, pheta_emit(chunk, ph_CONSTB, 1));
          pheta_emit(chunk, ph_FCOMMIT, ph_C, 0, 0);
          
          op2 = pheta_emit(chunk, ph_CONSTB, 0);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          overshiftlimit = pheta_newbasicblock(chunk, -1);

          op2 = pheta_emit(chunk, ph_CONSTB, 0);
          pheta_emit(chunk, ph_FWRITE, ph_C, pheta_emit(chunk, ph_CONSTB, 0));
          pheta_dp_guts(machine, inst, chunk, op2);
          
          rest = pheta_getbasicblock(chunk, line+1);
          
          pheta_link(original, ph_NAT|ph_GT, testatshiftlimit, undershiftlimit);
          pheta_link(undershiftlimit, ph_AL, rest, 0);
          pheta_link(testatshiftlimit, ph_NAT|ph_EQ, atshiftlimit, 
                     overshiftlimit);
          pheta_link(atshiftlimit, ph_AL, rest, 0);
          pheta_link(overshiftlimit, ph_AL, rest, 0);
        }
        else  // no flag nonsense
        {
          undershiftlimit = pheta_newbasicblock(chunk, -1);
          
          op2 = pheta_emit(chunk, ph_LSR, op2, shiftreg);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          overshiftlimit = pheta_newbasicblock(chunk, -1);
          
          op2 = pheta_emit(chunk, ph_CONSTB, 0);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          rest = pheta_getbasicblock(chunk, line+1);
          
          pheta_link(original, ph_NAT|ph_GT, overshiftlimit, undershiftlimit);
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
        pheta_emit(chunk, ph_NFCOMMIT, ph_C | ph_V | ph_N | ph_Z, 0, bitcc_GT);
//        pheta_emit(chunk, ph_NFENSURE, ph_C | ph_V | ph_N | ph_Z);

        if (islogic && inst.dp.s)
        {
          undershiftlimit = pheta_newbasicblock(chunk, -1);
          
          pheta_emit(chunk, ph_FEXPECT, ph_C);
          op2 = pheta_emit(chunk, ph_ASR, op2, shiftreg);
          pheta_emit(chunk, ph_FCOMMIT, ph_C, 0, 0);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          overshiftlimit = pheta_newbasicblock(chunk, -1);
          
          op2 = pheta_emit(chunk, ph_ASR, op2, shiftlimit);
          pheta_emit(chunk, ph_FEXPECT, ph_C);
          pheta_emit(chunk, ph_ASR, op2, pheta_emit(chunk, ph_CONSTB, 1));
          pheta_emit(chunk, ph_FCOMMIT, ph_C, 0, 0);
          pheta_dp_guts(machine, inst, chunk, op2);
          
          rest = pheta_getbasicblock(chunk, line+1);
          
          pheta_link(original, ph_NAT|ph_GT, overshiftlimit, undershiftlimit);
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
          
          rest = pheta_getbasicblock(chunk, line+1);
        }

        pheta_link(original, ph_NAT|ph_GT, overshiftlimit, undershiftlimit);
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
          pheta_emit(chunk, ph_NFCOMMIT, ph_Z | ph_N, 0, bitcc_EQ);
//          pheta_emit(chunk, ph_NFENSURE, ph_Z | ph_N);

          if (islogic && inst.dp.s)
          {
            int op2b;
            notzero = pheta_newbasicblock(chunk, -1);
            
            pheta_emit(chunk, ph_FEXPECT, ph_C);
            op2b = pheta_emit(chunk, ph_ROR, op2, shiftreg);
            pheta_emit(chunk, ph_FCOMMIT, ph_C, 0, 0);
            pheta_dp_guts(machine, inst, chunk, op2b);
            
            zero = pheta_newbasicblock(chunk, -1);
            
            pheta_dp_guts(machine, inst, chunk, op2);
            
            rest = pheta_getbasicblock(chunk, -1);
            
            pheta_link(original, ph_NAT|ph_EQ, zero, notzero);
            pheta_link(notzero, ph_AL, rest, 0);
            pheta_link(zero, ph_AL, rest, 0);
          }
          else  // no flags
          {
            op2 = pheta_emit(chunk, ph_ROR, shiftreg);
            return pheta_dp_guts(machine, inst, chunk, op2);
          }
        }
        else
        {
          shiftreg = pheta_emit(chunk, ph_AND, shiftreg, shiftmask);
          op2 = pheta_emit(chunk, ph_ROR, op2, shiftreg);
          return pheta_dp_guts(machine, inst, chunk, op2);
        }
      }
      break;
    }  // switch (shifttype)
  }
  else  // shift by immediate
  {
    int amount = temp >> 7;
    int amountreg = -1;

    pheta_cycles(chunk, 1);
    
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
            pheta_emit(chunk, ph_FCOMMIT, ph_C, 0, 0);
          }
          else
            op2 = pheta_emit(chunk, ph_LSL, op2, amountreg);
        }
        return pheta_dp_guts(machine, inst, chunk, op2);
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
            pheta_emit(chunk, ph_FCOMMIT, ph_C, 0, 0);
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
            pheta_emit(chunk, ph_FCOMMIT, ph_C, 0, 0);
          }
          op2 = pheta_emit(chunk, ph_CONSTB, 0);
        }
        return pheta_dp_guts(machine, inst, chunk, op2);
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
            pheta_emit(chunk, ph_FCOMMIT, ph_C, 0, 0);
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
            pheta_emit(chunk, ph_FCOMMIT, ph_C, 0, 0);
          }
        }
        return pheta_dp_guts(machine, inst, chunk, op2);
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
            pheta_emit(chunk, ph_FCOMMIT, ph_C, 0, 0);
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
            pheta_emit(chunk, ph_FCOMMIT, ph_C, 0, 0);
          }
          else
          {
            op2 = pheta_emit(chunk, ph_RRX, op2,
                             pheta_emit(chunk, ph_CONSTB, 1));
          }
        }
        return pheta_dp_guts(machine, inst, chunk, op2);
      }
      break;
    }  // shift type
  }  // register/immediate
  // should be unreachable
  assert(!"Code shouldn't reach here");
  return 0;
}

int pheta_dp_imm(machineinfo* machine, instructionformat inst, void* chunk)
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
  
  return pheta_dp_guts(machine, inst, chunk, op2);
}

int pheta_dp_guts(machineinfo* machine, instructionformat inst,
                  pheta_chunk* chunk, uint5 op2)
{
  sint5 dest = -1;
  sint5 op1 = -1;
  const uint5 needop1mask = 0x5fff;
  uint5 setflags = (inst.dp.s && inst.dp.rd!=15);
  
  IGNORE(machine);
  
  if (needop1mask & (1<<inst.dp.opcode))
    op1 = pheta_lfetch(chunk, (inst.dp.rn==15) ? ph_R15_ADDR : inst.dp.rn);
  
  switch (inst.dp.opcode)
  {
    case dp_AND:
    {
      if (setflags)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_AND, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_N | ph_Z, 0, 0);
      }
      else
      {
        dest = pheta_emit(chunk, ph_AND, op1, op2);
      }
    }
    break;

    case dp_EOR:
    {
      if (setflags)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_EOR, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_N | ph_Z, 0, 0);
      }
      else
      {
        dest = pheta_emit(chunk, ph_EOR, op1, op2);
      }
    }
    break;

    case dp_SUB:
    {
      if (setflags)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_C | ph_V | ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_SUB, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_C | ph_V | ph_N | ph_Z, 0, 0);
      }
      else
      {
        dest = pheta_emit(chunk, ph_SUB, op1, op2);
      }
    }
    break;

    case dp_RSB:
    {
      if (setflags)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_C | ph_V | ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_SUB, op2, op1);
        pheta_emit(chunk, ph_FCOMMIT, ph_C | ph_V | ph_N | ph_Z, 0, 0);
      }
      else
      {
        dest = pheta_emit(chunk, ph_SUB, op2, op1);
      }
    }
    break;

    case dp_ADD:
    {
      if (setflags)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_C | ph_V | ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_ADD, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_C | ph_V | ph_N | ph_Z, 0, 0);
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
      if (setflags)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_C | ph_V | ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_ADC, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_C | ph_V | ph_N | ph_Z, 0, 0);
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
      if (setflags)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_C | ph_V | ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_SBC, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_C | ph_V | ph_N | ph_Z, 0, 0);
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
      if (setflags)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_C | ph_V | ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_SBC, op2, op1);
        pheta_emit(chunk, ph_FCOMMIT, ph_C | ph_V | ph_N | ph_Z, 0, 0);
      }
      else
      {
        dest = pheta_emit(chunk, ph_SBC, op2, op1);
      }
    }
    break;
    
    case dp_TST:
    {
      if (setflags)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_N | ph_Z);
        pheta_emit(chunk, ph_TST, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_N | ph_Z, 0, 0);
      }
      else
        fprintf(stderr, "Warning: TST with clear S flag!\n");
    }
    break;
    
    case dp_TEQ:
    {
      if (setflags)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_N | ph_Z);
        pheta_emit(chunk, ph_TEQ, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_N | ph_Z, 0, 0);
      }
      else
        fprintf(stderr, "Warning: TEQ with clear S flag!\n");
    }
    break;
    
    case dp_CMP:
    {
      if (setflags)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_C | ph_V | ph_N | ph_Z);
        pheta_emit(chunk, ph_CMP, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_C | ph_V | ph_N | ph_Z, 0, 0);
      }
      else
        fprintf(stderr, "Warning: CMP with clear S flag!\n");
    }
    break;
    
    case dp_CMN:
    {
      if (setflags)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_C | ph_V | ph_N | ph_Z);
        pheta_emit(chunk, ph_CMN, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_C | ph_V | ph_N | ph_Z, 0, 0);
      }
      else
        fprintf(stderr, "Warning: CMN with clear S flag!\n");
    }
    break;
    
    case dp_ORR:
    {
      if (setflags)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_OR, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_N | ph_Z, 0, 0);
      }
      else
      {
        dest = pheta_emit(chunk, ph_OR, op1, op2);
      }
    }
    break;
    
    case dp_MOV:
    {
      if (setflags)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_MOV, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_N | ph_Z, 0, 0);
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
      if (setflags)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_AND, op1, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_N | ph_Z, 0, 0);
      }
      else
      {
        dest = pheta_emit(chunk, ph_AND, op1, op2);
      }
    }
    break;
    
    case dp_MVN:
    {
      if (setflags)
      {
        pheta_emit(chunk, ph_FEXPECT, ph_N | ph_Z);
        dest = pheta_emit(chunk, ph_NOT, op2);
        pheta_emit(chunk, ph_FCOMMIT, ph_N | ph_Z, 0, 0);
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
    uint5 destr = inst.dp.rd;
    pheta_lcommit(chunk, destr, dest);
    if (destr==15)
    {
      pheta_emit(chunk, ph_SYNC);
      destr = inst.dp.s ? ph_R15_FULL : ph_R15_ADDR;

      // sometimes this should be a RTS
      dest = pheta_emit(chunk, inst.dp.s ? ph_CAJMP26F : ph_CAJMP, dest);
      // PC modified, return 1
      return 1;
    }
  }
  return 0;
}

int pheta_bra(machineinfo* machine, instructionformat inst, void* data)
{
  sint5 offset = ((sint5)(inst.bra.offset<<8))>>8;
  pheta_chunk* chunk = (pheta_chunk*)data;
  uint5 index = ((chunk->virtualaddress-chunk->start)>>2);
  sint5 dest = index+offset+2;
  uint5 next = index+1;

  IGNORE(machine);
  
  pheta_cycles(chunk, 2);  // err, or maybe 3
  
  if (inst.bra.l)
  {
    uint5 destpc = pheta_emit(chunk, ph_CONST, chunk->virtualaddress-4);
    pheta_lcommit(chunk, ph_R14, destpc);
  }
  
  if (dest>=0 && dest<(sint5)chunk->length)
  {
    pheta_basicblock* prevblk = chunk->currentblock;
    jt_hashentry* taken = jt_hash_lookup(chunk->leaders, dest);
    jt_hashentry* nottaken = jt_hash_lookup(chunk->leaders, next);
    void* nt;

    fprintf(stderr, "Branch: taken=%p, nottaken=%p\n", taken, nottaken);
    fprintf(stderr, "taken->data=%p, nottaken->data=%p\n", taken->data,
      nottaken->data);

/*    if (!taken)
      taken = hash_insert(chunk->leaders, dest);*/

    if (!taken->data)
      taken->data = pheta_newbasicblock(chunk, -1);
    
  /*  if (!nottaken)
      nottaken = hash_insert(chunk->leaders, next);*/

    if (!nottaken->data)
      nottaken->data = pheta_newbasicblock(chunk, -1);
    
    // eeeeew!!!
    chunk->currentblock = 0;

//  pheta_emit(chunk, ph_FEXPECT, ph_C|ph_V|ph_N|ph_Z);
/*    fprintf(stderr, "linking %x to %x,%x with %d\n", chunk->currentblock,
      taken->data, nottaken->data, inst.bra.cond);*/
//    blkpred = pheta_emit(chunk, ph_SETPRED, inst.bra.cond);

    nt = nottaken->data;

  /*  if (inst.bra.cond==ph_AL || inst.bra.cond==ph_NV)
      nt = 0;*/

    pheta_link(prevblk, inst.bra.cond, taken->data, nt);
  }
  else  // going outside - ack!
  {
    uint5 pctemp;
    pheta_basicblock* previous = chunk->currentblock;
    pheta_basicblock* nottaken;
    pheta_basicblock* exitchunk = pheta_newbasicblock(chunk, -1);
    pheta_lsync(chunk);
    // deal with xjmp later
    pctemp = pheta_emit(chunk, ph_CONST,
      (uint5)chunk->virtualaddress + offset*4 + 8);
/*    fprintf(stderr, "PC TEMP=%d\n", pctemp);
    exit(0);*/
    pheta_emit(chunk, ph_UKJMP, pctemp);
    nottaken = pheta_getbasicblock(chunk, next);

 /*   if (inst.bra.cond==ph_AL || inst.bra.cond==ph_NV)
      nottaken = 0;*/

    pheta_link(previous, inst.bra.cond, exitchunk, nottaken);
  }
  // we haven't necessarily set PC & synced, so return 0
  return 0;
}

int pheta_mul(machineinfo* machine, instructionformat inst, void* chunk)
{
  uint5 op1 = pheta_lfetch(chunk, inst.mul.rm);
  uint5 op2 = pheta_lfetch(chunk, inst.mul.rs);
  uint5 dest;

  IGNORE(machine);

  if (inst.mul.a)  // accumulate
  {
    uint5 temp = pheta_emit(chunk, ph_MUL, op1, op2);
    if (inst.mul.s)
    {
      pheta_emit(chunk, ph_FEXPECT, ph_N | ph_Z);
      dest = pheta_emit(chunk, ph_ADD, temp,
                        pheta_lfetch(chunk, inst.mul.rn));
      pheta_emit(chunk, ph_FCOMMIT, ph_N | ph_Z, 0, 0);
    }
    else
    {
      dest = pheta_emit(chunk, ph_ADD, temp,
                        pheta_lfetch(chunk, inst.mul.rn));
    }
    pheta_cycles(chunk, 4);  // possibly average-case ish
  }
  else
  {
    if (inst.mul.s)
    {
      pheta_emit(chunk, ph_FEXPECT, ph_N | ph_Z);
      dest = pheta_emit(chunk, ph_MUL, op1, op2);
      pheta_emit(chunk, ph_FCOMMIT, ph_N | ph_Z, 0, 0);
    }
    else
    {
      dest = pheta_emit(chunk, ph_MUL, op1, op2);
    }
    pheta_cycles(chunk, 3);
  }

  pheta_lcommit(chunk, inst.mul.rd, dest);
  // never sets PC, return 0
  return 0;
}

int pheta_mull(machineinfo* machine, instructionformat inst, void* chunk)
{
  IGNORE(machine);
  IGNORE(inst);
  IGNORE(chunk);
  fprintf(stderr, "Long mul not yet implemented in recompiler\n");
  abort();
  return 0;
}

int pheta_sdt(machineinfo* machine, instructionformat inst, void* chunk)
{
  int basereg = pheta_lfetch(chunk, inst.sdt.rn);
  sint5 offsetreg = -1;  // -1 for zero/no offset
  uint5 setpc = 0;
  uint5 loadreg = 0;

  IGNORE(machine);
  
  pheta_lsync(chunk);
  
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
    if (inst.sdt.offset != 0)
    {
      if (inst.sdt.u)
        offsetreg = pheta_emit(chunk, ph_CONST, inst.sdt.offset);
      else
        offsetreg = pheta_emit(chunk, ph_CONST, -inst.sdt.offset);
    }
  }
  
  if (inst.sdt.p && offsetreg != -1)  // pre-indexed if 1
    basereg = pheta_emit(chunk, ph_ADD, basereg, offsetreg);

/*  pheta_state(chunk);*/
  
  if (inst.sdt.l)  // load
  {
/*    uint5 destr = inst.sdt.rd==15 ? ph_R15_ADDR : inst.sdt.rd;*/
    if (inst.sdt.b)  // it's a byte!
    {
      loadreg = pheta_emit(chunk, ph_LDB, basereg);
    }
    else  // it's a word!
    {
      loadreg = pheta_emit(chunk, ph_LDW, basereg);
    }
    pheta_lcommit(chunk, inst.sdt.rd, loadreg);
    if (inst.sdt.rd==15) setpc=1;
    pheta_cycles(chunk, 3);
  }
  else  // save (pc+12 issue!)
  {
    uint5 srcr = inst.sdt.rd==15 ? ph_R15_ADDR : inst.sdt.rd;
    if (inst.sdt.b) // byte
    {
      int store = pheta_lfetch(chunk, srcr);
      if (inst.sdt.rd==15)
      {
        store = pheta_emit(chunk, ph_ADD, chunk,
                           pheta_emit(chunk, ph_CONSTB, 4));
      }
      pheta_emit(chunk, ph_STB, store, basereg);
    }
    else  // word
    {
      int store = pheta_lfetch(chunk, srcr);
      if (inst.sdt.rd==15)
      {
        store = pheta_emit(chunk, ph_ADD, chunk,
                           pheta_emit(chunk, ph_CONSTB, 4));
      }
      pheta_emit(chunk, ph_STW, store, basereg);
    }
    pheta_cycles(chunk, 2);
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

  // !!! exception semantics? !!!
  if (setpc)
  {
    // use an extra cycle for pipeline bubble or something
    pheta_cycles(chunk, 1);
    loadreg = pheta_emit(chunk, ph_CAJMP26F, loadreg);
    pheta_emit(chunk, ph_SYNC);
    pheta_lcommit(chunk, ph_R15_FULL, loadreg);
    return 1;
  }
  return 0;
}

int pheta_sdth(machineinfo* machine, instructionformat inst, void* chunk)
{
  IGNORE(machine);
  IGNORE(inst);
  IGNORE(chunk);
  fprintf(stderr, "sdth not implemented\n");
  exit(1);
}

int pheta_bdt(machineinfo* machine, instructionformat inst, void* chunk)
{
  uint5 basereg = pheta_lfetch(chunk, inst.bdt.rn);
  uint5 fourreg = pheta_emit(chunk, ph_CONSTB, 4);
  uint5 setpc = 0, loadreg = 0;

  IGNORE(machine);
  
  pheta_lsync(chunk);
  pheta_cycles(chunk, 1);

  if (inst.bdt.w)  // writeback (real ARM does this before the transfer)
  {
    uint5 offsetby;

    if (inst.bdt.u)  // upwards
      offsetby = pheta_emit(chunk, ph_ADD, basereg,
        pheta_emit(chunk, ph_CONSTB, 4*jt_bset_setbits32(inst.bdt.reglist)));
    else
      offsetby = pheta_emit(chunk, ph_SUB, basereg,
        pheta_emit(chunk, ph_CONSTB, 4*jt_bset_setbits32(inst.bdt.reglist)));

    pheta_cycles(chunk, jt_bset_setbits32(inst.bdt.reglist));

    pheta_lcommit(chunk, inst.bdt.rn, offsetby);
  }

/*  pheta_state(chunk);*/

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
          loadreg = pheta_emit(chunk, ph_LDW, basereg);
          if (i==15)
          {
            setpc = 1;
            pheta_cycles(chunk, 1);
          }
          else
            pheta_lcommit(chunk, i, loadreg);
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
          loadreg = pheta_emit(chunk, ph_LDW, basereg);
          if (i==15)
          {
            setpc = 1;
            pheta_cycles(chunk, 1);
          }
          else
            pheta_lcommit(chunk, i, loadreg);
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

  // !!! exception semantics? !!!  
  if (setpc)
  {
    loadreg = pheta_emit(chunk, inst.bdt.s ? ph_CAJMP26F : ph_CAJMP, loadreg);
    pheta_emit(chunk, ph_SYNC);
    pheta_lcommit(chunk, inst.bdt.s ? ph_R15_FULL : ph_R15_ADDR, loadreg);
    return 1;
  }
  return 0;
}

int pheta_swi(machineinfo* machine, instructionformat inst, void* chunk)
{
  IGNORE(machine);
  IGNORE(inst);
  pheta_lsync(chunk);
  pheta_emit(chunk, ph_SYNC);
  pheta_emit(chunk, ph_SWI);
  return 1;
}

int pheta_cdt(machineinfo* machine, instructionformat inst, void* chunk)
{
  IGNORE(machine);
  IGNORE(inst);
  pheta_lsync(chunk);
  pheta_emit(chunk, ph_SYNC);
  pheta_emit(chunk, ph_UNDEF);
  return 1;
}

int pheta_cdo(machineinfo* machine, instructionformat inst, void* chunk)
{
  IGNORE(machine);
  IGNORE(inst);
  pheta_lsync(chunk);
  pheta_emit(chunk, ph_SYNC);
  pheta_emit(chunk, ph_UNDEF);
  return 1;
}

int pheta_crt(machineinfo* machine, instructionformat inst, void* chunk)
{
  IGNORE(machine);
  IGNORE(inst);
  pheta_lsync(chunk);
  pheta_emit(chunk, ph_SYNC);
  pheta_emit(chunk, ph_UNDEF);
  return 1;
}

int pheta_sds(machineinfo* machine, instructionformat inst, void* chunk)
{
  IGNORE(machine);
  IGNORE(inst);
  IGNORE(chunk);
  fprintf(stderr, "Unimplemented instruction: SWP\n");
  exit(1);
  // when it's implemented, it won't usually sync
  return 0;
}

int pheta_und(machineinfo* machine, instructionformat inst, void* chunk)
{
  IGNORE(machine);
  IGNORE(inst);
  pheta_lsync(chunk);
  pheta_emit(chunk, ph_SYNC);
  pheta_emit(chunk, ph_UNDEF);
  return 1;
}
