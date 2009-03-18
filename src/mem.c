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

void *mem_ptr;  /* The start of which malloc will return memory from */
extern int end; /* Defined in link.ld */

void init_memory()
{
    printf("Initializing memory..");

    /* Both variables explained above */
    mem_ptr = &end;

    printf("\tOK. Starting at %x.\n", mem_ptr);
}

void *malloc(size_t size)
{
    /* Very simplistic approach :) */
    void *ret = mem_ptr;
    mem_ptr += size;

    return ret;
}

void free(void* ptr)
{
    /* Does nothing right now */
    ptr = 0;
}
