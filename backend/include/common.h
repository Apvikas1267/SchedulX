#ifndef COMMON_H
#define COMMON_H

#define PID_LEN 16
#define MAX_PROCS 100
#define MAX_DEPS 500
#define MAX_TIME_VALUE 10000
#define MAX_SEGMENTS 20000
#define MAX_TRACE 2000          /* trace steps recorded for the DSA visualizer */

/* One process control block. Also the node type of the singly linked list. */
typedef struct Process {
    char pid[PID_LEN];
    int arrival, burst, priority;
    int index;                 /* position in the process array / graph vertex id */
    /* simulation state (reset at the start of every run) */
    int remaining, pkey;       /* pkey: priority key, smaller = scheduled first */
    int start_time, completion, turnaround, waiting, response;
    int admitted, done;
    struct Process *next;
} Process;

#endif
