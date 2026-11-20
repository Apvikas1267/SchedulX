#ifndef GRAPH_H
#define GRAPH_H

/* Directed graph, adjacency-list representation. Edge u->v: v depends on u. */
typedef struct GEdge { int to; struct GEdge *next; } GEdge;
typedef struct { int n; GEdge **adj; int *indeg; } Graph;

Graph *graph_create(int n);
int    graph_add_edge(Graph *g, int u, int v);        /* 0 ok, -1 invalid/duplicate/self-loop */
int    graph_topo_sort(const Graph *g, int *order);   /* Kahn; returns #vertices sorted (< n => cycle) */
int    graph_find_cycle(const Graph *g, int *path, int *len);   /* DFS; 1 if cycle, path closes on itself */
void   graph_free(Graph *g);

#endif
