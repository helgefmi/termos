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

#include "isr.h"
#include "stdio.h"
#include "irq.h"

#define PIC_EOI (0x20)

/* Used for more userfriendly errors due to interrupts */
static char* exception_names[] = {
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

static isr_handler_t isr_handlers[255] = {0};

void register_isr_handler(u8 num, isr_handler_t handler)
{
    isr_handlers[num] = handler;
}

void isr_handler(registers_t *regs)
{
    if (isr_handlers[regs->int_no])
    {
        isr_handlers[regs->int_no](regs);
    }
    else
    {
        if (regs->int_no <= 31)
        {
            if (regs->int_no == 8 || (regs->int_no >= 10 && regs->int_no <= 14))
            {
                printf("%s: %d\n", exception_names[regs->int_no], regs->err_code);
            }
            else
            {
                printf("%s\n", exception_names[regs->int_no]);
            }
        }
        else
        {
            printf("Unknown exception: %d\n", regs->int_no);
        }
    }
}


/* Tell's the PIC(s) that we got the interrupt (so it can continue sending interrupts) */
static void pic_send_eoi(int irq)
{
    if (irq >= 8)
    {
        /* Need to send an 'end of interrupt' to the slave PIC too, if it was sent from the slave */
        outb(PIC2_CMD, PIC_EOI);
    }
    outb(PIC1_CMD, PIC_EOI);
}

/* When an IRQ interrupt is sent, this will get called */
void irq_handler(registers_t *regs)
{
    if (isr_handlers[regs->int_no])
    {
        isr_handlers[regs->int_no](regs);
    }

    pic_send_eoi(regs->int_no-32);
}
