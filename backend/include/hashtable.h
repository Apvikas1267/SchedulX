#ifndef HASHTABLE_H
#define HASHTABLE_H
#include "common.h"
#include "strbuf.h"

/* Hash table with separate chaining. Small on purpose so collisions are visible. */
#define HT_SIZE 13

typedef struct HNode { char key[PID_LEN]; Process *value; struct HNode *next; } HNode;
typedef struct { HNode *bucket[HT_SIZE]; int count; } HashTable;

unsigned  ht_hash(const char *key);                       /* djb2 mod HT_SIZE */
void      ht_init(HashTable *t);
int       ht_put(HashTable *t, const char *key, Process *v);   /* 0 ok, -1 duplicate, -2 no memory */
Process  *ht_get(const HashTable *t, const char *key);
int       ht_remove(HashTable *t, const char *key);
void      ht_free(HashTable *t);
int       ht_collisions(const HashTable *t);              /* sum over buckets of (chain length - 1) */
void      ht_to_json(const HashTable *t, StrBuf *sb);

#endif
