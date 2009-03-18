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

#ifndef _COMMON_H
#define _COMMON_H

#define INT_MAX (4294967295ul)

typedef unsigned long long int  u64;
typedef signed long long int    s64;
typedef unsigned long int       u32;
typedef signed long int         s32;
typedef unsigned short          u16;
typedef signed short            s16;
typedef unsigned char           u8;
typedef signed char             s8;

typedef u32 size_t;
typedef u32 FILE;

void outb(u16, u8);
u8 inb(u16);
u16 inw(u16);

#endif
