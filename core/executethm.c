#define ARMTHUMB 1
#define EXECUTEFN(X) X##_thm

#include "core/execute.inc.c"

#undef EXECUTEFN
