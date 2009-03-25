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

#include "paging.h"
#include "stdio.h"
#include "mem.h"
#include "string.h"
#include "isr.h"
#include "heap.h"

/* divide on 32 */
#define BIT_INDEX(n) ((n) >> 5)
/* modulus 32 */
#define BIT_OFFSET(n) ((n)&0x1F)

u32 *frames;
u32 nframes;

extern u32 mem_ptr;

page_directory_t *kernel_directory,
                 *current_directory;

/*
static u32 test_frame(u32 addr)
{
    addr >>= 12; // divide on 0x1000
    u32 idx = BIT_INDEX(addr);
    u32 off = BIT_OFFSET(addr);
    return frames[idx] & (0x1 << off);
}
*/

static void set_frame(u32 addr)
{
    addr >>= 12; /* divide on 0x1000 */
    u32 idx = BIT_INDEX(addr);
    u32 off = BIT_OFFSET(addr);
    frames[idx] |= (0x1 << off);
}

static void clear_frame(u32 addr)
{
    addr >>= 12; /* divide on 0x1000 */
    u32 idx = BIT_INDEX(addr);
    u32 off = BIT_OFFSET(addr);
    frames[idx] &= ~(0x1 << off);
}

/* Finds the first unused frame (physical address). Used by alloc_frame */
static u32 first_frame()
{
    u32 idx, off;

    for (idx = 0; idx < BIT_INDEX(nframes); ++idx)
    {
        /* No point in checking an already full u32 */
        if (frames[idx] != 0xFFFFFFFF)
        {
            for (off = 0; off < 32; ++off)
            {
                if (!(frames[idx] & (0x1 << off)))
                {
                    return idx * 32 + off;
                }
            }
        }
    }

    return (u32)-1;
}

void alloc_frame(page_t *page, int is_kernel, int is_writable)
{
    /* Return if the page is already connected to a frame */
    if (page->frame)
    {
        return;
    }

    /* Finds the first available frame by checking the bitmap */
    u32 frame = first_frame();

    if (frame == (u32)-1)
    {
        PANIC("No free frames!");
    }

    page->present = 1;
    page->rw = (is_writable ? 1 : 0);
    page->user = (is_kernel ? 0 : 1);
    page->frame = frame;

    /* Update the bitmap */
    set_frame(page->frame * 0x1000);
}

void free_frame(page_t *page)
{
    /* Return if the page isn't connected to a frame */
    if (!page->frame)
    {
        return;
    }

    page->present = 0;

    /* Update the bitmap */
    clear_frame(page->frame * 0x1000);
}

void init_paging()
{
    /* Memory size */
    u32 end = 0x1000000;

    /* How many frames are needed to map the whole memory size */
    nframes = (end >> 12); /* divided by 0x1000 */

    /* Our bitmap of which frames are free or not */
    frames = (u32*)kmalloc_a(BIT_INDEX(nframes));
    memset(frames, 0, BIT_INDEX(nframes));

    kernel_directory = (page_directory_t*) kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));

    current_directory = kernel_directory;

    u32 idx;

    /* Make the pages for our kernel heap now with the simple version of kmalloc
     * before paging is initialized. Allocation will happen after the identity map */
    for (idx = KHEAP_START; idx < KHEAP_START + KHEAP_INITIAL_SIZE; idx += 0x1000)
    {
        get_page(idx, 1, kernel_directory);
    }

    /* Identity map the (so far) used memory,
     * so addresses used will still be valid after enabling paging.
     * This works since we start on 0 both in get_page and find_first() */
    for (idx = 0; idx < mem_ptr; idx += 0x1000)
    {
        alloc_frame( get_page(idx, 1, kernel_directory), 0, 0);
    }

    register_isr_handler(14, page_fault);

    switch_page_directory(kernel_directory);

    /* Allocate the kernel heap pages */
    for (idx = KHEAP_START; idx < KHEAP_START + KHEAP_INITIAL_SIZE; idx += 0x1000)
    {
        alloc_frame(get_page(idx, 0, kernel_directory), 0, 0);
    }
}

void switch_page_directory(page_directory_t *dir)
{
    current_directory = dir;
    asm volatile("mov %0, %%cr3":: "r"(dir->tablesPhysical));

    u32 cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));

    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

page_t *get_page(u32 addr, int make, page_directory_t *dir)
{
    u32 pagenum = addr >> 12;  /* divided by 0x1000 */
    u32 idx = pagenum >> 10;   /* divided by 1024 */
    u32 off = pagenum & 0x3ff; /* modulus 1024 */

    if (dir->tables[idx])
    {
        return &dir->tables[idx]->pages[off];
    }
    else if (make)
    {
        u32 phys;

        dir->tables[idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &phys);
        memset(dir->tables[idx], 0, sizeof(page_table_t));

        dir->tablesPhysical[idx] = phys | 0x7;

        return &dir->tables[idx]->pages[off];
    }
    
    return 0;
}

void page_fault(registers_t* regs)
{
    /* The address that caused a page fault is saved in cr2 at this point */
    u32 addr;
    asm volatile ("mov %%cr2, %0" : "=r" (addr));

    printf("\nPage fault at %x (", addr);
    if (regs->err_code & 0x1)
        printf("present");
    else
        printf("notpresent");

    if (regs->err_code & 0x2)
        printf(", write");
    else
        printf(", read");
    
    if (regs->err_code & 0x4)
        printf(", usermode");
    else
        printf(", kernelmode");

    if (regs->err_code & 0x8)
        printf(", overwritten");

    if (regs->err_code & 0x10)
        printf(", fetch");

    printf(")\n");

    PANIC("Page Fault");
}
