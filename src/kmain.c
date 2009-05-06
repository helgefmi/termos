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

#include <kernel/common.h>
#include <kernel/tty.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <mm/mem.h>
#include <mm/paging.h>
#include <mm/heap.h>
#include <fs/vfs.h>
#include <fs/initrd.h>
#include <fs/vfs_cache.h>
#include <lib/string.h>

extern page_directory_t *kernel_directory;
multiboot_header_t *multiboot_header;

int kmain(multiboot_header_t *_multiboot_header)
{
    /* Storing this info so other components can use it */
    multiboot_header = _multiboot_header;

    tty_clear();

    ASSERT(multiboot_header->mods_count);

    u32 initrd_start = *((u32*) multiboot_header->mods_addr);
    u32 initrd_end   = *((u32*) (multiboot_header->mods_addr + 4));

    set_end_of_mem(initrd_end);

    init_gdt();
    init_idt();
    init_paging();
    init_heap();

    init_pic();
    init_timer(100);

    asm volatile ("sti");

    vfs_init();
    init_initrd();

    vfs_mount(v_root, FSTYPE_INITRD, initrd_start, VFS_READ);
    
    /* TEST */
    struct vnode *node;

    node = vfs_vname("/home/");
    debug_vnode(node);
    vfs_mount(node, FSTYPE_INITRD, initrd_start, VFS_READ);

    node = vfs_vname("/home/home/amazing");
    debug_vnode(node);

//    debug_heap();
    printf("bai\n");

    return 0x12345678;
}
