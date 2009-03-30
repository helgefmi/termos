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

#include "string.h"
#include "common.h"
#include "tty.h"
#include "mem.h"
#include "gdt.h"
#include "idt.h"
#include "irq.h"
#include "timer.h"
#include "paging.h"
#include "heap.h"
#include "fs.h"

int debug;
extern page_directory_t *kernel_directory;

multiboot_header_t *multiboot_header;
int kmain(multiboot_header_t *_multiboot_header)
{
    /* Storing this info so other components can use it */
    multiboot_header = _multiboot_header;

    tty_clear();

   printf("flags: %x\n", multiboot_header->flags);
   printf("mem_lower: %x\n", multiboot_header->mem_lower);
   printf("mem_upper: %x\n", multiboot_header->mem_upper);
   printf("boot_device: %x\n", multiboot_header->boot_device);
   printf("cmdline: %x\n", multiboot_header->cmdline);
   printf("mods_count: %x\n", multiboot_header->mods_count);
   printf("mods_addr: %x\n", multiboot_header->mods_addr);
   printf("num: %x\n", multiboot_header->num);
   printf("size: %x\n", multiboot_header->size);
   printf("addr: %x\n", multiboot_header->addr);
   printf("shndx: %x\n", multiboot_header->shndx);
   printf("mmap_length: %x\n", multiboot_header->mmap_length);
   printf("mmap_addr: %x\n", multiboot_header->mmap_addr);
   printf("drives_length: %x\n", multiboot_header->drives_length);
   printf("drives_addr: %x\n", multiboot_header->drives_addr);
   printf("config_table: %x\n", multiboot_header->config_table);
   printf("boot_loader_name: %x\n", multiboot_header->boot_loader_name);
   printf("apm_table: %x\n", multiboot_header->apm_table);
   printf("vbe_control_info: %x\n", multiboot_header->vbe_control_info);
   printf("vbe_mode_info: %x\n", multiboot_header->vbe_mode_info);
   printf("vbe_mode: %x\n", multiboot_header->vbe_mode);
   printf("vbe_interface_seg: %x\n", multiboot_header->vbe_interface_seg);
   printf("vbe_interface_off: %x\n", multiboot_header->vbe_interface_off);
   printf("vbe_interface_len: %x\n", multiboot_header->vbe_interface_len);
    
    init_gdt();
    init_idt();
    init_paging();
    init_heap();

    init_pic();
    init_timer(100);

    //asm volatile ("sti");

    void *a;
    int i;

    for(i=0; i < 10; ++i)
    {
        a = (void*)kmalloc_a(0x100000);
        a = (void*)kmalloc(0x123);
        kfree(a);
    }

    //debug_heap();


    for (;;);
    return 0x12345678;
}
