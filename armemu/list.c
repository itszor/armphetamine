/*
**    Name: list.c
**
**    Date: Sat Jun 27 15:24:02 1998
**
*/

#include <stdlib.h>

#include "cnew.h"
#include "list.h"

list* list_add(list** oldhead)
{
  list* item = cnew(list);
  item->data = 0;
  item->prev = *oldhead;
  item->next = 0;
  if (*oldhead) (*oldhead)->next = item;

  return *oldhead = item;
}

void list_removehead(list** head)
{
  list* prev = (*head)->prev;
  if (*head) free(*head);
  *head = prev;
}

// Delinks an item without freeing its data pointer */
void list_delinkitem(list** head, list* item)
{
  if (!item) return;
  if (item->prev) item->prev->next = item->next;
  if (item->next) item->next->prev = item->prev;
  if (*head == item) *head = item->prev;
  free(item);
}

list* list_itemfromdata(list* li, void* data)
{
  for (; li; li = li->prev) if (li->data == data) return li;

  return 0;
}

void list_destroy(list* li)
{
  while (li) list_removehead(&li);
}

list* list_nthitem(list* li, int item)
{
  while (item>0)
  {
    li = li->prev;
    if (!li) return 0;   // return zero if item not in list
    item--;
  }
  return li;
}

int list_length(list* head)
{
  int count = 0;

  while (head)
  {
    count++;
    head = head->prev;
  }
  return count;
}
