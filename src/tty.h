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

#ifndef _SCREEN_H
#define _SCREEN_H

#include "common.h"
#include "stdio.h"

#define SCR_WIDTH (80)
#define SCR_HEIGHT (25)
#define SCR_SIZE (SCR_WIDTH * SCR_HEIGHT)

typedef struct
{
    u16 *buf;   /* Pointer to the video bufer */
    u8 x,y;     /* Cursor location */
    u8 attr;    /* Text attributes (colors, inverse, bold) */
} tty_t;

void putchar(char);
void puts(char*);
void tty_scroll();
void tty_clear();
void tty_move_cursor();

#endif
