#include <stdio.h>
#include <stdlib.h>
#include "api.h"
#include "http_server.h"

/* Usage: schedulx [port] [frontend_dir] */
int main(int argc, char **argv) {
    int port = 8080;
    const char *root = "frontend";
    if (argc > 1) { port = atoi(argv[1]); if (port < 1 || port > 65535) { fprintf(stderr, "Invalid port\n"); return 1; } }
    if (argc > 2) root = argv[2];
    if (api_init() != 0) { fprintf(stderr, "Initialisation failed\n"); return 1; }
    int rc = server_run(port, root);
    api_shutdown();
    printf("Server stopped, memory released.\n");
    return rc;
}
