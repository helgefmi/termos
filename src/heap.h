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

#ifndef _HEAP_H
#define _HEAP_H

#include "common.h"

#define KHEAP_START (0xC0000000)
#define KHEAP_INITIAL_SIZE (0x100000)

typedef struct
{
    u32 addr;
    u32 size;
    u8 allocated;
    u32 next;
} heap_obj_t;

typedef struct
{
    u32 size;
    u32 allocated;
    heap_obj_t *first_obj;
} heap_t;

void init_heap();
void *alloc(u32, int);
void free(void*);
void debug_heap();

#endif
