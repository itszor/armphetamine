#ifndef BSET_H
#define BSET_H 1

#include "defs.h"

typedef struct {
  uint5* bits;
  uint5 length;
} bset_info;

#define bset_SET(B,N) ((B)->bits[(N)>>5] |= (1<<((N)&31)))
#define bset_CLEAR(B,N) ((B)->bits[(N)>>5] &= ~(1<<((N)&31)))
#define bset_TEST(B,N) ((B)->bits[(N)>>5] & (1<<((N)&31)))

extern void bset_union(bset_info* dest, bset_info* src1, bset_info* src2);
extern void bset_intersection(bset_info* dest, bset_info* src1,
                              bset_info* src2);
extern void bset_not(bset_info* dest, bset_info* src);
extern void bset_difference(bset_info* dest, bset_info* src1, bset_info* src2);

#endif
