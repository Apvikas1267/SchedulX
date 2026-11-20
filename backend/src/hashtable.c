#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

unsigned ht_hash(const char *k) {
    unsigned long h = 5381;
    while (*k) h = ((h << 5) + h) + (unsigned char)*k++;
    return (unsigned)(h % HT_SIZE);
}

void ht_init(HashTable *t) { memset(t, 0, sizeof *t); }

int ht_put(HashTable *t, const char *key, Process *v) {
    unsigned b = ht_hash(key);
    for (HNode *n = t->bucket[b]; n; n = n->next)
        if (strcmp(n->key, key) == 0) return -1;
    HNode *n = (HNode *)malloc(sizeof *n);
    if (!n) return -2;
    strncpy(n->key, key, PID_LEN - 1); n->key[PID_LEN - 1] = '\0';
    n->value = v;
    n->next = t->bucket[b];        /* collision handling: chain at the head */
    t->bucket[b] = n;
    t->count++;
    return 0;
}

Process *ht_get(const HashTable *t, const char *key) {
    for (HNode *n = t->bucket[ht_hash(key)]; n; n = n->next)
        if (strcmp(n->key, key) == 0) return n->value;
    return NULL;
}

int ht_remove(HashTable *t, const char *key) {
    unsigned b = ht_hash(key);
    HNode *prev = NULL, *n = t->bucket[b];
    while (n) {
        if (strcmp(n->key, key) == 0) {
            if (prev) prev->next = n->next; else t->bucket[b] = n->next;
            free(n); t->count--;
            return 1;
        }
        prev = n; n = n->next;
    }
    return 0;
}

void ht_free(HashTable *t) {
    for (int i = 0; i < HT_SIZE; i++) {
        HNode *n = t->bucket[i];
        while (n) { HNode *nx = n->next; free(n); n = nx; }
        t->bucket[i] = NULL;
    }
    t->count = 0;
}

int ht_collisions(const HashTable *t) {
    int c = 0;
    for (int i = 0; i < HT_SIZE; i++) {
        int len = 0;
        for (HNode *n = t->bucket[i]; n; n = n->next) len++;
        if (len > 1) c += len - 1;
    }
    return c;
}

void ht_to_json(const HashTable *t, StrBuf *sb) {
    sb_printf(sb, "{\"size\":%d,\"count\":%d,\"collisions\":%d,\"loadFactor\":%.3f,\"buckets\":[",
              HT_SIZE, t->count, ht_collisions(t), (double)t->count / HT_SIZE);
    for (int i = 0; i < HT_SIZE; i++) {
        sb_printf(sb, "%s{\"index\":%d,\"chain\":[", i ? "," : "", i);
        for (HNode *n = t->bucket[i]; n; n = n->next) {
            if (n != t->bucket[i]) sb_append(sb, ",");
            sb_json_string(sb, n->key);
        }
        sb_append(sb, "]}");
    }
    sb_append(sb, "]}");
}
