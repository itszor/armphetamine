#include <assert.h>

#include "palloc.h"
#include "cnew.h"
#include "hash.h"
#include "x86asm.h"

void palloc_init(pheta_chunk* chunk)
{
  int i;
  chunk->alloc = cnewarray(palloc_info, chunk->tempno);
  
  for (i=0; i<chunk->tempno; i++)
  {
    chunk->alloc[i].type = pal_UNSET;
  }
/*  chunk->alloc[i].referenced_by = hash_new(8);*/
}

void palloc_free(pheta_chunk* chunk)
{
  free(chunk->alloc);
}

// allocate constant registers
void palloc_constant(pheta_chunk* chunk)
{
  list* scanblock;
  
  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
  {
    pheta_basicblock* blk = (pheta_basicblock*) scanblock->data;
    int i;
    for (i=0; i<blk->length; i++)
    {
      uint5 opcode = blk->base[i];
      switch (opcode)
      {
        case ph_CONST:
        {
          uint5 dest = blk->base[i+1];
          uint5 word = blk->base[i+2];
          word |= blk->base[i+3]<<8;
          word |= blk->base[i+4]<<16;
          word |= blk->base[i+5]<<24;
          chunk->alloc[dest].type = pal_CONST;
          chunk->alloc[dest].info.value = word;
        }
        break;
        
        case ph_CONSTB:
        {
          uint5 dest = blk->base[i+1];
          uint5 byte = blk->base[i+2];
          chunk->alloc[dest].type = pal_CONSTB;
          chunk->alloc[dest].info.value = byte;
        }
        break;
      }
      i += pheta_instlength[opcode];
    }
  }
}

// allocate non-orthogonal target registers (especially x86 specific!)
// (needs to be rewritten to take account of live ranges properly)
void palloc_nonorthog(pheta_chunk* chunk)
{
  list* scanblock;
  
  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
  {
    pheta_basicblock* blk = (pheta_basicblock*) scanblock->data;
    int i;
    for (i=0; i<blk->length; i++)
    {
      uint5 opcode = blk->base[i];
      switch (opcode)
      {
        // these shift operations can only use the 'cl' (ie, ecx) register
        case ph_LSL:
        case ph_LSR:
        case ph_ASR:
        case ph_ROR:
        case ph_ROL:
        {
          uint5 shiftby = blk->base[i+3];
          if (chunk->alloc[shiftby].type == pal_UNSET)
          {
            chunk->alloc[shiftby].type = pal_IREG;
            chunk->alloc[shiftby].info.ireg.num = ECX;
          }
        }
        break;
        
        // these are function calls, so return a value in EAX
        // (there may be others later)
        case ph_LDB:
        case ph_LDW:
        {
          uint5 dest = blk->base[i+1];
          if (chunk->alloc[dest].type == pal_UNSET)
          {
            chunk->alloc[dest].type = pal_IREG;
            chunk->alloc[dest].info.ireg.num = EAX;
          }
        }
        break;
      }
      i += pheta_instlength[opcode];
    }
  }
}

// If a source register in a particular instruction isn't used after that
// particular instruction, alias the instruction's destination register to
// that source register to take advantage of x86's two-address format
void palloc_srcdestalias_inner(pheta_chunk* chunk, pheta_basicblock* blk,
                               uint5 startline)
{
  int i;
    
  for (i=0; i<blk->length; i++)
  {
    uint5 opcode = blk->base[i];
    uint5 nsrc, srcr[ph_MAXSRC], ndest, destr[ph_MAXDEST];
    uint5 thisline = startline+i, j;

    switch (opcode)
    {
      // mov aliasing? hmmmm.
      case ph_MOV:
      {
        uint5 dest = blk->base[1];
        uint5 src = blk->base[2];
        if (chunk->alloc[dest].type==pal_UNSET)
        {
          chunk->alloc[dest].type = pal_ALIAS;
          chunk->alloc[dest].info.value = src;
        }
      }
      break;
    
      default:
      {
        pheta_getused(blk->base, i, &ndest, destr, &nsrc, srcr);

        if (ndest==1 && (nsrc==1 || nsrc==2))
        {
          palloc_liverange* srcspan = chunk->reversetable[srcr[0]];
          fprintf(stderr, "This reg finishes at: %d, this is line %d\n",
            srcspan->startline+srcspan->length, thisline);
          if (srcspan->startline+srcspan->length <= thisline)
          {
            uint5 srctype = chunk->alloc[srcr[0]].type;
            fprintf(stderr, "Can alias %d to %d\n", destr[0], srcspan->reg);
            if (chunk->alloc[destr[0]].type == pal_UNSET &&
                srctype!=pal_CONST && srctype!=pal_CONSTB)
            {
              chunk->alloc[destr[0]].type = pal_ALIAS;
              chunk->alloc[destr[0]].info.value = srcr[0];
            }
          }
        }
      }
      break;
    }
    
    i += pheta_instlength[opcode];
  }
  
  blk->marker = 1;

  if (blk->trueblk && !blk->trueblk->marker)
  {
    palloc_srcdestalias_inner(chunk, blk->trueblk, startline+blk->length);
  }
  
  if (blk->falseblk && !blk->falseblk->marker)
  {
    palloc_srcdestalias_inner(chunk, blk->falseblk, startline+blk->length);
  }
}

void palloc_srcdestalias(pheta_chunk* chunk)
{
  int i;
  list* scanblock;
  pheta_basicblock* blk;
  
  chunk->reversetable = cnewarray(palloc_liverange*, chunk->tempno);
  
  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
  {
    blk = (pheta_basicblock*)scanblock->data;
    for (i=0; i<blk->live->length; i++)
    {
      palloc_liverange* span =(palloc_liverange*)(blk->live->item[i])->data;
      chunk->reversetable[span->reg] = span;
    }
  }
  
  palloc_clearmarkers(chunk);
  
  palloc_srcdestalias_inner(chunk, chunk->root, 0);
  
  free(chunk->reversetable);
}

void palloc_closealias(pheta_chunk* chunk)
{
  uint5 some, i;
  do {
    some = 0;
    for (i=0; i<chunk->tempno; i++)
    {
      if (chunk->alloc[i].type==pal_ALIAS)
      {
        chunk->alloc[i] = chunk->alloc[chunk->alloc[i].info.value];
        some = 1;
      }
    }
  } while (some==1);
}

// allocate all 'fetched' & 'committed' registers to memory locations
// (which might be a silly thing to do)
void palloc_fetchmem(pheta_chunk* chunk)
{
  list* scanblock;
  
  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
  {
    pheta_basicblock* blk = (pheta_basicblock*) scanblock->data;
    int i;
    for (i=0; i<blk->length; i++)
    {
      uint5 opcode = blk->base[i];
      switch (opcode)
      {
/*        case ph_FETCH:
        {
          uint5 dest = blk->base[i+1];
          uint5 armreg = blk->base[i+2];
          if (chunk->alloc[dest].type == pal_UNSET)
          {
            chunk->alloc[dest].type = pal_RFILE;
            chunk->alloc[dest].info.value = armreg;
          }
        }
        break;*/
        
        case ph_COMMIT:
        {
          uint5 armreg = blk->base[i+1];
          uint5 src = blk->base[i+2];
          if (chunk->alloc[src].type == pal_UNSET)
          {
            chunk->alloc[src].type = pal_RFILE;
            chunk->alloc[src].info.value = armreg;
          }
        }
        break;
      }
      i += pheta_instlength[opcode];
    }
  }
}

void palloc_clearmarkers(pheta_chunk* chunk)
{
  list* scanblock;
  
  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
  {
    pheta_basicblock* blk = (pheta_basicblock*) scanblock->data;
    blk->marker = col_WHITE;
  }
}

uint5 palloc_close(pheta_chunk* chunk, uint5 reg)
{
  if (chunk->alloc[reg].type==pal_ALIAS)
    return palloc_close(chunk, chunk->alloc[reg].info.value);
  else
    return reg;
}

void palloc_findspans(pheta_chunk* chunk, pheta_basicblock* blk,
                      uint5 startline)
{
  int i;
    
 /* fprintf(stderr, "Block %x\n", blk);*/
    
  for (i=0; i<blk->length; i++)
  {
    uint5 j, destr[ph_MAXDEST], srcr[ph_MAXSRC], ndest, nsrc;
    uint5 opcode = blk->base[i];
    list* scanblock;

    pheta_getused(blk->base, i, &ndest, destr, &nsrc, srcr);

    for (j=0; j<ndest; j++)
    {
      pqueueitem* newspan;
      palloc_liverange* range;
      newspan = pqueue_insert(blk->live, startline+i);
      newspan->data = range = cnew(palloc_liverange);
      range->startline = startline+i;
      range->length = 0;
      range->reg = palloc_close(chunk, destr[j]);
     /* fprintf(stderr, "Line %d: dest reg %d\n", startline+i, destr[j]);*/
    }
    
    for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
    {
      pheta_basicblock* ablk = (pheta_basicblock*) scanblock->data;
      int k;
      for (k=0; k<ablk->live->length; k++)
      {
        pqueueitem* prevspan = ablk->live->item[k];
        palloc_liverange* range = (palloc_liverange*) prevspan->data;
        for (j=0; j<nsrc; j++)
        {
          if (palloc_close(chunk, srcr[j])==range->reg)
          {
            range->length = (startline+i) - range->startline;
        /*    fprintf(stderr, "Line %d: updating src reg %d\n", startline+i,
              destr[j]);*/
          }
        }
      }
    }

    i += pheta_instlength[opcode];
  }

  // prevent scanning of self!
  blk->marker = 1;
  
  if (blk->trueblk && !blk->trueblk->marker)
  {
    palloc_findspans(chunk, blk->trueblk, startline+blk->length);
  }
  
  if (blk->falseblk && !blk->falseblk->marker)
  {
    palloc_findspans(chunk, blk->falseblk, startline+blk->length);
  }
}

// this doesn't delete the actual container
void palloc_deletespans(pheta_chunk* chunk)
{
  list* scanblock;
  
  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
  {
    pheta_basicblock* blk = (pheta_basicblock*)scanblock->data;
    pqueueitem* del;
    int i;
    
    for (i=0; i<blk->live->length; i++)
    {
      pqueueitem* del = blk->live->item[i];
      free(del->data);
      free(del);
    }
    blk->live->length = 0;
  }
}

void palloc_linearscan(pheta_chunk* chunk, pheta_basicblock* blk,
                       uint5 startline)
{
  pqueueitem* rstart;
  const char* regname[] = {"EAX", "ECX", "EDX", "EBX",
                           "ESP", "EBP", "ESI", "EDI"};
  const uint5 maxreg = 6;
  uint5 regc = 0, i;
  
  while ((rstart = pqueue_extract(blk->live)))
  {
    pqueueitem* activate, *del;
    palloc_liverange* range = (palloc_liverange*) rstart->data;
    uint5 lineno = range->startline;

    if (chunk->alloc[range->reg].type==pal_UNSET)
    {
      fprintf(stderr, "Inserting range for reg %d at %d\n", range->reg, 
        lineno+range->length);

      activate = pqueue_insert(chunk->active, lineno+range->length);
      activate->data = range;
      free(rstart);

      if (chunk->regno<maxreg)
      {
        uint5 s;
        sint5 ireg = -1;
        for (s=0; ireg==-1 && s<8; s++)
        {
          if (chunk->reguse[s]==0)
          {
            chunk->reguse[s] = 1;
            ireg = s;
            chunk->regno++;
          }
        }
        assert(ireg != -1);
        fprintf(stderr, "Allocating reg %d as ireg %s\n", range->reg, 
          regname[ireg]);
        for (s=0; s<8; s++) fprintf(stderr, "%d ", chunk->reguse[s]);
        fprintf(stderr, "\n");
        chunk->alloc[range->reg].type = pal_IREG;
        chunk->alloc[range->reg].info.ireg.num = ireg;
      }
      else  // have to spill a register
      {
        pqueueitem* splititem;
        palloc_liverange* delrange, *splitrange;
        uint5 j;
        sint5 f = -1;
        uint5 end = range->startline + range->length;
 
        // find furthest end-point (linear search makes priority queue utterly
        // useless here, but I think I need the wrong end at this point...)
        for (j=0; j<chunk->active->length; j++)
        {
          palloc_liverange* seek =
            ((palloc_liverange*)chunk->active->item[j]->data);
          uint5 test = seek->startline + seek->length;

          if (chunk->alloc[seek->reg].type==pal_IREG &&
              (test>end || f==-1)) f=j;
          
          fprintf(stderr, "Active %d: reg %d\n", j, seek->reg);
        }
 
        assert(f!=-1);
 
        del = chunk->active->item[f];
        assert(del);
 
        delrange = (palloc_liverange*) del->data;
 
        fprintf(stderr, "Spilling register %d from ireg %s\n",
          delrange->reg, regname[chunk->alloc[delrange->reg].info.ireg.num]);
        fprintf(stderr, "Allocating %s to %d\n", 
          regname[chunk->alloc[delrange->reg].info.ireg.num], range->reg);
 
        chunk->alloc[range->reg].type = pal_IREG;
        chunk->alloc[range->reg].info.ireg.num = 
          chunk->alloc[delrange->reg].info.ireg.num;
 
        if (chunk->alloc[delrange->reg].type!=pal_SPLIT)
        {
          palloc_splitalloc* split;
          chunk->alloc[delrange->reg].type = pal_SPLIT;
          split = chunk->alloc[delrange->reg].info.extra =
            cnew(palloc_splitalloc);

          split->upper = chunk->alloc[delrange->reg];

          split->lower.type = pal_STACK;
          split->lower.info.value = (chunk->stacksize++)*4;
        }
        else
        {
          fprintf(stderr, "Write some code to split again?\n");
          abort();
        }
      }
    }

    while ((del = pqueue_head(chunk->active)))
    {
      palloc_liverange* delrange = (palloc_liverange*) del->data;
      if (delrange->startline+delrange->length < lineno)
      {
        if (chunk->alloc[delrange->reg].type == pal_IREG &&
            chunk->reguse[chunk->alloc[delrange->reg].info.ireg.num]==1)
        {
          int s;
          fprintf(stderr, "Expiring register %d\n", delrange->reg);
          chunk->reguse[chunk->alloc[delrange->reg].info.ireg.num] = 0;
          for (s=0; s<8; s++) fprintf(stderr, "%d ", chunk->reguse[s]);
          fprintf(stderr, "\n");
          chunk->regno--;
        }
        del = pqueue_extract(chunk->active);
        free(del->data);
        free(del);
      }
      else break;
    }
  }

  blk->marker = 1;

  if (blk->trueblk && !blk->trueblk->marker)
  {
    palloc_linearscan(chunk, blk->trueblk, startline+blk->length);
  }
  
  if (blk->falseblk && !blk->falseblk->marker)
  {
    palloc_linearscan(chunk, blk->falseblk, startline+blk->length);
  }
}

void palloc_printspans(pheta_chunk* chunk)
{
  list* scanblock;
  
  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
  {
    pheta_basicblock* blk = (pheta_basicblock*) scanblock->data;
    int i;
    fprintf(stderr, "Block %p:\n", blk);
    for (i=0; i<blk->live->length; i++)
    {
      pqueueitem* span = blk->live->item[i];
      palloc_liverange* range = (palloc_liverange*) span->data;
      fprintf(stderr, "Start line: %d, length: %d, reg: %d\n",
        range->startline, range->length, range->reg);
    }
  }
}

// This is a nasty piece of work, but I haven't slept all night & it's
// the best I can manage.
void palloc_shufflecommit(pheta_chunk* chunk)
{
  list* scanblock;
  sint5 i;
  sint5 lookfor[ph_NUMREG];
  sint5 commitplace[ph_NUMREG];
  
  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
  {
    pheta_basicblock* blk = (pheta_basicblock*)scanblock->data;
    uint3* newbase = cnewarray(uint3, blk->length);
    uint5 nlength = 0;

    for (i=0; i<ph_NUMREG; i++)
    {
      commitplace[i] = -1;
      lookfor[i] = -1;
    }

    for (i=blk->length-1; i>=0; i--)
    {
      uint5 inststart = i-blk->base[i]+1;
      uint5 opcode = blk->base[inststart];
      uint5 destr[ph_MAXDEST], srcr[ph_MAXSRC], ndest, nsrc, j;
      

      if (opcode==ph_COMMIT)
      {
        uint5 armreg = blk->base[inststart+1];
        uint5 src = blk->base[inststart+2];
        lookfor[armreg] = src;
   /* fprintf(stderr, "Found a commit for reg %d at %d\n", armreg, inststart);*/
      }
      
      pheta_getused(blk->base, inststart, &ndest, destr, &nsrc, srcr);
      
      for (j=0; j<ndest; j++)
      {
        int k;
        for (k=0; k<ph_NUMREG; k++)
        {
          if (lookfor[k]==destr[j])
          {
     /*   fprintf(stderr, "Setting commitplace[%d] to %d\n", k, inststart);*/
            commitplace[k] = inststart;
          }
        }
      }

      i = inststart;
    }
    
    for (i=0; i<blk->length; i++)
    {
      uint5 opcode = blk->base[i];
      
      if (opcode!=ph_COMMIT)
      {
        int m;
        memcpy(&newbase[nlength], &blk->base[i], pheta_instlength[opcode]+1);
        nlength += pheta_instlength[opcode]+1;
        for (m=0; m<ph_NUMREG; m++)
        {
          if (i==commitplace[m])
          {
            newbase[nlength++] = ph_COMMIT;
            newbase[nlength++] = m;
            newbase[nlength++] = lookfor[m];
            newbase[nlength++] = 4;
          }
        }
      }
      
      i += pheta_instlength[opcode];
    }
    
    free(blk->base);
    blk->base = newbase;
  }
}

// this has gone stale
/*
void palloc_refby(pheta_chunk* chunk)
{
  list* scanblock;
  
  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
  {
    pheta_basicblock* blk = (pheta_basicblock*) scanblock->data;
    int i;
    uint5 dest[ph_MAXDEST], src[ph_MAXSRC], ndest, nsrc;
    
    for (i=0; i<blk->length; i++)
    {
      int j;
      pheta_getused(blk->base, i, &ndest, dest, &nsrc, src);

      for (j=0; j<nsrc; j++)
        hash_insert(chunk->alloc[src[j]].referenced_by, (uint5)blk);

      for (j=0; j<ndest; j++)
        hash_insert(chunk->alloc[dest[j]].referenced_by, (uint5)blk);
    }
  }
}
*/

void palloc_print(pheta_chunk* chunk)
{
  const char* regname[] = {"EAX", "ECX", "EDX", "EBX",
                           "ESP", "EBP", "ESI", "EDI"};
  int i;
  for (i=0; i<chunk->tempno; i++)
  {
    palloc_info* a = &chunk->alloc[i];
    switch (a->type)
    {
      case pal_UNSET:
      {
        fprintf(stderr, "%3d: Unset\n", i);
      }
      break;
      
      case pal_CONSTB:
      {
        fprintf(stderr, "%3d: Constant byte %.2x\n", i, a->info.value);
      }
      break;
      
      case pal_CONST:
      {
        fprintf(stderr, "%3d: Constant word %.8x\n", i, a->info.value);
      }
      break;
      
      case pal_RFILE:
      {
        fprintf(stderr, "%3d: At %d in register file\n", i, a->info.value);
      }
      break;
      
      case pal_IREG:
      {
        fprintf(stderr, "%3d: x86 register %s\n", i, regname[a->info.ireg.num]);
      }
      break;
      
      case pal_ALIAS:
      {
        fprintf(stderr, "%3d: Aliased to %d\n", i, a->info.value);
      }
      break;
      
      case pal_SPLIT:
      {
        fprintf(stderr, "%3d: Split\n", i);
      }
      break;
      
      case pal_STACK:
      {
        fprintf(stderr, "%3d: On stack, location %d\n", i, a->info.value);
      }
      break;
    }
  }
}
