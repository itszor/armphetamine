#ifndef CLIST_H
#define CLIST_H 1

#include "list.h"

typedef struct clist_t {
  void* data;
  struct clist_t* prev;
  struct clist_t* next;
} clist;

extern clist* clist_new(void);
extern clist* clist_append(clist*);
extern clist* clist_prepend(clist*);
extern clist* clist_delinkitem(clist*);

#endif
