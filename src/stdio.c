#include "stdio.h"
#include "stdarg.h"
#include "mem.h"
#include "tty.h"

int printf(const char *fmt, ...)
{
    va_list args;
    int i;

    char* buf = malloc(1024);
    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    va_end(args);

    buf[i] = 0;

    puts(buf);
    return i;
}

int sprintf(char *buf, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    va_end(args);

    buf[i] = 0;

    return i;
}

/*
int fprintf(FILE *stream, const char *fmt, ...)
{
}

int snprintf(char *str, size_t size, const char *fmt, ...)
{
}
*/
