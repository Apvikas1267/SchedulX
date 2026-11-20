/* SchedulX tests: data-structure unit tests + scheduling cases.
   The expected values of the scheduling cases were produced by an independent tick-by-tick
   reference simulator (and checked against textbook examples), not by this code. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "process.h"
#include "scheduler.h"
#include "queue.h"
#include "stack.h"
#include "heap.h"

typedef struct { const char *pid; int a, b, p; } P;
typedef struct { const char *from, *to; } E;
typedef struct {
    const char *name; Algo algo; int quantum, high, np, nd;
    const P *procs; const E *deps;
    const char *gantt;            /* "PID:start-end,..." */
    int ct[8], tat[8], wt[8], rt[8];
    double avg_wait, avg_tat, cpu;
} Case;

static int g_fail = 0, g_pass = 0;
#define CHECK(cond, ...) do { if (cond) g_pass++; else { g_fail++; printf("  FAIL: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static void run_case(const Case *c) {
    World w; char err[200]; Process **arr; int n; Graph *g; SimResult r;
    char got[4096] = ""; size_t used = 0;
    world_init(&w);
    for (int i = 0; i < c->np; i++) world_add_process(&w, c->procs[i].pid, c->procs[i].a, c->procs[i].b, c->procs[i].p, err, sizeof err);
    for (int i = 0; i < c->nd; i++) world_add_dep(&w, c->deps[i].from, c->deps[i].to, 1, err, sizeof err);
    world_build(&w, &arr, &n, &g);
    int rc = sched_run(arr, n, g, c->algo, c->quantum, c->high, &r);
    printf("[%s]\n", c->name);
    CHECK(rc == 0, "sched_run failed: %s", r.error);
    if (rc == 0) {
        for (int i = 0; i < r.nseg; i++)
            used += (size_t)snprintf(got + used, sizeof got - used, "%s%s:%d-%d", i ? "," : "", r.seg[i].idle ? "IDLE" : r.seg[i].pid, r.seg[i].start, r.seg[i].end);
        CHECK(strcmp(got, c->gantt) == 0, "gantt\n    got      %s\n    expected %s", got, c->gantt);
        for (int i = 0; i < n; i++) {
            CHECK(arr[i]->completion == c->ct[i] && arr[i]->turnaround == c->tat[i] && arr[i]->waiting == c->wt[i] && arr[i]->response == c->rt[i],
                  "%s CT/TAT/WT/RT got %d/%d/%d/%d expected %d/%d/%d/%d", arr[i]->pid, arr[i]->completion, arr[i]->turnaround,
                  arr[i]->waiting, arr[i]->response, c->ct[i], c->tat[i], c->wt[i], c->rt[i]);
            CHECK(arr[i]->turnaround == arr[i]->completion - arr[i]->arrival && arr[i]->waiting == arr[i]->turnaround - arr[i]->burst,
                  "%s formulas TAT=CT-AT, WT=TAT-BT", arr[i]->pid);
        }
        CHECK(fabs(r.stats.avg_wait - c->avg_wait) < 1e-9 && fabs(r.stats.avg_tat - c->avg_tat) < 1e-9, "averages wait %.3f tat %.3f", r.stats.avg_wait, r.stats.avg_tat);
        CHECK(fabs(r.stats.cpu_util - c->cpu) < 1e-9, "cpu utilisation %.3f expected %.3f", r.stats.cpu_util, c->cpu);
        CHECK(fabs(r.stats.throughput - (double)n / r.stats.total_time) < 1e-12, "throughput");
    }
    sched_result_free(&r); free(arr); graph_free(g); world_free(&w);
}

static const P DS_textbook[] = {{"P1",0,8,3},{"P2",1,4,1},{"P3",2,9,4},{"P4",3,5,2}};
static const P DS_all_at_zero[] = {{"P1",0,6,2},{"P2",0,8,1},{"P3",0,7,3},{"P4",0,3,2}};
static const P DS_simultaneous[] = {{"A",2,3,2},{"B",2,3,1},{"C",2,1,1},{"D",2,3,1}};
static const P DS_idle_gaps[] = {{"P1",2,2,1},{"P2",8,3,2},{"P3",9,1,1}};
static const P DS_single[] = {{"P1",3,5,1}};
static const P DS_long_burst[] = {{"P1",0,500,2},{"P2",10,20,1},{"P3",20,3,3}};
static const P DS_with_deps[] = {{"P1",0,3,2},{"P2",0,2,1},{"P3",0,4,3},{"P4",1,2,1}};
static const E DEP_with_deps[] = {{"P1","P3"},{"P2","P4"}};

static const Case CASES[] = {
  {"textbook / fcfs",ALGO_FCFS,2,0,4,0,DS_textbook,NULL,"P1:0-8,P2:8-12,P3:12-21,P4:21-26",{8,12,21,26,0,0,0,0},{8,11,19,23,0,0,0,0},{0,7,10,18,0,0,0,0},{0,7,10,18,0,0,0,0},8.75,15.25,100.0},
  {"textbook / sjf",ALGO_SJF,2,0,4,0,DS_textbook,NULL,"P1:0-8,P2:8-12,P4:12-17,P3:17-26",{8,12,26,17,0,0,0,0},{8,11,24,14,0,0,0,0},{0,7,15,9,0,0,0,0},{0,7,15,9,0,0,0,0},7.75,14.25,100.0},
  {"textbook / srtf",ALGO_SRTF,2,0,4,0,DS_textbook,NULL,"P1:0-1,P2:1-5,P4:5-10,P1:10-17,P3:17-26",{17,5,26,10,0,0,0,0},{17,4,24,7,0,0,0,0},{9,0,15,2,0,0,0,0},{0,0,15,2,0,0,0,0},6.5,13.0,100.0},
  {"textbook / prio_np",ALGO_PRIO_NP,2,0,4,0,DS_textbook,NULL,"P1:0-8,P2:8-12,P4:12-17,P3:17-26",{8,12,26,17,0,0,0,0},{8,11,24,14,0,0,0,0},{0,7,15,9,0,0,0,0},{0,7,15,9,0,0,0,0},7.75,14.25,100.0},
  {"textbook / prio_p",ALGO_PRIO_P,2,0,4,0,DS_textbook,NULL,"P1:0-1,P2:1-5,P4:5-10,P1:10-17,P3:17-26",{17,5,26,10,0,0,0,0},{17,4,24,7,0,0,0,0},{9,0,15,2,0,0,0,0},{0,0,15,2,0,0,0,0},6.5,13.0,100.0},
  {"textbook / rr q=2",ALGO_RR,2,0,4,0,DS_textbook,NULL,"P1:0-2,P2:2-4,P3:4-6,P1:6-8,P4:8-10,P2:10-12,P3:12-14,P1:14-16,P4:16-18,P3:18-20,P1:20-22,P4:22-23,P3:23-25,P3:25-26",{22,12,26,23,0,0,0,0},{22,11,24,20,0,0,0,0},{14,7,15,15,0,0,0,0},{0,1,2,5,0,0,0,0},12.75,19.25,100.0},
  {"all_at_zero / fcfs",ALGO_FCFS,2,0,4,0,DS_all_at_zero,NULL,"P1:0-6,P2:6-14,P3:14-21,P4:21-24",{6,14,21,24,0,0,0,0},{6,14,21,24,0,0,0,0},{0,6,14,21,0,0,0,0},{0,6,14,21,0,0,0,0},10.25,16.25,100.0},
  {"all_at_zero / sjf",ALGO_SJF,2,0,4,0,DS_all_at_zero,NULL,"P4:0-3,P1:3-9,P3:9-16,P2:16-24",{9,24,16,3,0,0,0,0},{9,24,16,3,0,0,0,0},{3,16,9,0,0,0,0,0},{3,16,9,0,0,0,0,0},7.0,13.0,100.0},
  {"all_at_zero / srtf",ALGO_SRTF,2,0,4,0,DS_all_at_zero,NULL,"P4:0-3,P1:3-9,P3:9-16,P2:16-24",{9,24,16,3,0,0,0,0},{9,24,16,3,0,0,0,0},{3,16,9,0,0,0,0,0},{3,16,9,0,0,0,0,0},7.0,13.0,100.0},
  {"all_at_zero / prio_np",ALGO_PRIO_NP,2,0,4,0,DS_all_at_zero,NULL,"P2:0-8,P1:8-14,P4:14-17,P3:17-24",{14,8,24,17,0,0,0,0},{14,8,24,17,0,0,0,0},{8,0,17,14,0,0,0,0},{8,0,17,14,0,0,0,0},9.75,15.75,100.0},
  {"all_at_zero / prio_p",ALGO_PRIO_P,2,0,4,0,DS_all_at_zero,NULL,"P2:0-8,P1:8-14,P4:14-17,P3:17-24",{14,8,24,17,0,0,0,0},{14,8,24,17,0,0,0,0},{8,0,17,14,0,0,0,0},{8,0,17,14,0,0,0,0},9.75,15.75,100.0},
  {"all_at_zero / rr q=2",ALGO_RR,2,0,4,0,DS_all_at_zero,NULL,"P1:0-2,P2:2-4,P3:4-6,P4:6-8,P1:8-10,P2:10-12,P3:12-14,P4:14-15,P1:15-17,P2:17-19,P3:19-21,P2:21-23,P3:23-24",{17,23,24,15,0,0,0,0},{17,23,24,15,0,0,0,0},{11,15,17,12,0,0,0,0},{0,2,4,6,0,0,0,0},13.75,19.75,100.0},
  {"simultaneous / fcfs",ALGO_FCFS,2,0,4,0,DS_simultaneous,NULL,"IDLE:0-2,A:2-5,B:5-8,C:8-9,D:9-12",{5,8,9,12,0,0,0,0},{3,6,7,10,0,0,0,0},{0,3,6,7,0,0,0,0},{0,3,6,7,0,0,0,0},4.0,6.5,83.33333333333333},
  {"simultaneous / sjf",ALGO_SJF,2,0,4,0,DS_simultaneous,NULL,"IDLE:0-2,C:2-3,A:3-6,B:6-9,D:9-12",{6,9,3,12,0,0,0,0},{4,7,1,10,0,0,0,0},{1,4,0,7,0,0,0,0},{1,4,0,7,0,0,0,0},3.0,5.5,83.33333333333333},
  {"simultaneous / srtf",ALGO_SRTF,2,0,4,0,DS_simultaneous,NULL,"IDLE:0-2,C:2-3,A:3-6,B:6-9,D:9-12",{6,9,3,12,0,0,0,0},{4,7,1,10,0,0,0,0},{1,4,0,7,0,0,0,0},{1,4,0,7,0,0,0,0},3.0,5.5,83.33333333333333},
  {"simultaneous / prio_np",ALGO_PRIO_NP,2,0,4,0,DS_simultaneous,NULL,"IDLE:0-2,B:2-5,C:5-6,D:6-9,A:9-12",{12,5,6,9,0,0,0,0},{10,3,4,7,0,0,0,0},{7,0,3,4,0,0,0,0},{7,0,3,4,0,0,0,0},3.5,6.0,83.33333333333333},
  {"simultaneous / prio_p",ALGO_PRIO_P,2,0,4,0,DS_simultaneous,NULL,"IDLE:0-2,B:2-5,C:5-6,D:6-9,A:9-12",{12,5,6,9,0,0,0,0},{10,3,4,7,0,0,0,0},{7,0,3,4,0,0,0,0},{7,0,3,4,0,0,0,0},3.5,6.0,83.33333333333333},
  {"simultaneous / rr q=2",ALGO_RR,2,0,4,0,DS_simultaneous,NULL,"IDLE:0-2,A:2-4,B:4-6,C:6-7,D:7-9,A:9-10,B:10-11,D:11-12",{10,11,7,12,0,0,0,0},{8,9,5,10,0,0,0,0},{5,6,4,7,0,0,0,0},{0,2,4,5,0,0,0,0},5.5,8.0,83.33333333333333},
  {"idle_gaps / fcfs",ALGO_FCFS,2,0,3,0,DS_idle_gaps,NULL,"IDLE:0-2,P1:2-4,IDLE:4-8,P2:8-11,P3:11-12",{4,11,12,0,0,0,0,0},{2,3,3,0,0,0,0,0},{0,0,2,0,0,0,0,0},{0,0,2,0,0,0,0,0},0.6666666666666666,2.6666666666666665,50.0},
  {"idle_gaps / sjf",ALGO_SJF,2,0,3,0,DS_idle_gaps,NULL,"IDLE:0-2,P1:2-4,IDLE:4-8,P2:8-11,P3:11-12",{4,11,12,0,0,0,0,0},{2,3,3,0,0,0,0,0},{0,0,2,0,0,0,0,0},{0,0,2,0,0,0,0,0},0.6666666666666666,2.6666666666666665,50.0},
  {"idle_gaps / srtf",ALGO_SRTF,2,0,3,0,DS_idle_gaps,NULL,"IDLE:0-2,P1:2-4,IDLE:4-8,P2:8-9,P3:9-10,P2:10-12",{4,12,10,0,0,0,0,0},{2,4,1,0,0,0,0,0},{0,1,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},0.3333333333333333,2.3333333333333335,50.0},
  {"idle_gaps / prio_np",ALGO_PRIO_NP,2,0,3,0,DS_idle_gaps,NULL,"IDLE:0-2,P1:2-4,IDLE:4-8,P2:8-11,P3:11-12",{4,11,12,0,0,0,0,0},{2,3,3,0,0,0,0,0},{0,0,2,0,0,0,0,0},{0,0,2,0,0,0,0,0},0.6666666666666666,2.6666666666666665,50.0},
  {"idle_gaps / prio_p",ALGO_PRIO_P,2,0,3,0,DS_idle_gaps,NULL,"IDLE:0-2,P1:2-4,IDLE:4-8,P2:8-9,P3:9-10,P2:10-12",{4,12,10,0,0,0,0,0},{2,4,1,0,0,0,0,0},{0,1,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},0.3333333333333333,2.3333333333333335,50.0},
  {"idle_gaps / rr q=2",ALGO_RR,2,0,3,0,DS_idle_gaps,NULL,"IDLE:0-2,P1:2-4,IDLE:4-8,P2:8-10,P3:10-11,P2:11-12",{4,12,11,0,0,0,0,0},{2,4,2,0,0,0,0,0},{0,1,1,0,0,0,0,0},{0,0,1,0,0,0,0,0},0.6666666666666666,2.6666666666666665,50.0},
  {"single / fcfs",ALGO_FCFS,2,0,1,0,DS_single,NULL,"IDLE:0-3,P1:3-8",{8,0,0,0,0,0,0,0},{5,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},0.0,5.0,62.5},
  {"single / sjf",ALGO_SJF,2,0,1,0,DS_single,NULL,"IDLE:0-3,P1:3-8",{8,0,0,0,0,0,0,0},{5,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},0.0,5.0,62.5},
  {"single / srtf",ALGO_SRTF,2,0,1,0,DS_single,NULL,"IDLE:0-3,P1:3-8",{8,0,0,0,0,0,0,0},{5,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},0.0,5.0,62.5},
  {"single / prio_np",ALGO_PRIO_NP,2,0,1,0,DS_single,NULL,"IDLE:0-3,P1:3-8",{8,0,0,0,0,0,0,0},{5,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},0.0,5.0,62.5},
  {"single / prio_p",ALGO_PRIO_P,2,0,1,0,DS_single,NULL,"IDLE:0-3,P1:3-8",{8,0,0,0,0,0,0,0},{5,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},0.0,5.0,62.5},
  {"single / rr q=2",ALGO_RR,2,0,1,0,DS_single,NULL,"IDLE:0-3,P1:3-5,P1:5-7,P1:7-8",{8,0,0,0,0,0,0,0},{5,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},0.0,5.0,62.5},
  {"long_burst / fcfs",ALGO_FCFS,2,0,3,0,DS_long_burst,NULL,"P1:0-500,P2:500-520,P3:520-523",{500,520,523,0,0,0,0,0},{500,510,503,0,0,0,0,0},{0,490,500,0,0,0,0,0},{0,490,500,0,0,0,0,0},330.0,504.3333333333333,100.0},
  {"long_burst / sjf",ALGO_SJF,2,0,3,0,DS_long_burst,NULL,"P1:0-500,P3:500-503,P2:503-523",{500,523,503,0,0,0,0,0},{500,513,483,0,0,0,0,0},{0,493,480,0,0,0,0,0},{0,493,480,0,0,0,0,0},324.3333333333333,498.6666666666667,100.0},
  {"long_burst / srtf",ALGO_SRTF,2,0,3,0,DS_long_burst,NULL,"P1:0-10,P2:10-20,P3:20-23,P2:23-33,P1:33-523",{523,33,23,0,0,0,0,0},{523,23,3,0,0,0,0,0},{23,3,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},8.666666666666666,183.0,100.0},
  {"long_burst / prio_np",ALGO_PRIO_NP,2,0,3,0,DS_long_burst,NULL,"P1:0-500,P2:500-520,P3:520-523",{500,520,523,0,0,0,0,0},{500,510,503,0,0,0,0,0},{0,490,500,0,0,0,0,0},{0,490,500,0,0,0,0,0},330.0,504.3333333333333,100.0},
  {"long_burst / prio_p",ALGO_PRIO_P,2,0,3,0,DS_long_burst,NULL,"P1:0-10,P2:10-30,P1:30-520,P3:520-523",{520,30,523,0,0,0,0,0},{520,20,503,0,0,0,0,0},{20,0,500,0,0,0,0,0},{0,0,500,0,0,0,0,0},173.33333333333334,347.6666666666667,100.0},
  {"long_burst / rr q=2",ALGO_RR,2,0,3,0,DS_long_burst,NULL,"P1:0-2,P1:2-4,P1:4-6,P1:6-8,P1:8-10,P2:10-12,P1:12-14,P2:14-16,P1:16-18,P2:18-20,P1:20-22,P3:22-24,P2:24-26,P1:26-28,P3:28-29,P2:29-31,P1:31-33,P2:33-35,P1:35-37,P2:37-39,P1:39-41,P2:41-43,P1:43-45,P2:45-47,P1:47-49,P2:49-51,P1:51-53,P1:53-55,P1:55-57,P1:57-59,P1:59-61,P1:61-63,P1:63-65,P1:65-67,P1:67-69,P1:69-71,P1:71-73,P1:73-75,P1:75-77,P1:77-79,P1:79-81,P1:81-83,P1:83-85,P1:85-87,P1:87-89,P1:89-91,P1:91-93,P1:93-95,P1:95-97,P1:97-99,P1:99-101,P1:101-103,P1:103-105,P1:105-107,P1:107-109,P1:109-111,P1:111-113,P1:113-115,P1:115-117,P1:117-119,P1:119-121,P1:121-123,P1:123-125,P1:125-127,P1:127-129,P1:129-131,P1:131-133,P1:133-135,P1:135-137,P1:137-139,P1:139-141,P1:141-143,P1:143-145,P1:145-147,P1:147-149,P1:149-151,P1:151-153,P1:153-155,P1:155-157,P1:157-159,P1:159-161,P1:161-163,P1:163-165,P1:165-167,P1:167-169,P1:169-171,P1:171-173,P1:173-175,P1:175-177,P1:177-179,P1:179-181,P1:181-183,P1:183-185,P1:185-187,P1:187-189,P1:189-191,P1:191-193,P1:193-195,P1:195-197,P1:197-199,P1:199-201,P1:201-203,P1:203-205,P1:205-207,P1:207-209,P1:209-211,P1:211-213,P1:213-215,P1:215-217,P1:217-219,P1:219-221,P1:221-223,P1:223-225,P1:225-227,P1:227-229,P1:229-231,P1:231-233,P1:233-235,P1:235-237,P1:237-239,P1:239-241,P1:241-243,P1:243-245,P1:245-247,P1:247-249,P1:249-251,P1:251-253,P1:253-255,P1:255-257,P1:257-259,P1:259-261,P1:261-263,P1:263-265,P1:265-267,P1:267-269,P1:269-271,P1:271-273,P1:273-275,P1:275-277,P1:277-279,P1:279-281,P1:281-283,P1:283-285,P1:285-287,P1:287-289,P1:289-291,P1:291-293,P1:293-295,P1:295-297,P1:297-299,P1:299-301,P1:301-303,P1:303-305,P1:305-307,P1:307-309,P1:309-311,P1:311-313,P1:313-315,P1:315-317,P1:317-319,P1:319-321,P1:321-323,P1:323-325,P1:325-327,P1:327-329,P1:329-331,P1:331-333,P1:333-335,P1:335-337,P1:337-339,P1:339-341,P1:341-343,P1:343-345,P1:345-347,P1:347-349,P1:349-351,P1:351-353,P1:353-355,P1:355-357,P1:357-359,P1:359-361,P1:361-363,P1:363-365,P1:365-367,P1:367-369,P1:369-371,P1:371-373,P1:373-375,P1:375-377,P1:377-379,P1:379-381,P1:381-383,P1:383-385,P1:385-387,P1:387-389,P1:389-391,P1:391-393,P1:393-395,P1:395-397,P1:397-399,P1:399-401,P1:401-403,P1:403-405,P1:405-407,P1:407-409,P1:409-411,P1:411-413,P1:413-415,P1:415-417,P1:417-419,P1:419-421,P1:421-423,P1:423-425,P1:425-427,P1:427-429,P1:429-431,P1:431-433,P1:433-435,P1:435-437,P1:437-439,P1:439-441,P1:441-443,P1:443-445,P1:445-447,P1:447-449,P1:449-451,P1:451-453,P1:453-455,P1:455-457,P1:457-459,P1:459-461,P1:461-463,P1:463-465,P1:465-467,P1:467-469,P1:469-471,P1:471-473,P1:473-475,P1:475-477,P1:477-479,P1:479-481,P1:481-483,P1:483-485,P1:485-487,P1:487-489,P1:489-491,P1:491-493,P1:493-495,P1:495-497,P1:497-499,P1:499-501,P1:501-503,P1:503-505,P1:505-507,P1:507-509,P1:509-511,P1:511-513,P1:513-515,P1:515-517,P1:517-519,P1:519-521,P1:521-523",{523,51,29,0,0,0,0,0},{523,41,9,0,0,0,0,0},{23,21,6,0,0,0,0,0},{0,0,2,0,0,0,0,0},16.666666666666668,191.0,100.0},
  {"with_deps / fcfs",ALGO_FCFS,2,0,4,2,DS_with_deps,DEP_with_deps,"P1:0-3,P2:3-5,P3:5-9,P4:9-11",{3,5,9,11,0,0,0,0},{3,5,9,10,0,0,0,0},{0,3,5,8,0,0,0,0},{0,3,5,8,0,0,0,0},4.0,6.75,100.0},
  {"with_deps / sjf",ALGO_SJF,2,0,4,2,DS_with_deps,DEP_with_deps,"P2:0-2,P4:2-4,P1:4-7,P3:7-11",{7,2,11,4,0,0,0,0},{7,2,11,3,0,0,0,0},{4,0,7,1,0,0,0,0},{4,0,7,1,0,0,0,0},3.0,5.75,100.0},
  {"with_deps / srtf",ALGO_SRTF,2,0,4,2,DS_with_deps,DEP_with_deps,"P2:0-2,P4:2-4,P1:4-7,P3:7-11",{7,2,11,4,0,0,0,0},{7,2,11,3,0,0,0,0},{4,0,7,1,0,0,0,0},{4,0,7,1,0,0,0,0},3.0,5.75,100.0},
  {"with_deps / prio_np",ALGO_PRIO_NP,2,0,4,2,DS_with_deps,DEP_with_deps,"P2:0-2,P4:2-4,P1:4-7,P3:7-11",{7,2,11,4,0,0,0,0},{7,2,11,3,0,0,0,0},{4,0,7,1,0,0,0,0},{4,0,7,1,0,0,0,0},3.0,5.75,100.0},
  {"with_deps / prio_p",ALGO_PRIO_P,2,0,4,2,DS_with_deps,DEP_with_deps,"P2:0-2,P4:2-4,P1:4-7,P3:7-11",{7,2,11,4,0,0,0,0},{7,2,11,3,0,0,0,0},{4,0,7,1,0,0,0,0},{4,0,7,1,0,0,0,0},3.0,5.75,100.0},
  {"with_deps / rr q=2",ALGO_RR,2,0,4,2,DS_with_deps,DEP_with_deps,"P1:0-2,P2:2-4,P1:4-5,P4:5-7,P3:7-9,P3:9-11",{5,4,11,7,0,0,0,0},{5,4,11,6,0,0,0,0},{2,2,7,4,0,0,0,0},{0,2,7,4,0,0,0,0},3.75,6.5,100.0},
  {"textbook / rr q=1",ALGO_RR,1,0,4,0,DS_textbook,NULL,"P1:0-1,P2:1-2,P1:2-3,P3:3-4,P2:4-5,P4:5-6,P1:6-7,P3:7-8,P2:8-9,P4:9-10,P1:10-11,P3:11-12,P2:12-13,P4:13-14,P1:14-15,P3:15-16,P4:16-17,P1:17-18,P3:18-19,P4:19-20,P1:20-21,P3:21-22,P1:22-23,P3:23-24,P3:24-25,P3:25-26",{23,13,26,20,0,0,0,0},{23,12,24,17,0,0,0,0},{15,8,15,12,0,0,0,0},{0,0,1,2,0,0,0,0},12.5,19.0,100.0},
  {"textbook / rr q=3",ALGO_RR,3,0,4,0,DS_textbook,NULL,"P1:0-3,P2:3-6,P3:6-9,P4:9-12,P1:12-15,P2:15-16,P3:16-19,P4:19-21,P1:21-23,P3:23-26",{23,16,26,21,0,0,0,0},{23,15,24,18,0,0,0,0},{15,11,15,13,0,0,0,0},{0,2,4,6,0,0,0,0},13.5,20.0,100.0},
  {"textbook / rr q=4",ALGO_RR,4,0,4,0,DS_textbook,NULL,"P1:0-4,P2:4-8,P3:8-12,P4:12-16,P1:16-20,P3:20-24,P4:24-25,P3:25-26",{20,8,26,25,0,0,0,0},{20,7,24,22,0,0,0,0},{12,3,15,17,0,0,0,0},{0,3,6,9,0,0,0,0},11.75,18.25,100.0},
  {"textbook / rr q=100",ALGO_RR,100,0,4,0,DS_textbook,NULL,"P1:0-8,P2:8-12,P3:12-21,P4:21-26",{8,12,21,26,0,0,0,0},{8,11,19,23,0,0,0,0},{0,7,10,18,0,0,0,0},{0,7,10,18,0,0,0,0},8.75,15.25,100.0},
  {"textbook / prio_np high-number-first",ALGO_PRIO_NP,2,1,4,0,DS_textbook,NULL,"P1:0-8,P3:8-17,P4:17-22,P2:22-26",{8,26,17,22,0,0,0,0},{8,25,15,19,0,0,0,0},{0,21,6,14,0,0,0,0},{0,21,6,14,0,0,0,0},10.25,16.75,100.0},
  {"textbook / prio_p high-number-first",ALGO_PRIO_P,2,1,4,0,DS_textbook,NULL,"P1:0-2,P3:2-11,P1:11-17,P4:17-22,P2:22-26",{17,26,11,22,0,0,0,0},{17,25,9,19,0,0,0,0},{9,21,0,14,0,0,0,0},{0,21,0,14,0,0,0,0},11.0,17.5,100.0},
  {"simultaneous / prio_np high-number-first",ALGO_PRIO_NP,2,1,4,0,DS_simultaneous,NULL,"IDLE:0-2,A:2-5,B:5-8,C:8-9,D:9-12",{5,8,9,12,0,0,0,0},{3,6,7,10,0,0,0,0},{0,3,6,7,0,0,0,0},{0,3,6,7,0,0,0,0},4.0,6.5,83.33333333333333},
  {"simultaneous / prio_p high-number-first",ALGO_PRIO_P,2,1,4,0,DS_simultaneous,NULL,"IDLE:0-2,A:2-5,B:5-8,C:8-9,D:9-12",{5,8,9,12,0,0,0,0},{3,6,7,10,0,0,0,0},{0,3,6,7,0,0,0,0},{0,3,6,7,0,0,0,0},4.0,6.5,83.33333333333333},
  {"simultaneous / rr q=1",ALGO_RR,1,0,4,0,DS_simultaneous,NULL,"IDLE:0-2,A:2-3,B:3-4,C:4-5,D:5-6,A:6-7,B:7-8,D:8-9,A:9-10,B:10-11,D:11-12",{10,11,5,12,0,0,0,0},{8,9,3,10,0,0,0,0},{5,6,2,7,0,0,0,0},{0,1,2,3,0,0,0,0},5.0,7.5,83.33333333333333},
};

/* ---------- data structure unit tests ---------- */
static void test_structures(void) {
    printf("[data structures]\n");
    Process a, b, c, d; memset(&a, 0, sizeof a); b = c = d = a;
    strcpy(a.pid, "A"); a.burst = 5; a.index = 0; strcpy(b.pid, "B"); b.burst = 2; b.index = 1;
    strcpy(c.pid, "C"); c.burst = 9; c.index = 2; strcpy(d.pid, "D"); d.burst = 2; d.index = 3;
    /* min-heap */
    MinHeap h; heap_init(&h, 4, HEAP_BURST);
    heap_insert(&h, &a); heap_insert(&h, &c); heap_insert(&h, &d); heap_insert(&h, &b);
    CHECK(heap_insert(&h, &a) == -1, "heap overflow detected");
    CHECK(heap_extract_min(&h) == &b && heap_extract_min(&h) == &d && heap_extract_min(&h) == &a && heap_extract_min(&h) == &c, "heap extracts in order B, D, A, C");
    CHECK(heap_extract_min(&h) == NULL, "empty heap returns NULL"); heap_free(&h);
    /* circular queue wrap-around */
    CircularQueue cq; cq_init(&cq, 3);
    cq_enqueue(&cq, &a); cq_enqueue(&cq, &b); cq_enqueue(&cq, &c);
    CHECK(cq_enqueue(&cq, &d) == -1, "circular queue full");
    CHECK(cq_dequeue(&cq) == &a && cq_enqueue(&cq, &d) == 0, "wrap-around enqueue");
    CHECK(cq_dequeue(&cq) == &b && cq_dequeue(&cq) == &c && cq_dequeue(&cq) == &d && cq_empty(&cq), "FIFO order after wrap");
    cq_free(&cq);
    /* linked queue + stack */
    Queue q; queue_init(&q); queue_enqueue(&q, &a); queue_enqueue(&q, &b);
    CHECK(queue_peek(&q) == &a && queue_dequeue(&q) == &a && queue_dequeue(&q) == &b && queue_dequeue(&q) == NULL, "linked queue FIFO");
    queue_free(&q);
    Stack s; char pid[PID_LEN]; int t; stack_init(&s);
    stack_push(&s, "P1", 0); stack_push(&s, "P2", 4); stack_push(&s, "P3", 7);
    CHECK(stack_peek(&s, pid, &t) && !strcmp(pid, "P3") && t == 7, "stack peek");
    CHECK(stack_pop(&s, pid, &t) && !strcmp(pid, "P3") && stack_pop(&s, pid, &t) && !strcmp(pid, "P2") && stack_pop(&s, pid, &t) && !stack_pop(&s, pid, &t), "stack LIFO");
    stack_free(&s);
    /* linked list + hash table (with a real collision) */
    World w; char err[200]; world_init(&w);
    char k1[PID_LEN] = "P1", k2[PID_LEN] = "";
    for (int i = 0; i < 200 && !k2[0]; i++) { char t2[PID_LEN]; snprintf(t2, sizeof t2, "Q%d", i); if (ht_hash(t2) == ht_hash(k1)) strcpy(k2, t2); }
    CHECK(world_add_process(&w, k1, 0, 3, 1, err, sizeof err) == 0 && world_add_process(&w, k2, 1, 4, 2, err, sizeof err) == 0, "insert colliding PIDs");
    CHECK(ht_collisions(&w.ht) == 1 && ht_get(&w.ht, k1) && ht_get(&w.ht, k2) != ht_get(&w.ht, k1), "collision handled by chaining");
    CHECK(world_add_process(&w, k1, 0, 3, 1, err, sizeof err) != 0, "duplicate PID rejected");
    CHECK(world_add_process(&w, "bad pid", 0, 3, 1, err, sizeof err) != 0 && world_add_process(&w, "Z", 0, 0, 1, err, sizeof err) != 0 && world_add_process(&w, "Z", -1, 2, 1, err, sizeof err) != 0, "invalid input rejected");
    CHECK(pl_search(&w.list, k2) == ht_get(&w.ht, k2), "list search and hash lookup agree");
    CHECK(world_delete_process(&w, k1) == 0 && !ht_get(&w.ht, k1) && !pl_search(&w.list, k1) && w.list.count == 1, "delete from list + hash");
    world_free(&w);
    /* graph: topo order and cycle detection */
    world_init(&w);
    world_add_process(&w, "A", 0, 1, 1, err, sizeof err); world_add_process(&w, "B", 0, 1, 1, err, sizeof err); world_add_process(&w, "C", 0, 1, 1, err, sizeof err);
    CHECK(world_add_dep(&w, "A", "B", 1, err, sizeof err) == 0 && world_add_dep(&w, "B", "C", 1, err, sizeof err) == 0, "valid dependencies");
    CHECK(world_add_dep(&w, "C", "A", 1, err, sizeof err) != 0 && strstr(err, "Circular"), "circular dependency rejected: %s", err);
    Process **arr; int n; Graph *g; int order[3];
    world_build(&w, &arr, &n, &g);
    CHECK(graph_topo_sort(g, order) == 3 && order[0] == 0 && order[1] == 1 && order[2] == 2, "topological order A, B, C");
    graph_free(g); free(arr);
    world_add_dep(&w, "C", "A", 0, err, sizeof err);               /* force a cycle, bypassing the check */
    world_build(&w, &arr, &n, &g);
    SimResult r;
    CHECK(sched_run(arr, n, g, ALGO_FCFS, 2, 0, &r) != 0 && strstr(r.error, "Circular"), "scheduler refuses cyclic dependencies: %s", r.error);
    sched_result_free(&r); graph_free(g); free(arr); world_free(&w);
}

int main(void) {
    test_structures();
    for (size_t i = 0; i < sizeof CASES / sizeof CASES[0]; i++) run_case(&CASES[i]);
    printf("\n%d checks passed, %d failed\n", g_pass, g_fail);
    return g_fail ? 1 : 0;
}
