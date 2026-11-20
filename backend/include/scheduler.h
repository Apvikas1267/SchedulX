#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "common.h"
#include "graph.h"
#include "stats.h"

typedef enum { ALGO_FCFS, ALGO_SJF, ALGO_SRTF, ALGO_RR, ALGO_PRIO_NP, ALGO_PRIO_P, ALGO_COUNT } Algo;

typedef struct {
    int idle;
    char pid[PID_LEN];
    int start, end;
    int rem_after;           /* remaining burst when the block ended (>0 => preempted / quantum expired) */
} Segment;

typedef struct {
    int time;
    char pid[PID_LEN];       /* process dispatched at this step */
    char ops[256];           /* data-structure operations performed */
    char *ready_json;        /* snapshot of the ready queue / circular queue / heap */
    char *stack_json;        /* snapshot of the execution-history stack */
} TraceStep;

typedef struct {
    Algo algo;
    int quantum, high_is_high;
    Segment *seg; int nseg;
    TraceStep *trace; int ntrace;
    int *topo; int ntopo;    /* topological order of process indices */
    Stats stats;
    char error[200];
} SimResult;

const char *algo_id(Algo a);
const char *algo_name(Algo a);
int algo_from_string(const char *s, Algo *out);

/* arr[i]->index must equal i and g must have one vertex per process. Returns 0 on success. */
int  sched_run(Process **arr, int n, const Graph *g, Algo algo, int quantum, int high_is_high, SimResult *res);
void sched_result_free(SimResult *res);

#endif
