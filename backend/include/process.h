#ifndef PROCESS_H
#define PROCESS_H
#include <stddef.h>
#include "common.h"
#include "linkedlist.h"
#include "hashtable.h"
#include "graph.h"

/* A dependency: "to" can only run after "from" has completed (edge from -> to). */
typedef struct { char from[PID_LEN]; char to[PID_LEN]; } Dep;

/* A "world" = process list (linked list) + PID index (hash table) + dependency edges. */
typedef struct {
    ProcList list;
    HashTable ht;
    Dep *deps;
    int ndeps;
} World;

int  process_valid_pid(const char *s);
int  process_validate(const char *pid, int arrival, int burst, int priority, char *err, size_t en);

int  world_init(World *w);
void world_free(World *w);
int  world_add_process(World *w, const char *pid, int a, int b, int pr, char *err, size_t en);
int  world_update_process(World *w, const char *pid, int a, int b, int pr, char *err, size_t en);
int  world_delete_process(World *w, const char *pid);
void world_clear(World *w);
int  world_add_dep(World *w, const char *from, const char *to, int check_cycle, char *err, size_t en);
int  world_remove_dep(World *w, const char *from, const char *to);
/* Builds the process array (list order, index set) and the dependency graph. Caller frees both. */
int  world_build(World *w, Process ***arr, int *n, Graph **g);
void world_load_sample(World *w, int with_deps);

#endif
