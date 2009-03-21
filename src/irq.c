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
#include "isr.h"

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

    /* Not sure why/if these are needed */
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    /* Restore the initial state of the data ports */
    outb(PIC1_DATA, data1);
    outb(PIC2_DATA, data2);

    printf("\t\tOK. PIC1=%x PIC2=%x\n", PIC1_OFFSET, PIC2_OFFSET);
}
