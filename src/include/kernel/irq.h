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

#ifndef _IRQ_H
#define _IRQ_H

#include <kernel/common.h>

/* IRQ numbers mapped to their IDT index */
#define IRQ0  (32)
#define IRQ1  (33)
#define IRQ2  (34)
#define IRQ3  (35)
#define IRQ4  (36)
#define IRQ5  (37)
#define IRQ6  (38)
#define IRQ7  (39)
#define IRQ8  (40)
#define IRQ9  (41)
#define IRQ10 (42)
#define IRQ11 (43)
#define IRQ12 (44)
#define IRQ13 (45)
#define IRQ14 (46)
#define IRQ15 (47)

#define PIC1_CMD (0x20)
#define PIC2_CMD (0xA0)
#define PIC1_DATA (PIC1_CMD+1)
#define PIC2_DATA (PIC2_CMD+1)

#define PIC1_OFFSET (0x20)
#define PIC2_OFFSET (0x28)

void init_pic();

#endif
