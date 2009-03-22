/* 
 * This file is part of TermOS.
 *
 * TermOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TermOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TermOS.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdio.h"
#include "stdarg.h"
#include "mem.h"
#include "tty.h"

int printf(const char *fmt, ...)
{
    va_list args;
    int i;

    char buf[1024];
    va_start(args, fmt);
    i = vsprintf((char*)&buf, fmt, args);
    va_end(args);

    buf[i] = 0;

    puts((char*)&buf);
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
