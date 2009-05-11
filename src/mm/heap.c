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
#include <mm/heap.h>
#include <mm/mem.h>
#include <lib/stdio.h>
#include <mm/paging.h>

heap_t *kheap;
extern page_directory_t *kernel_directory;
static void debug_heap_obj(heap_obj_t*);

void init_heap()
{
    /* Create the heap */
    kheap = (heap_t*) KHEAP_START;
    kheap->size = KHEAP_INITIAL_SIZE;
    kheap->allocated = 0;

    /* Create a huge hole, using the initial size */
    heap_obj_t *first = (heap_obj_t*) (KHEAP_START + sizeof(heap_t));
    first->addr = (u32)first + sizeof(heap_obj_t);
    first->allocated = 0;
    first->size = KHEAP_INITIAL_SIZE - sizeof(heap_t) - sizeof(heap_obj_t);
    first->next = 0;

    /* Attach the hole to the heap */
    kheap->first_obj = first;
}

/* TODO: Make sure we always have enough space for a new page table allocation */
void *alloc(u32 size, int aligned)
{
    ASSERT(size);

    heap_obj_t *current = (heap_obj_t*) kheap->first_obj,
               *last = 0,
               *underflow_obj = 0,
               *aligned_obj = 0;

    /* Try to find a hole that fits our size */
    while (current)
    {
        /* We're looking for holes */
        if (!current->allocated)
        {
            /* If we ask for page aligned allocation, make sure we won't pick nodes with nonaligned addresses */
            if (!aligned || ((u32)current->addr & 0xFFF) == 0)
            {
                /* Perfect fit */
                if (current->size == size) {
                    //printf("equalhole\n");
                    break;
                }
                else if (current->size > size + sizeof(heap_obj_t))
                {
                    //printf("makehole\n");

                    /* Putting this here since we can :) Used for creating a hole if we don't need all the space of current */
                    underflow_obj = (heap_obj_t*) (current->addr + size);
                    break;
                }
            }
            /* See if we can fit a page aligned addresse inside current's space */
            else if (aligned)
            {
                u32 aligned_addr = (u32)current->addr & ~0xFFFul;
                aligned_addr += 0x1000;

                /* Do we have room on the left side to make the aligned_obj (without leaking memory)? */
                if (aligned_addr - (u32)current->addr > sizeof(heap_obj_t))
                {
                    /* We want the *address* to be page aligned, not the object */
                    aligned_obj = (heap_obj_t*) (aligned_addr - sizeof(heap_obj_t));

                    u32 current_end = (u32)current->addr + current->size;
                    u32 aligned_end = (u32)aligned_addr + size;

                    /* Does it fit perfectly? */
                    if (aligned_end == current_end)
                    {
                        //printf("perfect fit\n");

                        aligned_obj->addr = aligned_addr;
                        aligned_obj->size = size;
                        aligned_obj->allocated = 1;

                        aligned_obj->next = current->next;
                        current->next = (u32)aligned_obj;
                        current->size = (u32)aligned_obj - current->addr;

                        kheap->allocated += size;
                        return (void*) aligned_obj->addr;
                    }
                    /* It doesn't fit perfectly; we need to make an underflow_obj */
                    else if (aligned_end < current_end + sizeof(heap_obj_t))
                    {
                        //printf("making underflow\n");

                        aligned_obj->addr = aligned_addr;
                        aligned_obj->size = size;
                        aligned_obj->allocated = 1;

                        underflow_obj = (heap_obj_t*) aligned_end;
                        underflow_obj->addr = (u32)underflow_obj + sizeof(heap_obj_t);
                        underflow_obj->size = current_end - (u32)underflow_obj->addr;
                        underflow_obj->allocated = 0;

                        current->size = (u32)aligned_obj - current->addr;

                        underflow_obj->next = current->next;
                        current->next = (u32)aligned_obj;
                        aligned_obj->next = (u32) underflow_obj;

                        kheap->allocated += size;
                        return (void*) aligned_obj->addr;
                    }
                }
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
            underflow_obj->addr = (u32)underflow_obj + sizeof(heap_obj_t);
            underflow_obj->size = current->size - size - sizeof(heap_obj_t);
            underflow_obj->allocated = 0;

            underflow_obj->next = current->next;
            current->next = (u32) underflow_obj;
        }
    }
    else if (last)
    {
        /* We didn't find anything and need to expand */
        //printf("last\n");

        /* The end of last should always be the end of the heap at this point */
        ASSERT((u32)last->addr + last->size == KHEAP_START + kheap->size);

        /* We should at least expand with enough room for a page table allocation to prevent endless recursive loops */
        u32 aligned_size = size & 0xFFFF0000;
        aligned_size += 0x00010000;

        u32 start = KHEAP_START + kheap->size;
        u32 end = KHEAP_START + kheap->size + aligned_size;

        //printf("allocating frames: %x <=> %x\n", start, end);
        /* For each new address, make sure the page is allocated */
        while (start < end)
        {
            alloc_frame(get_page(start, 1, kernel_directory), 0, 0);
            start += 0x1000;
        }

        /* Update the size of the heap and the last hole */
        /* We need to find the last again, since alloc_frame might have currupted the current "last" */
        last = (heap_obj_t*) kheap->first_obj;
        while (last->next)
        {
            last = (heap_obj_t*) last->next;
        }

        ASSERT(!last->allocated);

        last->size += aligned_size;
        kheap->size += aligned_size;

        /* We should have room in the heap now */
        return alloc(size, aligned);
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
        if (current->addr == (u32)addr)
            break;
        last = current;
        current = (heap_obj_t*) current->next;
    }

    /* .. panic of not :) */
    ASSERT(current);

    memset((void*) current->addr, 0, current->size);

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


static void debug_heap_obj(heap_obj_t* obj)
{
    printf("%x(%s): addr:%x, size:%d, next:%x\n", (u32)obj, obj->allocated ? "alloc" : "hole ", (u32)obj->addr, obj->size, (u32)obj->next);

    ASSERT((u32)obj->addr == (u32)obj + sizeof(heap_obj_t));
    ASSERT(!obj->next || ((u32)obj->addr + obj->size) == (u32)obj->next);
}

void debug_heap()
{
    printf("Allocated: %d\n", kheap->allocated);

    heap_obj_t *current = (heap_obj_t*) kheap->first_obj;
    while (current)
    {
        debug_heap_obj(current);
        current = (heap_obj_t*) current->next;
    }

    printf("\n");
}
