#include <stdlib.h>
#include "queue.h"

void queue_init(Queue *q) { q->front = q->rear = NULL; q->size = 0; }

int queue_enqueue(Queue *q, Process *p) {
    QNode *n = (QNode *)malloc(sizeof *n);
    if (!n) return -1;
    n->p = p; n->next = NULL;
    if (q->rear) q->rear->next = n; else q->front = n;
    q->rear = n; q->size++;
    return 0;
}

Process *queue_dequeue(Queue *q) {
    if (!q->front) return NULL;
    QNode *n = q->front; Process *p = n->p;
    q->front = n->next;
    if (!q->front) q->rear = NULL;
    free(n); q->size--;
    return p;
}

Process *queue_peek(const Queue *q) { return q->front ? q->front->p : NULL; }
int queue_empty(const Queue *q) { return q->size == 0; }
void queue_free(Queue *q) { while (q->front) queue_dequeue(q); }

void queue_to_json(const Queue *q, StrBuf *sb) {
    sb_append(sb, "{\"kind\":\"queue\",\"items\":[");
    for (QNode *n = q->front; n; n = n->next) {
        if (n != q->front) sb_append(sb, ",");
        sb_json_string(sb, n->p->pid);
    }
    sb_append(sb, "]}");
}

int cq_init(CircularQueue *q, int cap) {
    q->slot = (Process **)calloc((size_t)cap, sizeof(Process *));
    q->cap = cap; q->front = 0; q->size = 0;
    return q->slot ? 0 : -1;
}

int cq_enqueue(CircularQueue *q, Process *p) {
    if (q->size == q->cap) return -1;
    q->slot[(q->front + q->size) % q->cap] = p;     /* rear wraps around */
    q->size++;
    return 0;
}

Process *cq_dequeue(CircularQueue *q) {
    if (q->size == 0) return NULL;
    Process *p = q->slot[q->front];
    q->slot[q->front] = NULL;
    q->front = (q->front + 1) % q->cap;
    q->size--;
    return p;
}

Process *cq_peek(const CircularQueue *q) { return q->size ? q->slot[q->front] : NULL; }
int cq_empty(const CircularQueue *q) { return q->size == 0; }
void cq_free(CircularQueue *q) { free(q->slot); q->slot = NULL; q->cap = q->size = q->front = 0; }

void cq_to_json(const CircularQueue *q, StrBuf *sb) {
    sb_printf(sb, "{\"kind\":\"circular\",\"cap\":%d,\"front\":%d,\"size\":%d,\"slots\":[",
              q->cap, q->front, q->size);
    for (int i = 0; i < q->cap; i++) {
        if (i) sb_append(sb, ",");
        if (q->slot[i]) sb_json_string(sb, q->slot[i]->pid); else sb_append(sb, "null");
    }
    sb_append(sb, "]}");
}
