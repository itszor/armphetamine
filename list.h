/*
**    Name: list.h
**
**    Date: Wed Sep 16 23:20:08 1998
**
*/

#ifndef LIST_H
#define LIST_H 1

typedef struct list_t {
  void* data;
  struct list_t* prev;
  struct list_t* next;
} list;

typedef list* (*listsearchfn)(list*, void*);

extern list* list_add(list** oldhead);
extern void list_removehead(list** head);
extern void list_delinkitem(list** head, list* item);
extern list* list_itemfromdata(list* li, void* data);
extern void list_destroy(list* li);
extern list* list_nthitem(list* li, int item);
extern int list_length(list* head);
extern list* list_end(list* head);
extern list* list_search(list* head, listsearchfn srch, void* data);

#endif
