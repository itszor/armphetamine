#include <stdio.h>
#include <math.h>

#include "cnew.h"
#include "hash.h"

hashtable* hash_new(uint5 size)
{
  hashtable* htab = cnew(hashtable);
	uint5 i;
	
	htab->entries = 0;
	size = htab->size = 1<<(int)(log((float)size)/log(2)+1.0);
	htab->table = cnewarray(list*, size);
	
	for (i=0; i<size; i++)
	{
	  htab->table[i] = 0;
	}
	
	return htab;
}

// erm; any old nonsense really
#define HASHFN(K,S) (((K)^0x78f352b7)&((S)-1))

hashentry* hash_insert(hashtable* hash, uint5 key)
{
  hashentry* entry;
	uint5 loc = HASHFN(key, hash->size);
	list* item, *theitem=0;
	
	for (item=hash->table[loc]; item; item=item->prev)
	{
	  hashentry* h = (hashentry*) item->data;

		if (h->key==key)
		{
		  theitem = item;
			entry = item->data;
	    break;
		}
	}
	
	if (!theitem)
	{
	  theitem = list_add(&hash->table[loc]);
  	theitem->data = entry = cnew(hashentry);
  	entry->key = key;
  	entry->data = 0;
		hash->entries++;
	}

	return entry;
}

// this gives you back your data pointer, cos you have to free that yourself
void* hash_remove(hashtable* hash, uint5 key)
{
  uint5 loc = HASHFN(key, hash->size);
  list* item, *theitem=0;
	void* hashentrydata=0;
	
	for (item=hash->table[loc]; item;)
	{
	  list* prev = item->prev;
		hashentry* h = (hashentry*) item->data;

		if (h->key==key)
		{
		  hashentrydata = h->data;
		  free(h);
		  list_delinkitem(&hash->table[loc], item);
			hash->entries--;
		}
		
		item = prev;
	}
	
	return hashentrydata;
}

// delete an entry, calling a destructor function
void hash_delete(hashtable* hash, uint5 key, hashdestructor_fn destructor)
{
  void* data = hash_remove(hash, key);
	destructor(data);
}

// destroy an entire hash table, plus all its contents
void hash_nuke(hashtable* hash, hashdestructor_fn destructor)
{
  list* w;
	uint5 i;
	
	for (i=0; i<hash->size; i++)
	{
	  while (hash->table[i])
		{
		  if (destructor) destructor(hash->table[i]->data);
			list_removehead(&hash->table[i]);
		}
	}

	free(hash->table);
	free(hash);
}

hashentry* hash_lookup(hashtable* hash, uint5 key)
{
	uint5 loc = HASHFN(key, hash->size);
	list* item;
	
	for (item=hash->table[loc]; item; item=item->prev)
	{
	  hashentry* h = (hashentry*) item->data;
		if (h->key==key) return h;
	}

	return 0;
}
