#include "palloc.h"
#include "cnew.h"
#include "hash.h"

void palloc_init(pheta_chunk* chunk)
{
  int i;
  chunk->alloc = cnewarray(palloc_info, chunk->tempno);
  
  for (i=0; i<chunk->tempno; i++)
  {
    chunk->alloc[i] = pal_UNSET;
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
          chunk->alloc[dest].type = pal_CONST;
          chunk->alloc[dest].info.value = word;
        }
        break;
        
        case ph_CONSTB:
        {
          uint5 dest = blk->base[i++];
          uint5 byte = blk->base[i++];
          chunk->alloc[dest].type = pal_CONSTB;
          chunk->alloc[dest].info.value = byte;
        }
        break;
      }
    }
  }
}

// allocate non-orthogonal target registers
void palloc_nonorthog(pheta_chunk* chunk)
{
}

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
      pheta_getused(blk->base, &i, &ndest, dest, &nsrc, src);

      for (j=0; j<nsrc; j++)
        hash_insert(chunk->alloc[src[j]].referenced_by, blk);

      for (j=0; j<ndest; j++)
        hash_insert(chunk->alloc[dest[j]].referenced_by, blk);
    }
  }
}
