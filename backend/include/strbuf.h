#ifndef STRBUF_H
#define STRBUF_H
#include <stddef.h>

typedef struct { char *buf; size_t len, cap; } StrBuf;

void  sb_init(StrBuf *sb);
void  sb_free(StrBuf *sb);
char *sb_take(StrBuf *sb);                 /* hands the buffer to the caller */
void  sb_append(StrBuf *sb, const char *s);
void  sb_printf(StrBuf *sb, const char *fmt, ...);
void  sb_json_string(StrBuf *sb, const char *s);

#endif
