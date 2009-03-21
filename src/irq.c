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

#include "irq.h"
#include "common.h"
#include "idt.h"
#include "stdio.h"

#define PIC_EOI (0x20)
#define PIC1_CMD (0x20)
#define PIC2_CMD (0xA0)
#define PIC1_DATA (PIC1_CMD+1)
#define PIC2_DATA (PIC2_CMD+1)

#define PIC1_OFFSET (0x20)
#define PIC2_OFFSET (0x28)

irq_handler_t irq_handlers[16] = {0};

void init_pic()
{
    printf("Remapping the PIC..");

    /* Save the initial state of the data ports */
    u8 data1, data2;
    data1 = inb(PIC1_DATA);
    data2 = inb(PIC2_DATA);

    /* Start initialization */
    outb(PIC1_CMD, 0x11);
    outb(PIC2_CMD, 0x11);

    /* Map IRQ 1-8 to IDT entries 32-40 and IRQ 9-16 to IDT entries 41-48 */
    outb(PIC1_DATA, PIC1_OFFSET);
    outb(PIC2_DATA, PIC2_OFFSET);

    /* Dunno what this really does :'( */
    outb(PIC1_DATA, 4);
    outb(PIC2_DATA, 2);

    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    /* Restore the initial state of the data ports */
    outb(PIC1_DATA, data1);
    outb(PIC2_DATA, data2);

    printf("\t\tOK. PIC1=%x PIC2=%x\n", PIC1_OFFSET, PIC2_OFFSET);
}

void init_irq()
{
    printf("Setting up IRQ gates..");

    idt_set_gate(32, (u32)irq0 , 0x08, 0x8E);
    idt_set_gate(33, (u32)irq1 , 0x08, 0x8E);
    idt_set_gate(34, (u32)irq2 , 0x08, 0x8E);
    idt_set_gate(35, (u32)irq3 , 0x08, 0x8E);
    idt_set_gate(36, (u32)irq4 , 0x08, 0x8E);
    idt_set_gate(37, (u32)irq5 , 0x08, 0x8E);
    idt_set_gate(38, (u32)irq6 , 0x08, 0x8E);
    idt_set_gate(39, (u32)irq7 , 0x08, 0x8E);
    idt_set_gate(40, (u32)irq8 , 0x08, 0x8E);
    idt_set_gate(41, (u32)irq9 , 0x08, 0x8E);
    idt_set_gate(42, (u32)irq10, 0x08, 0x8E);
    idt_set_gate(43, (u32)irq11, 0x08, 0x8E);
    idt_set_gate(44, (u32)irq12, 0x08, 0x8E);
    idt_set_gate(45, (u32)irq13, 0x08, 0x8E);
    idt_set_gate(46, (u32)irq14, 0x08, 0x8E);
    idt_set_gate(47, (u32)irq15, 0x08, 0x8E);

    printf("\tOK.");
}

void register_irq_handler(u8 num, irq_handler_t handler)
{
    if (num <= 15)
    {
        irq_handlers[num] = handler;
    }
}

/* Tell's the IRQ that I got the interrupt (so it can continue sending interrupts) */
void pic_send_eoi(int irq)
{
    if (irq >= 8)
    {
        /* Need to send an 'end of interrupt' to the slave PIC too, if it was sent from the slave */
        outb(PIC2_CMD, PIC_EOI);
    }
    outb(PIC1_CMD, PIC_EOI);
}

/* When an interrupt is sent due to an IRQ, this is where it ends up */
void irq_handler(registers_t regs)
{
    pic_send_eoi(regs.int_no);
    printf("hm\n");
}
