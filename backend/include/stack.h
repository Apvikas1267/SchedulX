#ifndef STACK_H
#define STACK_H
#include "common.h"
#include "strbuf.h"

/* LIFO stack implemented with a linked list: CPU execution history. */
typedef struct SNode { char pid[PID_LEN]; int time; struct SNode *next; } SNode;
typedef struct { SNode *top; int size; } Stack;

void stack_init(Stack *s);
int  stack_push(Stack *s, const char *pid, int time);
int  stack_pop(Stack *s, char *pid_out, int *time_out);    /* 1 ok, 0 empty */
int  stack_peek(const Stack *s, char *pid_out, int *time_out);
int  stack_empty(const Stack *s);
void stack_free(Stack *s);
void stack_to_json(const Stack *s, StrBuf *sb, int limit);   /* top first */

#endif
