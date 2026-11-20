#ifndef API_H
#define API_H
#include "http_server.h"
#include "strbuf.h"

int  api_init(void);
void api_shutdown(void);
/* Fills *out with a JSON body and *status with the HTTP status code. */
void api_handle(const HttpRequest *rq, StrBuf *out, int *status);

#endif
