#include "defs.h"
#include "bset.h"
#include "cnew.h"

static const uint3 nbit[] =
{
  0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  4, 5, 5, 4, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

// returns number of set bits in a word
// not actually used at the moment, but I feel it should be here ;-)
uint5 bset_setbits(uint5 x)
{
  return nbit[x&0xff] + nbit[(x>>8)&0xff]
	     + nbit[(x>>16)&0xff] + nbit[(x>>24)&0xff];
}

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
