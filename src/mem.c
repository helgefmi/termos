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

#include "mem.h"
#include "stdio.h"
#include "heap.h"

extern int end; /* Defined in link.ld */
u32 mem_ptr = (u32)&end;  /* The start of which malloc will return memory from */

static u32 kmalloc_int(size_t size, int aligned, u32 *phys)
{
    if (aligned)
    {
        mem_ptr &= 0xFFFFF000;
        mem_ptr += 0x00001000;
    }

    if (phys)
    {
        *phys = mem_ptr;
    }

    u32 tmp = mem_ptr;
    mem_ptr += size;

    return tmp;
}

u32 kmalloc_a(size_t size)
{
    return kmalloc_int(size, 1, 0);
}

u32 kmalloc_ap(size_t size, u32 *phys)
{

    return kmalloc_int(size, 1, phys);
}

u32 kmalloc(size_t size)
{
    return kmalloc_int(size, 0, 0);
}

void kfree(void* ptr)
{
    ptr = 0;
}
