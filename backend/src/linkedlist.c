#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

void pl_init(ProcList *l) { l->head = l->tail = NULL; l->count = 0; }

Process *pl_insert_tail(ProcList *l, const Process *src) {
    Process *n = (Process *)malloc(sizeof *n);
    if (!n) return NULL;
    *n = *src; n->next = NULL;
    if (l->tail) l->tail->next = n; else l->head = n;
    l->tail = n; l->count++;
    return n;
}

Process *pl_insert_head(ProcList *l, const Process *src) {
    Process *n = (Process *)malloc(sizeof *n);
    if (!n) return NULL;
    *n = *src; n->next = l->head;
    l->head = n;
    if (!l->tail) l->tail = n;
    l->count++;
    return n;
}

int pl_delete(ProcList *l, const char *pid) {
    Process *prev = NULL, *cur = l->head;
    while (cur) {
        if (strcmp(cur->pid, pid) == 0) {
            if (prev) prev->next = cur->next; else l->head = cur->next;
            if (cur == l->tail) l->tail = prev;
            free(cur); l->count--;
            return 1;
        }
        prev = cur; cur = cur->next;
    }
    return 0;
}

Process *pl_search(const ProcList *l, const char *pid) {
    for (Process *p = l->head; p; p = p->next)
        if (strcmp(p->pid, pid) == 0) return p;
    return NULL;
}

Process **pl_to_array(const ProcList *l, int *n) {
    Process **a = (Process **)malloc(sizeof(Process *) * (size_t)(l->count ? l->count : 1));
    int i = 0;
    if (!a) { *n = 0; return NULL; }
    for (Process *p = l->head; p; p = p->next) a[i++] = p;
    *n = i;
    return a;
}

void pl_free(ProcList *l) {
    Process *p = l->head;
    while (p) { Process *nx = p->next; free(p); p = nx; }
    l->head = l->tail = NULL; l->count = 0;
}
