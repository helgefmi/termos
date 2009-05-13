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

#include <lib/string.h>
#include <mm/mem.h>

const char *strtokStr = 0;
char *strtokRet = 0;

char *strtok(const char *str, const char *delim)
{
    const char *delim2 = delim;

    if (str == NULL)
        str = strtokStr;
    else
        strtokStr = str;

    if (strtokRet != NULL)
        kfree(strtokRet);

    int matched = 0;
    while (*str)
    {
        while (*delim2 && *delim2 != *str)
            ++delim2;

        if (!matched && *delim2)
            ++str;
        else if (matched && !*delim2)
            ++str;
        else if (!matched)
            matched = str - strtokStr;
        else
            break;

        delim2 = delim;
    }

    if (!matched)
        return NULL;

    int len = (str - strtokStr - matched);
    if (!len)
        return NULL;

    strtokRet = (char*) kmalloc(len + 1);
    strncpy(strtokRet, strtokStr + matched, len + 1);
    strtokRet[len] = '\0';

    strtokStr = str;
    return strtokRet;
}

char *index(const char *src, char c)
{
    while (*src && *src != c)
        ++src;

    return *src ? (char*) src : NULL;
}

char *rindex(const char *src, char c)
{
    char *last = NULL;
    while (*src)
    {
        if (*src == c)
            last = (char*) src;
        ++src;
    }

    return last;
}

char *strncpy(char *dst, const char *src, u32 size)
{
    char *tmp = dst;

    while (*src && size)
    {
        *dst++ = *src++;
        --size;
    }
    while (size)
    {
        *dst++ = '\0';
        --size;
    }

    return tmp;
}

char *strcpy(char *dst, const char *src)
{
    char *tmp = dst;

    while (*src)
    {
        *dst++ = *src++;
    }

    return tmp;
}

int strcmp(const char *s1, const char *s2)
{
    for (;*s1 || *s2; ++s1, ++s2)
    {
        if (*s1 < *s2)
            return -1;
        else if (*s1 > *s2)
            return 1;
    }

    return 0;
}

void *memset(void *src, int c, size_t num)
{
    u8 *src_u8 = src;
    while (num--)
        *src_u8++ = c;

    return src;
}

void *wmemset(void *src, int c, size_t num)
{
    u16 *src_u16 = src;
    while (num)
    {
        *src_u16++ = c;
        --num;
    }

    return src;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    u8 *dst_u8 = dst;
    const u8 *src_u8 = src;

    while (n)
    {
        *dst_u8++ = *src_u8++;
        --n;
    }

    return dst;
}

void *wmemcpy(void *dst, const void *src, size_t n)
{
    u16 *dst_u16 = dst;
    const u16 *src_u16 = src;

    while (n)
    {
        *dst_u16++ = *src_u16++;
        --n;
    }

    return dst;
}

int strlen(const char *src)
{
    const char *start = src;
    while (*src)
        ++src;

    return src - start;
}
