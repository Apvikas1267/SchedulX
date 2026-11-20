#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"

int process_valid_pid(const char *s) {
    size_t n = strlen(s);
    if (n == 0 || n >= PID_LEN) return 0;
    for (size_t i = 0; i < n; i++) {
        unsigned char c = (unsigned char)s[i];
        if (!(isalnum(c) || c == '_' || c == '-')) return 0;
    }
    return 1;
}

int process_validate(const char *pid, int a, int b, int pr, char *err, size_t en) {
    if (!process_valid_pid(pid)) { snprintf(err, en, "Process ID must be 1-%d characters: letters, digits, '_' or '-'", PID_LEN - 1); return -1; }
    if (a < 0 || a > MAX_TIME_VALUE) { snprintf(err, en, "Arrival time must be between 0 and %d", MAX_TIME_VALUE); return -1; }
    if (b < 1 || b > MAX_TIME_VALUE) { snprintf(err, en, "Burst time must be between 1 and %d", MAX_TIME_VALUE); return -1; }
    if (pr < 0 || pr > 999)          { snprintf(err, en, "Priority must be between 0 and 999"); return -1; }
    return 0;
}

int world_init(World *w) {
    pl_init(&w->list); ht_init(&w->ht); w->ndeps = 0;
    w->deps = (Dep *)malloc(sizeof(Dep) * MAX_DEPS);
    return w->deps ? 0 : -1;
}

void world_free(World *w) {
    pl_free(&w->list); ht_free(&w->ht);
    free(w->deps); w->deps = NULL; w->ndeps = 0;
}

void world_clear(World *w) {
    pl_free(&w->list); ht_free(&w->ht); w->ndeps = 0;
}

int world_add_process(World *w, const char *pid, int a, int b, int pr, char *err, size_t en) {
    if (process_validate(pid, a, b, pr, err, en) != 0) return -1;
    if (w->list.count >= MAX_PROCS) { snprintf(err, en, "At most %d processes are supported", MAX_PROCS); return -1; }
    if (ht_get(&w->ht, pid)) { snprintf(err, en, "Process ID '%s' already exists", pid); return -1; }
    Process p; memset(&p, 0, sizeof p);
    strcpy(p.pid, pid); p.arrival = a; p.burst = b; p.priority = pr;
    Process *node = pl_insert_tail(&w->list, &p);
    if (!node) { snprintf(err, en, "Out of memory"); return -1; }
    if (ht_put(&w->ht, node->pid, node) != 0) { pl_delete(&w->list, pid); snprintf(err, en, "Hash table insert failed"); return -1; }
    return 0;
}

int world_update_process(World *w, const char *pid, int a, int b, int pr, char *err, size_t en) {
    Process *p = ht_get(&w->ht, pid);                 /* O(1) lookup through the hash table */
    if (!p) { snprintf(err, en, "Process '%s' not found", pid); return -1; }
    if (process_validate(pid, a, b, pr, err, en) != 0) return -1;
    p->arrival = a; p->burst = b; p->priority = pr;
    return 0;
}

int world_delete_process(World *w, const char *pid) {
    if (!ht_get(&w->ht, pid)) return -1;
    int j = 0;
    for (int i = 0; i < w->ndeps; i++)
        if (strcmp(w->deps[i].from, pid) != 0 && strcmp(w->deps[i].to, pid) != 0) w->deps[j++] = w->deps[i];
    w->ndeps = j;
    ht_remove(&w->ht, pid);
    pl_delete(&w->list, pid);
    return 0;
}

int world_build(World *w, Process ***arr, int *n, Graph **g) {
    *arr = pl_to_array(&w->list, n);
    if (!*arr) return -1;
    for (int i = 0; i < *n; i++) (*arr)[i]->index = i;
    *g = graph_create(*n);
    if (!*g) { free(*arr); *arr = NULL; return -1; }
    for (int i = 0; i < w->ndeps; i++) {
        Process *f = ht_get(&w->ht, w->deps[i].from), *t = ht_get(&w->ht, w->deps[i].to);
        if (f && t) graph_add_edge(*g, f->index, t->index);
    }
    return 0;
}

int world_add_dep(World *w, const char *from, const char *to, int check_cycle, char *err, size_t en) {
    if (!ht_get(&w->ht, from) || !ht_get(&w->ht, to)) { snprintf(err, en, "Unknown process in dependency %s -> %s", from, to); return -1; }
    if (strcmp(from, to) == 0) { snprintf(err, en, "A process cannot depend on itself"); return -1; }
    for (int i = 0; i < w->ndeps; i++)
        if (!strcmp(w->deps[i].from, from) && !strcmp(w->deps[i].to, to)) { snprintf(err, en, "Dependency %s -> %s already exists", from, to); return -1; }
    if (w->ndeps >= MAX_DEPS) { snprintf(err, en, "Too many dependencies"); return -1; }
    strcpy(w->deps[w->ndeps].from, from); strcpy(w->deps[w->ndeps].to, to);
    w->ndeps++;
    if (check_cycle) {
        Process **arr; int n; Graph *g;
        if (world_build(w, &arr, &n, &g) != 0) { w->ndeps--; snprintf(err, en, "Out of memory"); return -1; }
        int *path = (int *)malloc(sizeof(int) * (size_t)(n + 2)), len = 0;
        int cyc = path ? graph_find_cycle(g, path, &len) : 0;
        if (cyc) {
            size_t used = (size_t)snprintf(err, en, "Circular dependency detected: ");
            for (int i = 0; i < len && used < en; i++)
                used += (size_t)snprintf(err + used, en - used, "%s%s", i ? " -> " : "", arr[path[i]]->pid);
            w->ndeps--;
        }
        free(path); free(arr); graph_free(g);
        if (cyc) return -1;
    }
    return 0;
}

int world_remove_dep(World *w, const char *from, const char *to) {
    for (int i = 0; i < w->ndeps; i++)
        if (!strcmp(w->deps[i].from, from) && !strcmp(w->deps[i].to, to)) {
            w->deps[i] = w->deps[--w->ndeps];
            return 0;
        }
    return -1;
}

void world_load_sample(World *w, int with_deps) {
    static const struct { const char *pid; int a, b, p; } S[] = {
        {"P1", 0, 8, 3}, {"P2", 1, 4, 1}, {"P3", 2, 9, 4}, {"P4", 3, 5, 2}, {"P5", 30, 3, 1}
    };
    char err[160];
    world_clear(w);
    for (size_t i = 0; i < sizeof S / sizeof S[0]; i++) world_add_process(w, S[i].pid, S[i].a, S[i].b, S[i].p, err, sizeof err);
    if (with_deps) {
        world_add_dep(w, "P2", "P4", 1, err, sizeof err);
        world_add_dep(w, "P1", "P3", 1, err, sizeof err);
    }
}
