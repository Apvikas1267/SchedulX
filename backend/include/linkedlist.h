#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include "common.h"

/* Singly linked list of processes (dynamic process store). */
typedef struct { Process *head, *tail; int count; } ProcList;

void      pl_init(ProcList *l);
Process  *pl_insert_tail(ProcList *l, const Process *src);   /* copies *src */
Process  *pl_insert_head(ProcList *l, const Process *src);
int       pl_delete(ProcList *l, const char *pid);           /* 1 if removed */
Process  *pl_search(const ProcList *l, const char *pid);     /* linear search */
Process **pl_to_array(const ProcList *l, int *n);            /* traversal -> array (caller frees) */
void      pl_free(ProcList *l);

#endif
