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

#include "common.h"
#include "paging.h"
#include "stdio.h"
#include "mem.h"
#include "string.h"
#include "isr.h"
#include "heap.h"

#define BIT_IDX(n) ((n) >> 5)
#define BIT_OFF(n) ((n)&0x1F)

#define FRAME_SET(n) (frames[BIT_IDX((n))] |= (0x1 << BIT_OFF((n))))
#define FRAME_CLR(n) (frames[BIT_IDX((n))] &= ~(0x1ul << BIT_OFF((n))))
#define FRAME_TST(n) (famres[BIT_IDX((n))] & (0x1 << BIT_OFF((n))))

u32 *frames;
u32 nframes;

extern u32 mem_ptr;

page_directory_t *kernel_directory,
                 *current_directory;

static u32 first_frame()
{
    u32 idx, off;

    for (idx = 0; idx < BIT_IDX(nframes); ++idx)
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

    PANIC("No free frames!");
    return (u32)-1;
}

void alloc_frame(page_t *page, int is_kernel, int is_writable)
{
    /* Return if the page is already connected to a frame */
    if (page->frame)
    {
        //PANIC("page already has a frame in alloc_frame()");
        return;
    }

    /* Finds the first available frame by checking the bitmap */
    u32 frame;
    frame = first_frame();

    if (frame == (u32)-1)
    {
        PANIC("Hm. No free frames!");
    }

    page->present = 1;
    page->rw = (is_writable ? 1 : 0);
    page->user = (is_kernel ? 0 : 1);
    page->frame = frame;

    /* Update the bitmap */
    FRAME_SET(page->frame);
}

void free_frame(page_t *page)
{
    /* Return if the page isn't connected to a frame */
    if (!page->frame)
    {
        PANIC("page doesn't have a frame in free_frame()");
        return;
    }

    page->present = 0;

    /* Update the bitmap */
    FRAME_CLR(page->frame);
}

void init_paging()
{
    /* Memory size */
    u32 end = 0x20000000;

    /* How many frames are needed to map the whole memory size */
    nframes = (end >> 12); /* divided by 0x1000 */

    /* Our bitmap of which frames are free or not */
    frames = (u32*)kmalloc_a(BIT_IDX(nframes));
    memset(frames, 0, BIT_IDX(nframes));

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

    /* Enable paging */
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

        //printf("Creating a new page table\n");

        dir->tables[idx] = (page_table_t*) kmalloc_ap(sizeof(page_table_t), &phys);
        memset(dir->tables[idx], 0, sizeof(page_table_t));

        //printf("asked for: %x, phys: %x addr: %x\n", addr, phys, dir->tables[idx]);

        dir->tablesPhysical[idx] = phys | 0x7;

        return &dir->tables[idx]->pages[off];
    }

    PANIC("No page to get in get_page()!");
    
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
