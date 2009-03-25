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

#include "tty.h"
#include "common.h"
#include "string.h"
#include "ctype.h"

tty_t tty = {(u16*)0xB8000, 0, 0, 0x07};

void puts(char *str)
{
    /* Prints a string */
    while (*str)
        putchar(*str++);
}

void putchar(char c)
{
    /* Prints a char */
    switch (c)
    {
        case '\n':
            putchar('\r');
            ++tty.y;
            break;

        case '\r':
            tty.x = 0;
            break;

        case '\t':
            tty.x += 4;
            tty.x &= 0xfc; /* Remove the last 2 bits making it aligned by 4 */
            break;

        default:
            if (isprint(c))
            {
                tty.buf[tty.y * SCR_WIDTH + tty.x] = c | (tty.attr << 8);
                ++tty.x;
            }
            break;
    }

    if (tty.x >= SCR_WIDTH)
    {
        tty.x = 0;
        ++tty.y;
    }

    if (tty.y >= SCR_HEIGHT)
    {
        tty.y = SCR_HEIGHT-1;
        tty_scroll();
    }

    tty_move_cursor();
}

void tty_clear()
{
    /* Resets the whole screen */
    wmemset(tty.buf, ' ' | (0x07) << 8, SCR_SIZE * sizeof(u16));

    /* Resets the cursor */
    tty.x = tty.y = 0;
    tty_move_cursor();
}

void tty_scroll()
{
    /* Move everything one row up */
    u16 *src = (u16*)(tty.buf + SCR_WIDTH);
    wmemcpy(tty.buf, src, (SCR_SIZE - SCR_WIDTH) * sizeof(u16));

    /* Reset the last line */
    wmemset(tty.buf + SCR_SIZE - SCR_WIDTH, ' ' | (0x07) << 8, SCR_WIDTH);
}

void tty_move_cursor()
{
    u16 idx = tty.y * SCR_WIDTH + tty.x;

    outb(0x3D4, 14);
    outb(0x3D5, idx >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, idx);
}
