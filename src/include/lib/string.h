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

#ifndef _STRING_H
#define _STRING_H

#include <kernel/common.h>

void *memset(void*, int, size_t);
void *wmemset(void*, int, size_t);
void *memcpy(void*, const void*, size_t);
void *wmemcpy(void*, const void*, size_t);
char *strcpy(char *, const char *);
char *strncpy(char *, const char *, u32);
int strcmp(const char*, const char*);

#endif
