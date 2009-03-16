#ifndef _STDARG_H
#define _STDARG_H

#include "common.h"

typedef __builtin_va_list va_list;
#define va_start(v,l) __builtin_va_start(v,l)
#define va_end(v)   __builtin_va_end(v)
#define va_arg(v,l) __builtin_va_arg(v,l)

/*
int vprintf(const char*, va_list);
int vfprintf(FILE*, const char*, va_list);
*/
int vsprintf(char*, const char*, va_list);
int vsnprintf(char*, size_t, const char*, va_list);

#endif
