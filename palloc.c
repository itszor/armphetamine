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
  chunk->alloc[i].referenced_by = hash_new(8);
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
        case ph_FETCH:
        {
          uint5 dest = blk->base[i+1];
          uint5 armreg = blk->base[i+2];
          if (chunk->alloc[dest].type == pal_UNSET)
          {
            chunk->alloc[dest].type = pal_RFILE;
            chunk->alloc[dest].info.value = armreg;
          }
        }
        break;
        
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
      newspan = pqueue_insert(&blk->live, startline+i);
      newspan->item = range = cnew(palloc_liverange);
      range->startline = startline+i;
      range->length = 0;
      range->reg = destr[j];
     /* fprintf(stderr, "Line %d: dest reg %d\n", startline+i, destr[j]);*/
    }
    
    for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
    {
      pheta_basicblock* ablk = (pheta_basicblock*) scanblock->data;
      int k;
      for (k=0; k<ablk->live->length; k++)
      {
        pqueueitem* prevspan = ablk->live->data[k];
        palloc_liverange* range = (palloc_liverange*) prevspan->item;
        for (j=0; j<nsrc; j++)
        {
          if (srcr[j]==range->reg)
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

void palloc_linearscan(pheta_chunk* chunk, pheta_basicblock* blk,
                       uint5 startline)
{
  pqueueitem* rstart;
  pqueue* active = pqueue_new();
  const uint5 regno = 6;
  const uint3 regmap[] = {EAX, ECX, EDX, EBX, ESI, EDI};
  uint3 reguse[] = {0, 0, 0, 0, 2, 2, 0, 0};
  uint5 regc = 0;
  
  while ((rstart = pqueue_head(blk->live)))
  {
    pqueueitem* activate, *del;
    palloc_liverange* range = (palloc_liverange*) rstart->item;
    uint5 lineno = range->startline;
    
    pqueue_extract(blk->live);
    
    activate = pqueue_insert(&active, lineno+range->length);
    activate->item = range;
    free(rstart);

    if (regc<regno)
    {
      uint5 s;
      sint5 ireg = -1;
      for (s=0; s<8; s++)
      {
        if (reguse[s]==0)
        {
          reguse[s] = 1;
          ireg = s;
          regc++;
          break;
        }
      }
      chunk->alloc[range->reg].type = pal_IREG;
      chunk->alloc[range->reg].info.ireg.num = ireg;
      range->ireg = ireg;
    }
    else
    {
      palloc_liverange* delrange;
      del = pqueue_extract(active);
      delrange = (palloc_liverange*) del->item;
      chunk->alloc[range->reg].type = pal_IREG;
      chunk->alloc[range->reg].info.ireg.num = 
        chunk->alloc[delrange->reg].info.ireg.num;
      chunk->alloc[delrange->reg].type = pal_RFILE;
      chunk->alloc[delrange->reg].info.value = delrange->reg;
      free(del->item);
      free(del);
    }

    while ((del = pqueue_head(active)))
    {
      palloc_liverange* delrange = (palloc_liverange*) del->item;
      if (delrange->startline+delrange->length < lineno)
      {
        if (reguse[delrange->ireg]==1)
        {
          reguse[delrange->ireg] = 0;
          regc--;
        }
        free(del->item);
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
    fprintf(stderr, "Block %x:\n", blk);
    for (i=0; i<blk->live->length; i++)
    {
      pqueueitem* span = blk->live->data[i];
      palloc_liverange* range = (palloc_liverange*) span->item;
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
  
  for (i=0; i<ph_NUMREG; i++)
  {
    commitplace[i] = -1;
    lookfor[i] = -1;
  }
  
  for (scanblock=chunk->blocks; scanblock; scanblock=scanblock->prev)
  {
    pheta_basicblock* blk = (pheta_basicblock*)scanblock->data;
    uint3* newbase = cnewarray(uint3, blk->length);
    uint5 nlength = 0;

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
      /*  fprintf(stderr, "Found a commit at %d\n", i);*/
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
    }
  }
}
