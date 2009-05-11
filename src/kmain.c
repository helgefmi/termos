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
#include <fs/devfs/devfs.h>
#include <lib/string.h>

multiboot_header_t *multiboot_header;

extern heap_t *kheap;

void test(struct vnode *node, u32 pad)
{
    u32 i = 0;
    if (pad)
        printf("|");
    while (i++ < pad)
        printf("-");
    if (pad)
        printf(" ");

    printf("%s\n", node->v_name);

    if (V_ISDIR(node->v_flags))
    {
        DIR *dh = vfs_opendir(node);
        while ((node = vfs_readdir(dh)) != NULL)
            test(node, pad + 2);
        vfs_closedir(dh);
    }
    else if (V_ISFILE(node->v_flags))
    {
        i = 0;
        while (i++ < pad + 2)
            printf("-");
        printf("-> ");

        FILE* fh = vfs_open(node, VFS_READ);
        char buf[1025];

        size_t ret = vfs_read(fh, buf, 1024);
        buf[ret] = '\0';
        printf("\"%s\"\n", buf);
        vfs_close(fh);
    }
}

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

    sti();

    vfs_init();
    init_initrd();
    init_devfs();

    vfs_mount(v_root, FSTYPE_INITRD, initrd_start, VFS_READ);
    struct vnode *test_mnt = vfs_lookup(v_root, "test_mnt");
    vfs_mount(test_mnt, FSTYPE_INITRD, initrd_start, VFS_READ);
    vfs_unmount(test_mnt);
    vput(test_mnt);

    struct vnode *dev_node = vfs_lookup(v_root, "dev");
    vfs_mount(dev_node, FSTYPE_DEVFS, 0, VFS_READ | VFS_WRITE);
    vput(dev_node);

    debug_vnode(v_root);

    printf("%d\n", kheap->allocated);
    test(v_root, 0);
    printf("%d\n", kheap->allocated);

    printf("bai\n");

    cli();
    return 0x12345678;
}
