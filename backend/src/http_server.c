#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http_server.h"
#include "api.h"
#include "strbuf.h"

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  typedef SOCKET sock_t;
  #define CLOSESOCK closesocket
  #define BAD_SOCK INVALID_SOCKET
  static CRITICAL_SECTION g_lock;
  #define LOCK()   EnterCriticalSection(&g_lock)
  #define UNLOCK() LeaveCriticalSection(&g_lock)
#else
  #include <sys/socket.h>
  #include <sys/select.h>
  #include <sys/time.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <pthread.h>
  #include <unistd.h>
  typedef int sock_t;
  #define CLOSESOCK close
  #define BAD_SOCK (-1)
  static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
  #define LOCK()   pthread_mutex_lock(&g_lock)
  #define UNLOCK() pthread_mutex_unlock(&g_lock)
#endif

#define REQ_MAX (256 * 1024)

static volatile int g_stop = 0;
static char g_root[512] = "frontend";

static void on_sigint(int sig) { (void)sig; g_stop = 1; }

static void send_all(sock_t s, const char *buf, size_t n) {
    while (n > 0) {
        int k = send(s, buf, (int)n, 0);
        if (k <= 0) return;
        buf += k; n -= (size_t)k;
    }
}

static const char *status_text(int c) {
    switch (c) { case 200: return "OK"; case 201: return "Created"; case 204: return "No Content"; case 400: return "Bad Request";
                 case 403: return "Forbidden"; case 404: return "Not Found"; case 405: return "Method Not Allowed";
                 case 413: return "Payload Too Large"; default: return "Internal Server Error"; }
}

static void respond(sock_t c, int status, const char *ctype, const char *body, size_t len) {
    char head[512];
    int h = snprintf(head, sizeof head,
        "HTTP/1.1 %d %s\r\nContent-Type: %s\r\nContent-Length: %zu\r\nAccess-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\nAccess-Control-Allow-Headers: Content-Type\r\n"
        "Cache-Control: no-cache\r\nConnection: close\r\n\r\n", status, status_text(status), ctype, len);
    send_all(c, head, (size_t)h);
    if (len) send_all(c, body, len);
}

static const char *mime_of(const char *path) {
    const char *dot = strrchr(path, '.');
    if (!dot) return "application/octet-stream";
    if (!strcmp(dot, ".html")) return "text/html; charset=utf-8";
    if (!strcmp(dot, ".css"))  return "text/css; charset=utf-8";
    if (!strcmp(dot, ".js"))   return "application/javascript; charset=utf-8";
    if (!strcmp(dot, ".json")) return "application/json";
    if (!strcmp(dot, ".svg"))  return "image/svg+xml";
    if (!strcmp(dot, ".png"))  return "image/png";
    if (!strcmp(dot, ".ico"))  return "image/x-icon";
    return "application/octet-stream";
}

static void serve_static(sock_t c, const char *urlpath) {
    char full[1024];
    if (strstr(urlpath, "..") || strchr(urlpath, '\\')) { respond(c, 403, "text/plain", "Forbidden", 9); return; }
    snprintf(full, sizeof full, "%s%s%s", g_root, urlpath, strcmp(urlpath, "/") == 0 ? "index.html" : "");
    FILE *f = fopen(full, "rb");
    if (!f) { respond(c, 404, "text/plain", "Not found", 9); return; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    char *buf = (char *)malloc((size_t)sz + 1);
    if (!buf) { fclose(f); respond(c, 500, "text/plain", "Out of memory", 13); return; }
    size_t got = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    respond(c, 200, mime_of(full), buf, got);
    free(buf);
}

static int header_is(const char *p, const char *lower_name) {
    for (; *lower_name; p++, lower_name++)
        if (tolower((unsigned char)*p) != *lower_name) return 0;
    return 1;
}

static void handle_client(sock_t c) {
    char *buf = (char *)malloc(REQ_MAX + 1);
    int total = 0, hdr_end = -1, content_len = 0;
    if (!buf) return;
    while (total < REQ_MAX) {
        int k = recv(c, buf + total, REQ_MAX - total, 0);
        if (k <= 0) break;
        total += k; buf[total] = '\0';
        if (hdr_end < 0) {
            char *e = strstr(buf, "\r\n\r\n");
            if (e) {
                hdr_end = (int)(e - buf) + 4;
                for (char *p = buf; p < e; p++) {          /* case-insensitive Content-Length lookup */
                    if ((p == buf || p[-1] == '\n') && header_is(p, "content-length:")) { content_len = atoi(p + 15); break; }
                }
            }
        }
        if (hdr_end >= 0 && total >= hdr_end + content_len) break;
    }
    if (hdr_end < 0) { free(buf); return; }
    if (content_len > REQ_MAX - hdr_end) { respond(c, 413, "text/plain", "Too large", 9); free(buf); return; }

    HttpRequest rq; memset(&rq, 0, sizeof rq);
    char target[2400] = "";
    if (sscanf(buf, "%7s %2399s", rq.method, target) != 2) { respond(c, 400, "text/plain", "Bad request", 11); free(buf); return; }
    char *qm = strchr(target, '?');
    if (qm) { *qm = '\0'; snprintf(rq.query, sizeof rq.query, "%s", qm + 1); }
    snprintf(rq.path, sizeof rq.path, "%.255s", target);
    rq.body = buf + hdr_end; rq.body_len = total - hdr_end;
    buf[total] = '\0';

    if (!strcmp(rq.method, "OPTIONS")) respond(c, 204, "text/plain", "", 0);
    else if (!strncmp(rq.path, "/api/", 5)) {
        StrBuf out; int status = 200;
        sb_init(&out);
        LOCK(); api_handle(&rq, &out, &status); UNLOCK();
        respond(c, status, "application/json; charset=utf-8", out.buf, out.len);
        sb_free(&out);
    } else if (!strcmp(rq.method, "GET")) serve_static(c, rq.path);
    else respond(c, 405, "text/plain", "Method not allowed", 18);
    free(buf);
}

#ifdef _WIN32
static DWORD WINAPI worker(LPVOID arg) { sock_t c = (sock_t)(size_t)arg; handle_client(c); CLOSESOCK(c); return 0; }
static void spawn(sock_t c) {
    HANDLE h = CreateThread(NULL, 0, worker, (LPVOID)(size_t)c, 0, NULL);
    if (h) CloseHandle(h); else { handle_client(c); CLOSESOCK(c); }
}
#else
static void *worker(void *arg) { sock_t c = (sock_t)(size_t)arg; handle_client(c); CLOSESOCK(c); return NULL; }
static void spawn(sock_t c) {
    pthread_t t;
    if (pthread_create(&t, NULL, worker, (void *)(size_t)c) == 0) pthread_detach(t);
    else { handle_client(c); CLOSESOCK(c); }
}
#endif

int server_run(int port, const char *root_dir) {
    snprintf(g_root, sizeof g_root, "%s", root_dir);
    size_t rl = strlen(g_root);
    if (rl > 0 && (g_root[rl - 1] == '/' || g_root[rl - 1] == '\\')) g_root[rl - 1] = '\0';
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { fprintf(stderr, "WSAStartup failed\n"); return 1; }
    InitializeCriticalSection(&g_lock);
#endif
    sock_t ls = socket(AF_INET, SOCK_STREAM, 0);
    if (ls == BAD_SOCK) { fprintf(stderr, "Cannot create socket\n"); return 1; }
#ifndef _WIN32
    int yes = 1;
    setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof yes);
#endif
    struct sockaddr_in addr; memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET; addr.sin_port = htons((unsigned short)port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);              /* local only */
    if (bind(ls, (struct sockaddr *)&addr, sizeof addr) != 0 || listen(ls, 16) != 0) {
        fprintf(stderr, "Cannot listen on port %d (is another program using it?)\n", port);
        CLOSESOCK(ls); return 1;
    }
    signal(SIGINT, on_sigint);
    printf("SchedulX backend running.\nOpen http://localhost:%d in your browser (Ctrl+C to stop)\n", port);
    fflush(stdout);
    while (!g_stop) {
        fd_set rs; struct timeval tv = { 0, 300000 };
        FD_ZERO(&rs); FD_SET(ls, &rs);
        if (select((int)ls + 1, &rs, NULL, NULL, &tv) <= 0) continue;
        sock_t c = accept(ls, NULL, NULL);
        if (c == BAD_SOCK) continue;
#ifdef _WIN32
        DWORD to = 5000; setsockopt(c, SOL_SOCKET, SO_RCVTIMEO, (const char *)&to, sizeof to);
#else
        struct timeval to = { 5, 0 }; setsockopt(c, SOL_SOCKET, SO_RCVTIMEO, (const char *)&to, sizeof to);
#endif
        spawn(c);
    }
    CLOSESOCK(ls);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
