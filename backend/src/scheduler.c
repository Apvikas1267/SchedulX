#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"
#include "queue.h"
#include "stack.h"
#include "heap.h"
#include "strbuf.h"

static const char *IDS[]   = { "fcfs", "sjf", "srtf", "rr", "prio_np", "prio_p" };
static const char *NAMES[] = { "First Come First Served (FCFS)", "Shortest Job First (non-preemptive)",
                               "Shortest Remaining Time First (preemptive)", "Round Robin",
                               "Priority Scheduling (non-preemptive)", "Priority Scheduling (preemptive)" };

const char *algo_id(Algo a)   { return IDS[a]; }
const char *algo_name(Algo a) { return NAMES[a]; }
int algo_from_string(const char *s, Algo *out) {
    for (int i = 0; i < ALGO_COUNT; i++) if (strcmp(s, IDS[i]) == 0) { *out = (Algo)i; return 0; }
    return -1;
}

struct Cand { int ready, arrival, index; Process *p; };

typedef struct {
    Process **arr; int n; const Graph *g;
    int *indeg;              /* run-time in-degree: 0 => all dependencies finished */
    int *ready_at;           /* time a process became eligible: max(arrival, finish of its dependencies) */
    struct Cand *cand;       /* scratch array used by admit() */
    Algo algo; int quantum;
    int t, done, fail;
    Queue q; CircularQueue cq; MinHeap h; Stack hist;
    Process *last;
    char ops[256];
    SimResult *res;
} Sim;

static void sim_fail(Sim *s, const char *msg) {
    if (!s->fail) { s->fail = 1; snprintf(s->res->error, sizeof s->res->error, "%s", msg); }
}

static void op(Sim *s, const char *fmt, ...) {
    size_t used = strlen(s->ops);
    va_list ap;
    if (used + 4 >= sizeof s->ops) return;
    if (used) { strcat(s->ops, "; "); used += 2; }
    va_start(ap, fmt);
    vsnprintf(s->ops + used, sizeof s->ops - used, fmt, ap);
    va_end(ap);
}

/* ---------- ready-structure abstraction ---------- */
static void ready_insert(Sim *s, Process *p) {
    int rc = 0;
    switch (s->algo) {
    case ALGO_FCFS: rc = queue_enqueue(&s->q, p);  op(s, "enqueue %s", p->pid); break;
    case ALGO_RR:   rc = cq_enqueue(&s->cq, p);    op(s, "cq-enqueue %s", p->pid); break;
    default:        rc = heap_insert(&s->h, p);    op(s, "heap-insert %s", p->pid); break;
    }
    if (rc != 0) sim_fail(s, "Internal error: ready structure overflow");
}

static int ready_empty(const Sim *s) {
    switch (s->algo) {
    case ALGO_FCFS: return queue_empty(&s->q);
    case ALGO_RR:   return cq_empty(&s->cq);
    default:        return heap_empty(&s->h);
    }
}

static Process *ready_pop(Sim *s) {
    Process *p;
    switch (s->algo) {
    case ALGO_FCFS: p = queue_dequeue(&s->q);      op(s, "dequeue %s", p->pid); break;
    case ALGO_RR:   p = cq_dequeue(&s->cq);        op(s, "cq-dequeue %s", p->pid); break;
    default:        p = heap_extract_min(&s->h);   op(s, "extract-min %s", p->pid); break;
    }
    return p;
}

static int cmp_cand(const void *a, const void *b) {
    const struct Cand *x = (const struct Cand *)a, *y = (const struct Cand *)b;
    if (x->ready != y->ready) return x->ready < y->ready ? -1 : 1;
    if (x->arrival != y->arrival) return x->arrival < y->arrival ? -1 : 1;
    return x->index - y->index;
}

/* Move every process that is eligible now (arrived and all dependencies finished) into the ready
   structure, in order of the moment it became eligible (ties: arrival, then input order). */
static void admit(Sim *s) {
    int k = 0;
    for (int i = 0; i < s->n; i++) {
        Process *p = s->arr[i];
        if (!p->admitted && s->indeg[i] == 0 && s->ready_at[i] <= s->t) {
            s->cand[k].ready = s->ready_at[i]; s->cand[k].arrival = p->arrival; s->cand[k].index = i; s->cand[k].p = p; k++;
        }
    }
    if (k > 1) qsort(s->cand, (size_t)k, sizeof s->cand[0], cmp_cand);
    for (int i = 0; i < k && !s->fail; i++) { s->cand[i].p->admitted = 1; ready_insert(s, s->cand[i].p); }
}

/* Earliest future arrival among processes that are already dependency-free (-1 if none). */
static int next_arrival(const Sim *s) {
    int best = -1;
    for (int i = 0; i < s->n; i++) {
        const Process *p = s->arr[i];
        if (!p->admitted && s->indeg[p->index] == 0 && p->arrival > s->t && (best < 0 || p->arrival < best))
            best = p->arrival;
    }
    return best;
}

static void *grow(void *ptr, int count, size_t elem) {
    if (count == 0 || (count & (count - 1)) != 0) return ptr;         /* realloc only at powers of two */
    return realloc(ptr, elem * (size_t)count * 2);
}

static void add_segment(Sim *s, Process *p, int start, int end) {
    SimResult *r = s->res;
    if (end <= start) return;
    if (p && s->algo != ALGO_RR && r->nseg > 0) {                   /* merge contiguous slices of the same process */
        Segment *l = &r->seg[r->nseg - 1];
        if (!l->idle && l->end == start && strcmp(l->pid, p->pid) == 0) { l->end = end; l->rem_after = p->remaining; return; }
    }
    if (r->nseg >= MAX_SEGMENTS) { sim_fail(s, "Too many Gantt segments; use a larger time quantum"); return; }
    if (r->nseg == 0) r->seg = (Segment *)malloc(sizeof(Segment) * 2);
    else { Segment *t = (Segment *)grow(r->seg, r->nseg, sizeof(Segment)); if (!t) { sim_fail(s, "Out of memory"); return; } r->seg = t; }
    if (!r->seg) { sim_fail(s, "Out of memory"); return; }
    Segment *g = &r->seg[r->nseg++];
    memset(g, 0, sizeof *g);
    g->start = start; g->end = end;
    if (p) { strcpy(g->pid, p->pid); g->rem_after = p->remaining; } else g->idle = 1;
}

static void snapshot(Sim *s, Process *p) {
    SimResult *r = s->res;
    if (r->ntrace >= MAX_TRACE) return;      /* visualizer keeps the first MAX_TRACE steps only */
    if (r->ntrace == 0) r->trace = (TraceStep *)malloc(sizeof(TraceStep) * 2);
    else { TraceStep *t = (TraceStep *)grow(r->trace, r->ntrace, sizeof(TraceStep)); if (!t) { sim_fail(s, "Out of memory"); return; } r->trace = t; }
    if (!r->trace) { sim_fail(s, "Out of memory"); return; }
    TraceStep *ts = &r->trace[r->ntrace++];
    memset(ts, 0, sizeof *ts);
    ts->time = s->t;
    strcpy(ts->pid, p->pid);
    snprintf(ts->ops, sizeof ts->ops, "%s", s->ops);
    StrBuf a; sb_init(&a);
    switch (s->algo) {
    case ALGO_FCFS: queue_to_json(&s->q, &a); break;
    case ALGO_RR:   cq_to_json(&s->cq, &a); break;
    default:        heap_to_json(&s->h, &a); break;
    }
    ts->ready_json = sb_take(&a);
    StrBuf b; sb_init(&b);
    stack_to_json(&s->hist, &b, 12);
    ts->stack_json = sb_take(&b);
    s->ops[0] = '\0';
}

static void dispatch(Sim *s, Process *p) {
    if (p->start_time < 0) p->start_time = s->t;
    if (stack_push(&s->hist, p->pid, s->t) != 0) sim_fail(s, "Out of memory");
    op(s, "push %s", p->pid);
    snapshot(s, p);
    s->last = p;
}

static void complete(Sim *s, Process *p) {
    p->completion = s->t;
    p->turnaround = p->completion - p->arrival;
    p->waiting = p->turnaround - p->burst;
    p->response = p->start_time - p->arrival;
    p->done = 1; s->done++;
    for (GEdge *e = s->g->adj[p->index]; e; e = e->next) {                    /* unlock dependents */
        s->indeg[e->to]--;
        if (s->ready_at[e->to] < s->t) s->ready_at[e->to] = s->t;
    }
}

/* CPU idle: jump to the next arrival. Returns 0 if the simulation cannot progress. */
static int idle_jump(Sim *s) {
    int nt = next_arrival(s);
    if (nt < 0) { sim_fail(s, "Deadlock: remaining processes can never become ready"); return 0; }
    add_segment(s, NULL, s->t, nt);
    s->t = nt;
    return 1;
}

/* FCFS, SJF, non-preemptive priority and Round Robin. */
static void run_slice_based(Sim *s) {
    while (s->done < s->n && !s->fail) {
        admit(s);
        if (ready_empty(s)) { if (!idle_jump(s)) return; continue; }
        Process *p = ready_pop(s);
        dispatch(s, p);
        int run = p->remaining;
        if (s->algo == ALGO_RR && run > s->quantum) run = s->quantum;
        s->t += run; p->remaining -= run;
        add_segment(s, p, s->t - run, s->t);
        if (p->remaining == 0) complete(s, p);
        if (s->algo == ALGO_RR) {
            admit(s);                                   /* arrivals during/at the end of the slice go first */
            if (p->remaining > 0) ready_insert(s, p);   /* then the pre-empted process re-joins at the rear */
        }
    }
}

/* SRTF and preemptive priority: re-evaluate the heap root at every arrival. */
static void run_preemptive(Sim *s) {
    while (s->done < s->n && !s->fail) {
        admit(s);
        if (ready_empty(s)) { s->last = NULL; if (!idle_jump(s)) return; continue; }
        Process *p = heap_peek(&s->h);
        if (p != s->last) dispatch(s, p);
        int nt = next_arrival(s);
        int run = p->remaining;
        if (nt >= 0 && nt - s->t < run) run = nt - s->t;
        s->t += run; p->remaining -= run;
        add_segment(s, p, s->t - run, s->t);
        if (p->remaining == 0) {
            heap_extract_min(&s->h);
            op(s, "extract-min %s (finished)", p->pid);
            complete(s, p);
            s->last = NULL;
        }
    }
}

int sched_run(Process **arr, int n, const Graph *g, Algo algo, int quantum, int high_is_high, SimResult *res) {
    memset(res, 0, sizeof *res);
    res->algo = algo; res->quantum = quantum; res->high_is_high = high_is_high;
    if (n <= 0) { snprintf(res->error, sizeof res->error, "No processes to schedule"); return -1; }
    if (algo == ALGO_RR && (quantum < 1 || quantum > MAX_TIME_VALUE)) {
        snprintf(res->error, sizeof res->error, "Time quantum must be between 1 and %d", MAX_TIME_VALUE); return -1;
    }
    /* Dependencies: topological sort (Kahn) rejects circular dependencies. */
    res->topo = (int *)malloc(sizeof(int) * (size_t)n);
    if (!res->topo) { snprintf(res->error, sizeof res->error, "Out of memory"); return -1; }
    res->ntopo = graph_topo_sort(g, res->topo);
    if (res->ntopo < n) {
        int *path = (int *)malloc(sizeof(int) * (size_t)(n + 2)), len = 0;
        size_t used = (size_t)snprintf(res->error, sizeof res->error, "Circular dependency detected");
        if (path && graph_find_cycle(g, path, &len)) {
            used += (size_t)snprintf(res->error + used, sizeof res->error - used, ": ");
            for (int i = 0; i < len && used < sizeof res->error; i++)
                used += (size_t)snprintf(res->error + used, sizeof res->error - used, "%s%s", i ? " -> " : "", arr[path[i]]->pid);
        }
        free(path); free(res->topo); res->topo = NULL; res->ntopo = 0;
        return -1;
    }

    Sim s; memset(&s, 0, sizeof s);
    s.arr = arr; s.n = n; s.g = g; s.algo = algo; s.quantum = quantum; s.res = res;
    s.indeg = (int *)malloc(sizeof(int) * (size_t)n);
    s.ready_at = (int *)malloc(sizeof(int) * (size_t)n);
    s.cand = (struct Cand *)malloc(sizeof(struct Cand) * (size_t)n);
    if (!s.indeg || !s.ready_at || !s.cand) { free(s.indeg); free(s.ready_at); free(s.cand); snprintf(res->error, sizeof res->error, "Out of memory"); return -1; }
    for (int i = 0; i < n; i++) {
        Process *p = arr[i];
        p->remaining = p->burst;
        p->pkey = high_is_high ? -p->priority : p->priority;
        p->start_time = -1; p->completion = p->turnaround = p->waiting = p->response = 0;
        p->admitted = p->done = 0;
        s.indeg[i] = g->indeg[i];
        s.ready_at[i] = p->arrival;
    }
    queue_init(&s.q); stack_init(&s.hist);
    int ok = 0;
    switch (algo) {
    case ALGO_FCFS: ok = 1; break;
    case ALGO_RR:   ok = cq_init(&s.cq, n + 1) == 0; break;
    case ALGO_SJF:  ok = heap_init(&s.h, n, HEAP_BURST) == 0; break;
    case ALGO_SRTF: ok = heap_init(&s.h, n, HEAP_REMAINING) == 0; break;
    default:        ok = heap_init(&s.h, n, HEAP_PRIORITY) == 0; break;
    }
    if (!ok) sim_fail(&s, "Out of memory");
    else if (algo == ALGO_SRTF || algo == ALGO_PRIO_P) run_preemptive(&s);
    else run_slice_based(&s);

    queue_free(&s.q); stack_free(&s.hist);
    if (algo == ALGO_RR) cq_free(&s.cq);
    if (algo == ALGO_SJF || algo == ALGO_SRTF || algo == ALGO_PRIO_NP || algo == ALGO_PRIO_P) heap_free(&s.h);
    free(s.indeg); free(s.ready_at); free(s.cand);
    if (s.fail) return -1;
    stats_compute(arr, n, &res->stats);
    return 0;
}

void sched_result_free(SimResult *res) {
    for (int i = 0; i < res->ntrace; i++) { free(res->trace[i].ready_json); free(res->trace[i].stack_json); }
    free(res->trace); free(res->seg); free(res->topo);
    memset(res, 0, sizeof *res);
}
