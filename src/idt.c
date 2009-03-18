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

/* Defined in asm.S */
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

    idt_set_gate( 0, (u32)isr0 , 0x08, 0x8E);
    idt_set_gate( 1, (u32)isr1 , 0x08, 0x8E);
    idt_set_gate( 2, (u32)isr2 , 0x08, 0x8E);
    idt_set_gate( 3, (u32)isr3 , 0x08, 0x8E);
    idt_set_gate( 4, (u32)isr4 , 0x08, 0x8E);
    idt_set_gate( 5, (u32)isr5 , 0x08, 0x8E);
    idt_set_gate( 6, (u32)isr6 , 0x08, 0x8E);
    idt_set_gate( 7, (u32)isr7 , 0x08, 0x8E);
    idt_set_gate( 8, (u32)isr8 , 0x08, 0x8E);
    idt_set_gate( 9, (u32)isr9 , 0x08, 0x8E);
    idt_set_gate(10, (u32)isr10, 0x08, 0x8E);
    idt_set_gate(11, (u32)isr11, 0x08, 0x8E);
    idt_set_gate(12, (u32)isr12, 0x08, 0x8E);
    idt_set_gate(13, (u32)isr13, 0x08, 0x8E);
    idt_set_gate(14, (u32)isr14, 0x08, 0x8E);
    idt_set_gate(15, (u32)isr15, 0x08, 0x8E);
    idt_set_gate(16, (u32)isr16, 0x08, 0x8E);
    idt_set_gate(17, (u32)isr17, 0x08, 0x8E);
    idt_set_gate(18, (u32)isr18, 0x08, 0x8E);
    idt_set_gate(19, (u32)isr19, 0x08, 0x8E);
    idt_set_gate(20, (u32)isr20, 0x08, 0x8E);
    idt_set_gate(21, (u32)isr21, 0x08, 0x8E);
    idt_set_gate(22, (u32)isr22, 0x08, 0x8E);
    idt_set_gate(23, (u32)isr23, 0x08, 0x8E);
    idt_set_gate(24, (u32)isr24, 0x08, 0x8E);
    idt_set_gate(25, (u32)isr25, 0x08, 0x8E);
    idt_set_gate(26, (u32)isr26, 0x08, 0x8E);
    idt_set_gate(27, (u32)isr27, 0x08, 0x8E);
    idt_set_gate(28, (u32)isr28, 0x08, 0x8E);
    idt_set_gate(29, (u32)isr29, 0x08, 0x8E);
    idt_set_gate(30, (u32)isr30, 0x08, 0x8E);
    idt_set_gate(31, (u32)isr31, 0x08, 0x8E);

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

/*
 * Gets called when an interrupt occurs.
 */
void isr_handler(registers_t regs)
{
    if (regs.int_no <= 18)
    {
        if (regs.int_no == 8 || (regs.int_no >= 10 && regs.int_no <= 14))
        {
            printf("%s: %d\n", exception_names[regs.int_no], regs.err_code);
        }
        else
        {
            printf("%s\n", exception_names[regs.int_no]);
        }
    }
    else
    {
        printf("Unknown exception: %d\n", regs.int_no);
    }
}
