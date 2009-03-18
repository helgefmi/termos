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

#include "idt.h"
#include "string.h"
#include "stdio.h"

/* Defined in boot.S */
extern void idt_flush();

idt_entry_t idt_entries[255];
idt_ptr_t idt_ptr;

/* Used for more userfriendly errors due to interrupts */
char* exception_names[] = {
    "Division by zero exception",
    "Debug exception",
    "Non maskable interrupt",
    "Breakpoint exception",
    "Into detected overflow",
    "Out of bounds exception",
    "Invalid opcode exception",
    "No coprocessor exception",
    "Double fault",
    "Coprocessor segment overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown interrupt exception",
    "Coprocessor fault",
    "Alignment check exception",
    "Machine check exception",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
};

void init_idt()
{
    printf("Initializing IDT..");

    /* Tell the processor where our entries are, and their size */
    idt_ptr.base = (u32)idt_entries;
    idt_ptr.limit = sizeof(idt_entries) - 1;

    /* Must set everything to zero by default, so interrupts won't be fired without an ISR */
    memset(idt_entries, 0, sizeof(idt_entries));

    u8 idx;
    for (idx = 0; idx <= 32; ++idx)
    {
    }

    /* Start using the ISR's we just set up! */
    idt_flush();

    printf("\t\tOK. Installed %d entries at %x.\n", sizeof(idt_entries)/sizeof(idt_entry_t), idt_ptr.base);
}

void idt_set_gate(u8 idx, u32 base, u16 sel, u8 flags)
{
    /* These are explained in idt.h */
    idt_entries[idx].base_lo = base & 0xFFFF;
    idt_entries[idx].base_hi = (base >> 16) & 0xFFFF;

    idt_entries[idx].always0 = 0;

    idt_entries[idx].flags = flags;
    idt_entries[idx].sel = sel;
}
