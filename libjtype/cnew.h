/*
**    Name: allocv.h
**
**    Date: Mon Jul  6 17:26:58 1998
**
*/

#ifndef C3D_ALLOCV_H
#define C3D_ALLOCV_H 1

#include <stdlib.h>

#define cnew(n) safemalloc(sizeof(n))
#define cnewarray(n, s) safecalloc(sizeof(n), s)

extern void* safemalloc(size_t bytes);
extern void* safecalloc(size_t bytes, size_t num);

#endif
