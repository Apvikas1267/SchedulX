#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

typedef struct {
    char method[8];
    char path[256];
    char query[2048];
    char *body;
    int body_len;
} HttpRequest;

/* Blocks until Ctrl+C. Serves static files from root_dir and the JSON API under /api/. */
int server_run(int port, const char *root_dir);

#endif
