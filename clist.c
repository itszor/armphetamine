/*
 *  Circular list handling functions
 */

#include <assert.h>

#include "cnew.h"
#include "defs.h"
#include "clist.h"

clist* clist_new(void)
{
  clist* x = cnew(clist);
  x->data = 0;
  x->prev = x;
  x->next = x;
  return x;
}

clist* clist_append(clist* sentry)
{
  clist* n = cnew(clist), *after;
  n->data = 0;
  
  after = sentry->prev;
  after->next = n;
  n->prev = after;
  n->next = sentry;
  sentry->prev = n;
  
  return n;
}

clist* clist_prepend(clist* sentry)
{
  clist* n = cnew(clist), *before;
  n->data = 0;
  
  before = sentry->next;
  before->prev = n;
  n->next = before;
  n->prev = sentry;
  sentry->next = n;
  
  return n;
}

void clist_delinkitem(clist* entry)
{
  assert(entry->data != 0);
  entry->prev->next = entry->next;
  entry->next->prev = entry->prev;
  free(entry);
}

/* moves item 'from' after item 'to' */
void clist_moveitem(clist* to, clist* from)
{
  /* delink 'from' from previous location */
  from->prev->next = from->next;
  from->next->prev = from->prev;
  /* tie 'from' into new location */
  from->next = to->next;
  from->prev = to;
  /* insert 'from' after 'to' */
  to->next->prev = from;
  to->next = from;
}



