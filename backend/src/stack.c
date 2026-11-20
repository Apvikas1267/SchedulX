#include <stdlib.h>
#include <string.h>
#include "stack.h"

void stack_init(Stack *s) { s->top = NULL; s->size = 0; }

int stack_push(Stack *s, const char *pid, int time) {
    SNode *n = (SNode *)malloc(sizeof *n);
    if (!n) return -1;
    strncpy(n->pid, pid, PID_LEN - 1); n->pid[PID_LEN - 1] = '\0';
    n->time = time; n->next = s->top;
    s->top = n; s->size++;
    return 0;
}

int stack_pop(Stack *s, char *pid_out, int *time_out) {
    if (!s->top) return 0;
    SNode *n = s->top;
    if (pid_out) strcpy(pid_out, n->pid);
    if (time_out) *time_out = n->time;
    s->top = n->next; free(n); s->size--;
    return 1;
}

int stack_peek(const Stack *s, char *pid_out, int *time_out) {
    if (!s->top) return 0;
    if (pid_out) strcpy(pid_out, s->top->pid);
    if (time_out) *time_out = s->top->time;
    return 1;
}

int stack_empty(const Stack *s) { return s->top == NULL; }
void stack_free(Stack *s) { while (stack_pop(s, NULL, NULL)) {} }

void stack_to_json(const Stack *s, StrBuf *sb, int limit) {
    int i = 0;
    sb_append(sb, "[");
    for (SNode *n = s->top; n && i < limit; n = n->next, i++) {
        sb_printf(sb, "%s{\"pid\":", i ? "," : "");
        sb_json_string(sb, n->pid);
        sb_printf(sb, ",\"time\":%d}", n->time);
    }
    sb_append(sb, "]");
}
