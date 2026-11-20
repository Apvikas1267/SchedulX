#include <stdlib.h>
#include "graph.h"

Graph *graph_create(int n) {
    Graph *g = (Graph *)malloc(sizeof *g);
    if (!g) return NULL;
    g->n = n;
    g->adj = (GEdge **)calloc((size_t)(n ? n : 1), sizeof(GEdge *));
    g->indeg = (int *)calloc((size_t)(n ? n : 1), sizeof(int));
    if (!g->adj || !g->indeg) { free(g->adj); free(g->indeg); free(g); return NULL; }
    return g;
}

int graph_add_edge(Graph *g, int u, int v) {
    if (u < 0 || v < 0 || u >= g->n || v >= g->n || u == v) return -1;
    for (GEdge *e = g->adj[u]; e; e = e->next) if (e->to == v) return -1;
    GEdge *e = (GEdge *)malloc(sizeof *e);
    if (!e) return -1;
    e->to = v; e->next = g->adj[u]; g->adj[u] = e;
    g->indeg[v]++;
    return 0;
}

int graph_topo_sort(const Graph *g, int *order) {
    int *deg = (int *)malloc(sizeof(int) * (size_t)(g->n ? g->n : 1));
    int *q = (int *)malloc(sizeof(int) * (size_t)(g->n ? g->n : 1));
    int head = 0, tail = 0, cnt = 0;
    if (!deg || !q) { free(deg); free(q); return 0; }
    for (int i = 0; i < g->n; i++) { deg[i] = g->indeg[i]; if (deg[i] == 0) q[tail++] = i; }
    while (head < tail) {
        int u = q[head++];
        order[cnt++] = u;
        for (GEdge *e = g->adj[u]; e; e = e->next)
            if (--deg[e->to] == 0) q[tail++] = e->to;
    }
    free(deg); free(q);
    return cnt;
}

static int dfs(const Graph *g, int u, int *color, int *stack, int *sp, int *path, int *plen) {
    color[u] = 1; stack[(*sp)++] = u;
    for (GEdge *e = g->adj[u]; e; e = e->next) {
        int v = e->to;
        if (color[v] == 1) {
            int i = *sp - 1, k = 0;
            while (stack[i] != v) i--;
            for (; i < *sp; i++) path[k++] = stack[i];
            path[k++] = v;
            *plen = k;
            return 1;
        }
        if (color[v] == 0 && dfs(g, v, color, stack, sp, path, plen)) return 1;
    }
    color[u] = 2; (*sp)--;
    return 0;
}

int graph_find_cycle(const Graph *g, int *path, int *len) {
    int *color = (int *)calloc((size_t)(g->n ? g->n : 1), sizeof(int));
    int *stack = (int *)malloc(sizeof(int) * (size_t)(g->n ? g->n : 1));
    int sp = 0, found = 0;
    if (!color || !stack) { free(color); free(stack); return 0; }
    for (int i = 0; i < g->n && !found; i++)
        if (color[i] == 0) found = dfs(g, i, color, stack, &sp, path, len);
    free(color); free(stack);
    return found;
}

void graph_free(Graph *g) {
    if (!g) return;
    for (int i = 0; i < g->n; i++) {
        GEdge *e = g->adj[i];
        while (e) { GEdge *nx = e->next; free(e); e = nx; }
    }
    free(g->adj); free(g->indeg); free(g);
}
