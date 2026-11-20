#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strbuf.h"

static int sb_reserve(StrBuf *sb, size_t extra) {
    if (sb->len + extra + 1 <= sb->cap) return 1;
    size_t nc = sb->cap ? sb->cap : 256;
    while (nc < sb->len + extra + 1) nc *= 2;
    char *nb = (char *)realloc(sb->buf, nc);
    if (!nb) return 0;
    sb->buf = nb; sb->cap = nc;
    return 1;
}

void sb_init(StrBuf *sb) {
    sb->buf = NULL; sb->len = 0; sb->cap = 0;
    if (sb_reserve(sb, 0)) sb->buf[0] = '\0';
}

void sb_free(StrBuf *sb) { free(sb->buf); sb->buf = NULL; sb->len = sb->cap = 0; }

char *sb_take(StrBuf *sb) { char *p = sb->buf; sb->buf = NULL; sb->len = sb->cap = 0; return p; }

void sb_append(StrBuf *sb, const char *s) {
    size_t n = strlen(s);
    if (!sb_reserve(sb, n)) return;
    memcpy(sb->buf + sb->len, s, n + 1);
    sb->len += n;
}

void sb_printf(StrBuf *sb, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int need = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    if (need < 0 || !sb_reserve(sb, (size_t)need)) return;
    va_start(ap, fmt);
    vsnprintf(sb->buf + sb->len, (size_t)need + 1, fmt, ap);
    va_end(ap);
    sb->len += (size_t)need;
}

void sb_json_string(StrBuf *sb, const char *s) {
    sb_append(sb, "\"");
    for (; *s; s++) {
        unsigned char c = (unsigned char)*s;
        if (c == '"' || c == '\\') sb_printf(sb, "\\%c", c);
        else if (c < 0x20) sb_printf(sb, "\\u%04x", c);
        else sb_printf(sb, "%c", c);
    }
    sb_append(sb, "\"");
}
