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

#ifndef _IDT_H
#define _IDT_H

#include "common.h"

typedef struct
{
   u16 base_lo; /* The lower 16 bits of the address to jump to when this interrupt fires */
   u16 sel;     /* Kernel segment selector */
   u8  always0;
   u8  flags;
   u16 base_hi; /* The upper 16 bits of the address to jump to */
} __attribute__((packed)) idt_entry_t;

typedef struct
{
   u16 limit;   /* Size of the IDT entries */
   u32 base;    /* Location of the first IDT entry */
} __attribute__((packed))idt_ptr_t;

void init_idt();

void idt_set_gate(u8, u32, u16, u8);

#endif
