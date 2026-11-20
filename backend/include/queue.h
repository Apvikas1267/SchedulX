#ifndef QUEUE_H
#define QUEUE_H
#include "common.h"
#include "strbuf.h"

/* ---- FIFO queue implemented with a linked list (FCFS ready queue) ---- */
typedef struct QNode { Process *p; struct QNode *next; } QNode;
typedef struct { QNode *front, *rear; int size; } Queue;

void queue_init(Queue *q);
int  queue_enqueue(Queue *q, Process *p);
Process *queue_dequeue(Queue *q);
Process *queue_peek(const Queue *q);
int  queue_empty(const Queue *q);
void queue_free(Queue *q);
void queue_to_json(const Queue *q, StrBuf *sb);

/* ---- Circular queue implemented with an array (Round Robin ready queue) ---- */
typedef struct { Process **slot; int cap, front, size; } CircularQueue;

int  cq_init(CircularQueue *q, int cap);
int  cq_enqueue(CircularQueue *q, Process *p);     /* 0 ok, -1 full */
Process *cq_dequeue(CircularQueue *q);
Process *cq_peek(const CircularQueue *q);
int  cq_empty(const CircularQueue *q);
void cq_free(CircularQueue *q);
void cq_to_json(const CircularQueue *q, StrBuf *sb);

#endif
