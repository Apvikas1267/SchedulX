#ifndef HEAP_H
#define HEAP_H
#include "common.h"
#include "strbuf.h"

/* Binary min-heap (array representation of a complete binary tree). */
typedef enum { HEAP_BURST, HEAP_REMAINING, HEAP_PRIORITY } HeapMode;
typedef struct { Process **a; int size, cap; HeapMode mode; } MinHeap;

int  heap_init(MinHeap *h, int cap, HeapMode mode);
int  heap_insert(MinHeap *h, Process *p);      /* sift-up   */
Process *heap_extract_min(MinHeap *h);         /* sift-down */
Process *heap_peek(const MinHeap *h);
int  heap_empty(const MinHeap *h);
void heap_free(MinHeap *h);
int  heap_key(const Process *p, HeapMode m);
void heap_to_json(const MinHeap *h, StrBuf *sb);

#endif
