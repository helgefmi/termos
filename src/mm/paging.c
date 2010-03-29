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

#include <mm/paging.h>
#include <mm/mem.h>
#include <mm/heap.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <kernel/isr.h>
#include <kernel/common.h>

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
    u32 end = multiboot_header->mem_upper * 1000;

    /* How many frames are needed to map the whole memory size */
    nframes = (end / 0x1000);

    /* Our bitmap of which frames are free or not */
    frames = (u32*)kmalloc_a(BIT_IDX(nframes));
    memset(frames, 0, BIT_IDX(nframes));

    kernel_directory = (page_directory_t*) kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));
    kernel_directory->physicalAddr = (u32) kernel_directory->tablesPhysical;

    u32 idx;

    /* Make the pages for our kernel heap now with the simple version of kmalloc
     * before paging is initialized. Allocation will happen after the identity map */
    for (idx = KHEAP_START; idx < KHEAP_START + KHEAP_INITIAL_SIZE; idx += 0x1000)
        get_page(idx, 1, kernel_directory);

    /* Identity map the (so far) used memory,
     * so addresses used will still be valid after enabling paging.
     * This works since we start on 0 both in get_page and find_first() */
    for (idx = 0; idx < mem_ptr; idx += 0x1000)
        alloc_frame( get_page(idx, 1, kernel_directory), 0, 0);

    /* Allocate the kernel heap pages */
    for (idx = KHEAP_START; idx < KHEAP_START + KHEAP_INITIAL_SIZE; idx += 0x1000)
        alloc_frame(get_page(idx, 0, kernel_directory), 0, 0);

    register_isr_handler(14, page_fault);

    /* Enable paging */
    switch_page_directory(kernel_directory);
}

void switch_page_directory(page_directory_t *dir)
{
    current_directory = dir;
    asm volatile("mov %0, %%cr3":: "r"(dir->physicalAddr));

    u32 cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));

    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

page_t *get_page(u32 addr, int make, page_directory_t *dir)
{
    u32 pagenum = addr / 4096;
    u32 idx = pagenum / 1024;
    u32 off = pagenum % 1024;

    if (dir->tables[idx])
    {
        return &dir->tables[idx]->pages[off];
    }
    else if (make)
    {
        u32 phys;

        dir->tables[idx] = (page_table_t*) kmalloc_ap(sizeof(page_table_t), &phys);
        memset(dir->tables[idx], 0, sizeof(page_table_t));

        dir->tablesPhysical[idx] = phys | 0x7;

        return &dir->tables[idx]->pages[off];
    }

    PANIC("No page to get in get_page()!");
    
    return 0;
}

page_directory_t *clone_directory(page_directory_t *src)
{
    u32 phys;
    page_directory_t *dst = (page_directory_t*) kmalloc_ap(sizeof(page_directory_t), &phys);
    memset(dst, 0, sizeof(page_directory_t));
    dst->physicalAddr = phys + ((u32)dst->tablesPhysical - (u32)dst);

    int i, y;
    for (i = 0; i < 1024; ++i)
    {
        if (!src->tables[i])
            continue;

        if (kernel_directory->tables[i])
        {
            dst->tables[i] = kernel_directory->tables[i];
            dst->tablesPhysical[i] = kernel_directory->tablesPhysical[i];
        }
        else
        {
            dst->tables[i] = (page_table_t*) kmalloc_ap(sizeof(page_table_t), &phys);
            memset(&dst->tables[i], 0, sizeof(page_table_t));
            dst->tablesPhysical[i] = phys | 0x7;

            for (y = 0; y < 1024; ++y)
            {
                if (!src->tables[i]->pages[y].frame)
                    continue;

                page_t *src_page = &src->tables[i]->pages[y],
                       *dst_page = &dst->tables[i]->pages[y];

                alloc_frame(dst_page, 0, 0);
                dst_page->present = src_page->present;
                dst_page->rw = src_page->rw;
                dst_page->user = src_page->user;
                dst_page->accessed = src_page->accessed;
                dst_page->dirty = src_page->dirty;

                void *copy_from = (void*) (src_page->frame * 0x1000);
                void *copy_to = (void*) (dst_page->frame * 0x1000);

                asm volatile("cli\n"
                             "mov %cr0, %eax\n"
                             "and %eax, 0x7FFFFFFF\n"
                             "mov %eax, %cr0");

                memcpy(copy_from, copy_to, 0x1000);

                asm volatile("mov %cr0, %eax\n"
                             "or %eax, 0x80000000\n"
                             "mov %eax, %cr0\n"
                             "sti");
            }
        }
    }

    return dst;
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

    dump_regs(regs);

    PANIC("Page Fault");
}
