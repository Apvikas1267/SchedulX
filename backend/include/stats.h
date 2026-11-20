#ifndef STATS_H
#define STATS_H
#include "common.h"

typedef struct {
    double avg_wait, avg_tat, avg_resp, cpu_util, throughput;
    int total_time, busy_time, completed;
} Stats;

/* Needs completion/start times already filled in each process. */
void stats_compute(Process **arr, int n, Stats *out);

#endif
