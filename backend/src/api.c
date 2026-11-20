#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "api.h"
#include "process.h"
#include "scheduler.h"

static World G;      /* the Process Management dataset (linked list + hash table + dependency edges) */

int api_init(void) {
    if (world_init(&G) != 0) return -1;
    world_load_sample(&G, 0);
    return 0;
}
void api_shutdown(void) { world_free(&G); }

/* ---------------- small helpers ---------------- */
static char *xstrdup(const char *s) {
    size_t n = strlen(s) + 1; char *p = (char *)malloc(n);
    if (p) memcpy(p, s, n);
    return p;
}

static int hexv(int c) { return isdigit(c) ? c - '0' : (tolower(c) - 'a' + 10); }

static void url_decode(const char *src, size_t n, char *dst) {
    size_t j = 0;
    for (size_t i = 0; i < n; i++) {
        if (src[i] == '+') dst[j++] = ' ';
        else if (src[i] == '%' && i + 2 < n + 0 && isxdigit((unsigned char)src[i + 1]) && isxdigit((unsigned char)src[i + 2])) {
            dst[j++] = (char)(hexv((unsigned char)src[i + 1]) * 16 + hexv((unsigned char)src[i + 2])); i += 2;
        } else dst[j++] = src[i];
    }
    dst[j] = '\0';
}

/* Returns a malloc'd, decoded value for key in "a=1&b=2" (NULL if absent). */
static char *form_get(const char *s, const char *key) {
    size_t kl = strlen(key);
    while (s && *s) {
        const char *amp = strchr(s, '&');
        size_t seg = amp ? (size_t)(amp - s) : strlen(s);
        if (seg > kl && strncmp(s, key, kl) == 0 && s[kl] == '=') {
            size_t vl = seg - kl - 1;
            char *v = (char *)malloc(vl + 1);
            if (!v) return NULL;
            url_decode(s + kl + 1, vl, v);
            return v;
        }
        s = amp ? amp + 1 : NULL;
    }
    return NULL;
}

/* Reads an integer field; absent/invalid -> error text. */
static int form_int(const char *s, const char *key, int *out, char *err, size_t en) {
    char *v = form_get(s, key), *end;
    if (!v || !*v) { snprintf(err, en, "Missing field '%s'", key); free(v); return -1; }
    long x = strtol(v, &end, 10);
    if (*end != '\0' || x < -1000000 || x > 1000000) { snprintf(err, en, "Field '%s' must be a whole number", key); free(v); return -1; }
    *out = (int)x; free(v);
    return 0;
}

static void reply_error(StrBuf *out, int *status, int code, const char *msg) {
    *status = code;
    sb_append(out, "{\"ok\":false,\"error\":");
    sb_json_string(out, msg);
    sb_append(out, "}");
}

static void json_proc(StrBuf *sb, const Process *p) {
    sb_append(sb, "{\"pid\":"); sb_json_string(sb, p->pid);
    sb_printf(sb, ",\"arrival\":%d,\"burst\":%d,\"priority\":%d}", p->arrival, p->burst, p->priority);
}

static int contains_nocase(const char *hay, const char *needle) {
    size_t hl = strlen(hay), nl = strlen(needle);
    if (nl == 0) return 1;
    for (size_t i = 0; i + nl <= hl; i++) {
        size_t k = 0;
        while (k < nl && tolower((unsigned char)hay[i + k]) == tolower((unsigned char)needle[k])) k++;
        if (k == nl) return 1;
    }
    return 0;
}

static void json_process_list(StrBuf *out, const char *q) {
    int first = 1, shown = 0;
    sb_append(out, "{\"ok\":true,\"processes\":[");
    for (Process *p = G.list.head; p; p = p->next) {          /* linked-list traversal + search */
        if (q && *q && !contains_nocase(p->pid, q)) continue;
        if (!first) sb_append(out, ",");
        json_proc(out, p); first = 0; shown++;
    }
    sb_printf(out, "],\"count\":%d,\"total\":%d}", shown, G.list.count);
}

static void json_deps(StrBuf *out) {
    sb_append(out, "{\"ok\":true,\"deps\":[");
    for (int i = 0; i < G.ndeps; i++) {
        sb_printf(out, "%s{\"from\":", i ? "," : ""); sb_json_string(out, G.deps[i].from);
        sb_append(out, ",\"to\":"); sb_json_string(out, G.deps[i].to); sb_append(out, "}");
    }
    sb_append(out, "]}");
}

/* ---------------- /api/processes ---------------- */
static void route_processes(const HttpRequest *rq, const char *pidpart, StrBuf *out, int *status) {
    char err[256] = "";
    const char *m = rq->method;
    if (!pidpart) {
        if (!strcmp(m, "GET")) { char *q = form_get(rq->query, "q"); json_process_list(out, q); free(q); return; }
        if (!strcmp(m, "POST")) {
            char *pid = form_get(rq->body, "pid"); int a, b, pr;
            if (!pid) { reply_error(out, status, 400, "Missing field 'pid'"); return; }
            if (form_int(rq->body, "arrival", &a, err, sizeof err) || form_int(rq->body, "burst", &b, err, sizeof err) ||
                form_int(rq->body, "priority", &pr, err, sizeof err) || world_add_process(&G, pid, a, b, pr, err, sizeof err)) {
                reply_error(out, status, 400, err); free(pid); return;
            }
            free(pid); *status = 201; json_process_list(out, NULL); return;
        }
        reply_error(out, status, 405, "Method not allowed"); return;
    }
    if (!strcmp(m, "GET")) {                                    /* O(1) lookup through the hash table */
        Process *p = ht_get(&G.ht, pidpart);
        if (!p) { reply_error(out, status, 404, "Process not found"); return; }
        sb_append(out, "{\"ok\":true,\"process\":"); json_proc(out, p);
        sb_printf(out, ",\"bucket\":%u}", ht_hash(pidpart)); return;
    }
    if (!strcmp(m, "PUT")) {
        int a, b, pr;
        if (form_int(rq->body, "arrival", &a, err, sizeof err) || form_int(rq->body, "burst", &b, err, sizeof err) ||
            form_int(rq->body, "priority", &pr, err, sizeof err) || world_update_process(&G, pidpart, a, b, pr, err, sizeof err)) {
            reply_error(out, status, 400, err); return;
        }
        json_process_list(out, NULL); return;
    }
    if (!strcmp(m, "DELETE")) {
        if (world_delete_process(&G, pidpart) != 0) { reply_error(out, status, 404, "Process not found"); return; }
        json_process_list(out, NULL); return;
    }
    reply_error(out, status, 405, "Method not allowed");
}

/* ---------------- dependencies and graph ---------------- */
static void route_deps(const HttpRequest *rq, StrBuf *out, int *status) {
    char err[256] = "";
    if (!strcmp(rq->method, "GET")) { json_deps(out); return; }
    const char *src = !strcmp(rq->method, "POST") ? rq->body : rq->query;
    char *from = form_get(src, "from"), *to = form_get(src, "to");
    if (!from || !to) { reply_error(out, status, 400, "Fields 'from' and 'to' are required"); free(from); free(to); return; }
    if (!strcmp(rq->method, "POST")) {
        if (world_add_dep(&G, from, to, 1, err, sizeof err) != 0) reply_error(out, status, 400, err);
        else { *status = 201; json_deps(out); }
    } else if (!strcmp(rq->method, "DELETE")) {
        if (world_remove_dep(&G, from, to) != 0) reply_error(out, status, 404, "Dependency not found");
        else json_deps(out);
    } else reply_error(out, status, 405, "Method not allowed");
    free(from); free(to);
}

static void route_graph(StrBuf *out, int *status) {
    Process **arr; int n; Graph *g;
    if (world_build(&G, &arr, &n, &g) != 0) { reply_error(out, status, 500, "Out of memory"); return; }
    int *topo = (int *)malloc(sizeof(int) * (size_t)(n + 1)), *path = (int *)malloc(sizeof(int) * (size_t)(n + 2));
    int cnt = topo ? graph_topo_sort(g, topo) : 0, len = 0, cyc = (path && cnt < n) ? graph_find_cycle(g, path, &len) : 0;
    sb_append(out, "{\"ok\":true,\"nodes\":[");
    for (int i = 0; i < n; i++) {
        sb_printf(out, "%s{\"pid\":", i ? "," : ""); sb_json_string(out, arr[i]->pid);
        sb_printf(out, ",\"indegree\":%d,\"adj\":[", g->indeg[i]);
        for (GEdge *e = g->adj[i]; e; e = e->next) { sb_printf(out, "%s", e == g->adj[i] ? "" : ","); sb_json_string(out, arr[e->to]->pid); }
        sb_append(out, "]}");
    }
    sb_printf(out, "],\"hasCycle\":%s,\"topo\":[", cyc ? "true" : "false");
    for (int i = 0; i < cnt && topo; i++) { sb_printf(out, "%s", i ? "," : ""); sb_json_string(out, arr[topo[i]]->pid); }
    sb_append(out, "],\"cycle\":[");
    for (int i = 0; i < len && cyc; i++) { sb_printf(out, "%s", i ? "," : ""); sb_json_string(out, arr[path[i]]->pid); }
    sb_append(out, "]}");
    free(topo); free(path); free(arr); graph_free(g);
}

/* ---------------- simulation ---------------- */
static int parse_procs(World *w, const char *s, char *err, size_t en) {
    char *copy = xstrdup(s), *cur = copy;
    if (!copy) { snprintf(err, en, "Out of memory"); return -1; }
    int rc = 0;
    while (*cur && rc == 0) {
        char *semi = strchr(cur, ';');
        if (semi) *semi = '\0';
        if (*cur) {
            char pid[PID_LEN]; int a, b, p, used = 0;
            int cnt = sscanf(cur, "%15[^,],%d,%d,%d%n", pid, &a, &b, &p, &used);
            if (cnt != 4 || cur[used] != '\0') { snprintf(err, en, "Malformed process entry '%.40s'", cur); rc = -1; }
            else rc = world_add_process(w, pid, a, b, p, err, en);
        }
        cur = semi ? semi + 1 : cur + strlen(cur);
    }
    free(copy);
    return rc;
}

static int parse_deps(World *w, const char *s, char *err, size_t en) {
    char *copy = xstrdup(s), *cur = copy;
    if (!copy) { snprintf(err, en, "Out of memory"); return -1; }
    int rc = 0;
    while (*cur && rc == 0) {
        char *semi = strchr(cur, ';');
        if (semi) *semi = '\0';
        if (*cur) {
            char a[PID_LEN], b[PID_LEN];
            if (sscanf(cur, "%15[^>]>%15s", a, b) != 2) { snprintf(err, en, "Malformed dependency '%.40s'", cur); rc = -1; }
            else rc = world_add_dep(w, a, b, 0, err, en);      /* cycles are reported by sched_run */
        }
        cur = semi ? semi + 1 : cur + strlen(cur);
    }
    free(copy);
    return rc;
}

/* Common setup for /api/simulate and /api/compare. */
typedef struct { World *w; World local; int use_local; Process **arr; int n; Graph *g; int quantum, high; char *algo; } Job;

static int job_open(Job *j, const HttpRequest *rq, char *err, size_t en) {
    memset(j, 0, sizeof *j);
    char *q = form_get(rq->body, "quantum"), *prio = form_get(rq->body, "prio");
    char *procs = form_get(rq->body, "procs"), *deps = form_get(rq->body, "deps");
    j->algo = form_get(rq->body, "algo");
    j->quantum = 2; j->high = 0;
    int rc = 0;
    if (q) { char *end; long v = strtol(q, &end, 10); if (*q == '\0' || *end) { snprintf(err, en, "Time quantum must be a whole number"); rc = -1; } else j->quantum = (int)v; }
    if (prio) j->high = (strcmp(prio, "high") == 0);
    if (rc == 0 && procs) {
        j->use_local = 1;
        if (world_init(&j->local) != 0) { snprintf(err, en, "Out of memory"); rc = -1; }
        else {
            j->w = &j->local;
            rc = parse_procs(&j->local, procs, err, en);
            if (rc == 0 && deps) rc = parse_deps(&j->local, deps, err, en);
        }
    } else if (rc == 0) j->w = &G;
    if (rc == 0 && world_build(j->w, &j->arr, &j->n, &j->g) != 0) { snprintf(err, en, "Out of memory"); rc = -1; }
    free(q); free(prio); free(procs); free(deps);
    return rc;
}

static void job_close(Job *j) {
    free(j->arr); graph_free(j->g); free(j->algo);
    if (j->use_local) world_free(&j->local);
}

static void route_simulate(const HttpRequest *rq, StrBuf *out, int *status) {
    char err[256] = ""; Job j; Algo algo; SimResult r;
    if (job_open(&j, rq, err, sizeof err) != 0) { reply_error(out, status, 400, err); job_close(&j); return; }
    if (!j.algo || algo_from_string(j.algo, &algo) != 0) { reply_error(out, status, 400, "Unknown algorithm"); job_close(&j); return; }
    if (sched_run(j.arr, j.n, j.g, algo, j.quantum, j.high, &r) != 0) {
        reply_error(out, status, 400, r.error); sched_result_free(&r); job_close(&j); return;
    }
    sb_printf(out, "{\"ok\":true,\"algo\":\"%s\",\"algoName\":", algo_id(algo)); sb_json_string(out, algo_name(algo));
    sb_printf(out, ",\"quantum\":%d,\"priorityRule\":\"%s\",\"processes\":[", r.quantum, j.high ? "higher number = higher priority" : "lower number = higher priority");
    for (int i = 0; i < j.n; i++) {
        const Process *p = j.arr[i];
        sb_printf(out, "%s{\"pid\":", i ? "," : ""); sb_json_string(out, p->pid);
        sb_printf(out, ",\"arrival\":%d,\"burst\":%d,\"priority\":%d,\"start\":%d,\"completion\":%d,\"turnaround\":%d,\"waiting\":%d,\"response\":%d}",
                  p->arrival, p->burst, p->priority, p->start_time, p->completion, p->turnaround, p->waiting, p->response);
    }
    sb_append(out, "],\"gantt\":[");
    for (int i = 0; i < r.nseg; i++) {
        const Segment *s = &r.seg[i];
        sb_printf(out, "%s{\"pid\":", i ? "," : ""); sb_json_string(out, s->idle ? "IDLE" : s->pid);
        sb_printf(out, ",\"idle\":%s,\"start\":%d,\"end\":%d,\"rem\":%d}", s->idle ? "true" : "false", s->start, s->end, s->rem_after);
    }
    const Stats *st = &r.stats;
    sb_printf(out, "],\"stats\":{\"avgWait\":%.4f,\"avgTat\":%.4f,\"avgResp\":%.4f,\"cpuUtil\":%.4f,\"throughput\":%.6f,\"totalTime\":%d,\"busyTime\":%d,\"completed\":%d},\"topo\":[",
              st->avg_wait, st->avg_tat, st->avg_resp, st->cpu_util, st->throughput, st->total_time, st->busy_time, st->completed);
    for (int i = 0; i < r.ntopo; i++) { sb_printf(out, "%s", i ? "," : ""); sb_json_string(out, j.arr[r.topo[i]]->pid); }
    sb_append(out, "],\"trace\":[");
    for (int i = 0; i < r.ntrace; i++) {
        const TraceStep *t = &r.trace[i];
        sb_printf(out, "%s{\"time\":%d,\"pid\":", i ? "," : "", t->time); sb_json_string(out, t->pid);
        sb_append(out, ",\"ops\":"); sb_json_string(out, t->ops);
        sb_printf(out, ",\"ready\":%s,\"stack\":%s}", t->ready_json, t->stack_json);
    }
    sb_append(out, "]}");
    sched_result_free(&r); job_close(&j);
}

static void route_compare(const HttpRequest *rq, StrBuf *out, int *status) {
    char err[256] = ""; Job j; SimResult r;
    if (job_open(&j, rq, err, sizeof err) != 0) { reply_error(out, status, 400, err); job_close(&j); return; }
    StrBuf body; sb_init(&body);
    for (int a = 0; a < ALGO_COUNT; a++) {
        if (sched_run(j.arr, j.n, j.g, (Algo)a, j.quantum, j.high, &r) != 0) {
            reply_error(out, status, 400, r.error); sched_result_free(&r); sb_free(&body); job_close(&j); return;
        }
        const Stats *s = &r.stats;
        sb_printf(&body, "%s{\"algo\":\"%s\",\"name\":", a ? "," : "", algo_id((Algo)a)); sb_json_string(&body, algo_name((Algo)a));
        sb_printf(&body, ",\"avgWait\":%.4f,\"avgTat\":%.4f,\"avgResp\":%.4f,\"cpuUtil\":%.4f,\"throughput\":%.6f,\"totalTime\":%d,\"busyTime\":%d}",
                  s->avg_wait, s->avg_tat, s->avg_resp, s->cpu_util, s->throughput, s->total_time, s->busy_time);
        sched_result_free(&r);
    }
    sb_printf(out, "{\"ok\":true,\"quantum\":%d,\"count\":%d,\"results\":[%s]}", j.quantum, j.n, body.buf);
    sb_free(&body); job_close(&j);
}

/* ---------------- dispatcher ---------------- */
void api_handle(const HttpRequest *rq, StrBuf *out, int *status) {
    const char *p = rq->path;
    *status = 200;
    if (!strcmp(p, "/api/health")) { sb_append(out, "{\"ok\":true,\"service\":\"SchedulX C backend\"}"); return; }
    if (!strcmp(p, "/api/processes")) { route_processes(rq, NULL, out, status); return; }
    if (!strncmp(p, "/api/processes/", 15) && p[15]) { route_processes(rq, p + 15, out, status); return; }
    if (!strcmp(p, "/api/sample") && !strcmp(rq->method, "POST")) {
        char *d = form_get(rq->body, "deps");
        world_load_sample(&G, d && !strcmp(d, "1")); free(d);
        json_process_list(out, NULL); return;
    }
    if (!strcmp(p, "/api/reset") && !strcmp(rq->method, "POST")) { world_clear(&G); json_process_list(out, NULL); return; }
    if (!strcmp(p, "/api/deps")) { route_deps(rq, out, status); return; }
    if (!strcmp(p, "/api/graph") && !strcmp(rq->method, "GET")) { route_graph(out, status); return; }
    if (!strcmp(p, "/api/hash") && !strcmp(rq->method, "GET")) {
        sb_append(out, "{\"ok\":true,\"table\":"); ht_to_json(&G.ht, out); sb_append(out, "}"); return;
    }
    if (!strcmp(p, "/api/simulate") && !strcmp(rq->method, "POST")) { route_simulate(rq, out, status); return; }
    if (!strcmp(p, "/api/compare") && !strcmp(rq->method, "POST")) { route_compare(rq, out, status); return; }
    reply_error(out, status, 404, "Unknown API route");
}
