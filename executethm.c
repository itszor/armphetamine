#define ARMTHUMB 1
#define EXECUTEFN(X) X##_thm

#include "execute.inc.c"

#undef EXECUTEFN
