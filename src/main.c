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

int debug;
extern page_directory_t *kernel_directory;

int main()
{
    tty_clear();

    init_gdt();
    init_idt();
    init_paging();
    init_heap();

    init_pic();
    init_timer(100);

    //asm volatile ("sti");

    printf("BEGIN\n");

    void *a;
    int i;

    for(i=0; i < 10; ++i)
    {
        a = (void*)kmalloc_a(0x100000);
        a = (void*)kmalloc(0x123);
        kfree(a);
    }

    //debug_heap();

    printf("END\n");
    

    for (;;);
    return 0x12345678;
}
