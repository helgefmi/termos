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
#include <kernel/common.h>

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
    while (num--)
        *src_u16++ = c;

    return src;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    u8 *dst_u8 = dst;
    const u8 *src_u8 = src;

    while (n--)
        *dst_u8++ = *src_u8++;

    return dst;
}

void *wmemcpy(void *dst, const void *src, size_t n)
{
    u16 *dst_u16 = dst;
    const u16 *src_u16 = src;

    while (n--)
        *dst_u16++ = *src_u16++;

    return dst;
}
