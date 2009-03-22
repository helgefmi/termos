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

u32 mem_ptr;  /* The start of which malloc will return memory from */
extern int end; /* Defined in link.ld */

void init_memory()
{
    printf("Initializing memory..");

    /* Both explained above */
    mem_ptr = (u32)&end;

    printf("\tOK. Starting at %x.\n", mem_ptr);
}

u32 kmalloc_a(size_t size)
{
    mem_ptr &= 0xFFFFF000;
    mem_ptr += 0x00001000;

    return kmalloc(size);
}

u32 kmalloc_ap(size_t size, u32 *phys)
{
    mem_ptr &= 0xFFFFF000;
    mem_ptr += 0x00001000;

    *phys = mem_ptr;

    return kmalloc(size);
}

u32 kmalloc(size_t size)
{
    u32 ret = mem_ptr;

    mem_ptr += size;

    return ret;
}

void kfree(void* ptr)
{
    ptr = 0;
}
