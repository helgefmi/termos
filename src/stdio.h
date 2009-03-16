#ifndef _STDIO_H
#define _STDIO_H

#include "common.h"

extern int printf(const char*, ...);
extern int fprintf(FILE *, const char*, ...);
extern int sprintf(char *, const char*, ...);
extern int snprintf(char *, size_t, const char*, ...);

#endif
