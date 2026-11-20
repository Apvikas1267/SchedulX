#include "stats.h"

void stats_compute(Process **arr, int n, Stats *s) {
    long wait = 0, tat = 0, resp = 0;
    int last = 0, busy = 0, done = 0;
    for (int i = 0; i < n; i++) {
        Process *p = arr[i];
        if (!p->done) continue;
        done++;
        wait += p->waiting; tat += p->turnaround; resp += p->response;
        busy += p->burst;
        if (p->completion > last) last = p->completion;
    }
    s->completed = done; s->busy_time = busy; s->total_time = last;
    s->avg_wait = done ? (double)wait / done : 0;
    s->avg_tat  = done ? (double)tat / done : 0;
    s->avg_resp = done ? (double)resp / done : 0;
    s->cpu_util = last ? 100.0 * busy / last : 0;     /* busy / total simulation time (0 .. last completion) */
    s->throughput = last ? (double)done / last : 0;
}
