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
#include <lib/stdio.h>

void outb(u16 port, u8 value)
{
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

u8 inb(u16 port)
{
    u8 ret;
    asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

u16 inw(u16 port)
{
    u16 ret;
    asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

void dump_regs(registers_t *r)
{
   printf("\
ds: %x \n\
esi: %x, edi: %x, ebp: %x, esp: %x, eip: %x \n\
eax: %x, ebx: %x, ecx: %x, edx: %x \n\
int_no: %x, err_code: %x \n\
cs: %x, eflags: %x, useresp: %x, ss: %x \n",
   r->ds,
   r->esi, r->edi, r->ebp, r->esp, r->eip, 
   r->eax, r->ebx, r->ecx, r->edx,
   r->int_no, r->err_code,
   r->cs, r->eflags, r->useresp, r->ss);
}

