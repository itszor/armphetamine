#include "defs.h"
#include "bset.h"
#include "cnew.h"

bset_info* bset_new(uint5 length)
{
  bset_info* bset = cnew(bset_info);
  uint5 i, words = (length+31) >> 5;
  
  bset->bits = cnewarray(uint5, words);
  bset->length = length;
  
  for (i=0; i<words; i++) bset->bits[i] = 0;
  
  return bset;
}

void bset_delete(bset_info* bset)
{
  free(bset->bits);
  free(bset);
}

void bset_union(bset_info* dest, bset_info* src1, bset_info* src2)
{
  uint5 i, words = (src1->length+31) >> 5;
  
  for (i=0; i<words; i++)
    dest->bits[i] = src1->bits[i] | src2->bits[i];
}

void bset_intersection(bset_info* dest, bset_info* src1, bset_info* src2)
{
  uint5 i, words = (src1->length+31) >> 5;
  
  for (i=0; i<words; i++)
    dest->bits[i] = src1->bits[i] & src2->bits[i];
}

void bset_not(bset_info* dest, bset_info* src)
{
  uint5 i, words = (src->length+31) >> 5;
  
  for (i=0; i<words; i++)
    dest->bits[i] = ~src->bits[i];
}

void bset_difference(bset_info* dest, bset_info* src1, bset_info* src2)
{
  bset_not(dest, src2);
  bset_intersection(dest, src1, dest);
}
