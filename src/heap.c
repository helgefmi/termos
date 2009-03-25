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

#include "heap.h"
#include "mem.h"
#include "stdio.h"

heap_t *kheap;

void init_heap()
{
    kheap = (heap_t*) KHEAP_START;
    kheap->start = KHEAP_START + sizeof(heap_t);
    kheap->size = KHEAP_INITIAL_SIZE;
    kheap->first_obj = 0;
    kheap->allocated = 0;
    kheap->in_use = 0;
}

void expand(u32 new_size)
{
    new_size &= 0xFFFFF000;
    new_size += 0x1000;

    printf("old: %d, new: %d\n", kheap->size, new_size);

    kheap->size = new_size;
    PANIC("expand not made yet :-)");
}

void *alloc(u32 size)
{
    if( !size )
    {
        PANIC("alloc called without size");
    }

    heap_obj_t *current = (heap_obj_t*) kheap->first_obj,
               *last = 0,
               *underflow_obj = 0;

    /* Try to find a hole that fits our size */
    while (current)
    {
        if (!current->allocated)
        {
            if (current->size == size) {
                //printf("equalhole\n");
                break;
            }
            else if (current->size > size + sizeof(heap_obj_t)) {
                //printf("makehole\n");
                underflow_obj = (heap_obj_t*) (current->addr + size);
                break;
            }
        }

        last = current;
        current = (heap_obj_t*) current->next;
    }

    if (current)
    {
        /* If we found a hole and it doesn't fit perfectly, we can make a new hole after current */
        if (underflow_obj)
        {
            //printf("underflow\n");
            underflow_obj->addr = (void*) ((u32)underflow_obj + sizeof(heap_obj_t));
            underflow_obj->size = current->size - size - sizeof(heap_obj_t);
            underflow_obj->allocated = 0;

            underflow_obj->next = current->next;
            current->next = (struct heap_obj*)underflow_obj;
        }
    }
    else if (last)
    {
        //printf("last\n");

        /* No holes fits our size; check if we need to expand before making a new heap object */
        if (((u32)last->addr + last->size + sizeof(heap_obj_t) + size) > (kheap->start + kheap->size))
        {
            expand(kheap->size + sizeof(heap_obj_t) + size);
            return alloc(size);
        }

        /* Make an allocated object right after the last one */
        current = (heap_obj_t*) (last->addr + last->size);
        current->addr = (void*) ((u32)current + sizeof(heap_obj_t));
        current->next = 0;

        last->next = (struct heap_obj*) current;
    }
    else
    {
        //printf("first\n");
        /* Expand if we don't have enough room */
        if (size + sizeof(heap_obj_t) > kheap->size)
        {
            expand(kheap->size + sizeof(heap_obj_t) + size);
            return alloc(size);
        }

        /* The first allocated heap object! */
        current = (heap_obj_t*) (kheap->start);
        current->addr = (void*) ((u32) current + sizeof(heap_obj_t));
        current->next = 0;

        kheap->first_obj = current;
    }

    /* Common for all cases */
    current->allocated = 1;
    current->size = size;
    kheap->allocated += size;
    return (void*) current->addr;
}

void free(void *addr)
{
    //printf("free %x\n", (u32)addr);

    heap_obj_t *current = (heap_obj_t*) kheap->first_obj,
               *last = 0,
               *next = 0;

    /* Find the heap object that has this address */
    while (current)
    {
        if (current->addr == addr)
            break;
        last = current;
        current = (heap_obj_t*) current->next;
    }

    /* .. panic of not :) */
    if (!current)
    {
        PANIC("Couldn't find memory to free");
    }

    current->allocated = 0;
    kheap->allocated -= current->size;

    /* If next is a hole too, just append it to current */
    next = (heap_obj_t*) current->next;
    if (next && !next->allocated)
    {
        //printf("append-next\n");
        current->size += next->size + sizeof(heap_obj_t);
        current->next = next->next;
    }

    /* If last is a hole too, just append current to it */
    if (last && !last->allocated)
    {
        //printf("prepend-to-last\n");
        last->size += current->size + sizeof(heap_obj_t);
        last->next = current->next;
    }
}


/* debug_heap not only tells us the state of our heap,
 * it also checks for errors/bugs in the alloc-implementation */
void debug_heap()
{
    printf("Allocated: %d, In use: %d\n", kheap->allocated, kheap->in_use);

    heap_obj_t *current = (heap_obj_t*) kheap->first_obj;
    while (current)
    {
        printf("%x: size(%d) %s\n", (u32)current->addr, current->size, current->allocated ? "alloacted" : "hole");

        if ((u32)current->addr != (u32)current + sizeof(heap_obj_t))
        {
            printf("\n\n%x != %x+%x\n", (u32)current->addr, (u32)current, sizeof(heap_obj_t));
            PANIC("current->addr != (u32)current + sizeof(heap_obj_t)");
        }

        if (current->next && ((u32)current->addr + current->size) != (u32)current->next)
        {
            PANIC("current->next && ((u32)current->addr + current->size) != (u32)current->next");
        }

        current = (heap_obj_t*) current->next;
    }

    printf("\n");
}
