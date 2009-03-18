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

#ifndef _GDT_H
#define _GDT_H

#include "common.h"

typedef struct
{
    u16 limit_low;   /* The lower 16 bits of the limit */
    u16 base_low;    /* The lower 16 bits of the base */
    u8  base_middle; /* The next 8 bits of the base */
    u8  access;      /* Access flags, determine what ring this segment can be used in */
    u8  granularity;
    u8  base_high;   /* The last 8 bits of the base */
} __attribute__((packed)) gdt_entry_t; 

typedef struct
{
   u16 limit; /* Size of the GDT entries */
   u32 base;  /* Location of the first GDT entry */
} __attribute__((packed)) gdt_ptr_t; 

void init_gdt();

void gdt_set_gate(s32, u32, u32, u8, u8);

#endif
