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

#include "timer.h"
#include "stdio.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"

#define TIMER_CHAN1 (0x40)
#define TIMER_CHAN2 (0x41)
#define TIMER_CHAN3 (0x42)
#define TIMER_CMD   (0x43)

u32 ticks = 0;
void timer_callback(registers_t regs)
{
    ++ticks;
    if(ticks%100 == 0)
        printf("%d\n", ticks);
}

void init_timer(u32 freq)
{
    printf("Setting up timer..");

    register_isr_handler(IRQ0, &timer_callback);

    u32 divisor = 1193180 / freq;

    outb(TIMER_CMD, 0x36);
    outb(TIMER_CHAN1, divisor & 0xFF);
    outb(TIMER_CHAN1, (divisor >> 8) & 0xFF);

    printf("\t\tOK. Freq: %dhz\n", freq);
 }
