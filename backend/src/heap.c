#include <stdlib.h>
#include "heap.h"

int heap_key(const Process *p, HeapMode m) {
    return m == HEAP_BURST ? p->burst : (m == HEAP_REMAINING ? p->remaining : p->pkey);
}

/* Order: key, then earlier arrival, then input order (index). */
static int less(const Process *a, const Process *b, HeapMode m) {
    int ka = heap_key(a, m), kb = heap_key(b, m);
    if (ka != kb) return ka < kb;
    if (a->arrival != b->arrival) return a->arrival < b->arrival;
    return a->index < b->index;
}

int heap_init(MinHeap *h, int cap, HeapMode mode) {
    h->a = (Process **)malloc(sizeof(Process *) * (size_t)(cap > 0 ? cap : 1));
    h->size = 0; h->cap = cap; h->mode = mode;
    return h->a ? 0 : -1;
}

int heap_insert(MinHeap *h, Process *p) {
    if (h->size == h->cap) return -1;
    int i = h->size++;
    h->a[i] = p;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (!less(h->a[i], h->a[parent], h->mode)) break;
        Process *t = h->a[i]; h->a[i] = h->a[parent]; h->a[parent] = t;
        i = parent;
    }
    return 0;
}

Process *heap_extract_min(MinHeap *h) {
    if (h->size == 0) return NULL;
    Process *root = h->a[0];
    h->a[0] = h->a[--h->size];
    int i = 0;
    for (;;) {
        int l = 2 * i + 1, r = l + 1, m = i;
        if (l < h->size && less(h->a[l], h->a[m], h->mode)) m = l;
        if (r < h->size && less(h->a[r], h->a[m], h->mode)) m = r;
        if (m == i) break;
        Process *t = h->a[i]; h->a[i] = h->a[m]; h->a[m] = t;
        i = m;
    }
    return root;
}

Process *heap_peek(const MinHeap *h) { return h->size ? h->a[0] : NULL; }
int heap_empty(const MinHeap *h) { return h->size == 0; }
void heap_free(MinHeap *h) { free(h->a); h->a = NULL; h->size = h->cap = 0; }

void heap_to_json(const MinHeap *h, StrBuf *sb) {
    const char *mode = h->mode == HEAP_BURST ? "burst" : (h->mode == HEAP_REMAINING ? "remaining" : "priority");
    sb_printf(sb, "{\"kind\":\"heap\",\"mode\":\"%s\",\"items\":[", mode);
    for (int i = 0; i < h->size; i++) {
        sb_printf(sb, "%s{\"pid\":", i ? "," : "");
        sb_json_string(sb, h->a[i]->pid);
        sb_printf(sb, ",\"key\":%d}", heap_key(h->a[i], h->mode));
    }
    sb_append(sb, "]}");
}
