
#ifndef HASH_H
#define HASH_H 1

#include "defs.h"
#include "list.h"

typedef struct {
  uint5 key;
	void* data;
} hashentry;

typedef struct {
  list** table;
	uint5 entries;
	uint5 size;
} hashtable;

typedef void (*hashdestructor_fn)(void* e);

extern hashtable* hash_new(uint5 size);
extern hashentry* hash_insert(hashtable* hash, uint5 key);
extern void* hash_remove(hashtable* hash, uint5 key);
extern void hash_delete(hashtable* hash, uint5 key, hashdestructor_fn f);
extern void hash_nuke(hashtable* hash, hashdestructor_fn destructor);
extern hashentry* hash_lookup(hashtable* hash, uint5 key);

#endif
