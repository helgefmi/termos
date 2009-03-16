#include "tty.h"
#include "common.h"
#include "string.h"
#include "ctype.h"

tty_t tty;

void init_tty()
{
    /* Video buffer adress */
    tty.buf = (u16*)0xB8000;

    /* Gray on white */
    tty.attr = 0x07;

    /* Start with a clean screen with cursor on the first index */
    tty_clear();
}

void puts(char *str)
{
    /* Prints a string */
    while(*str)
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
            tty.x = (tty.x >> 2) << 2;
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

        if (tty.y >= SCR_HEIGHT)
        {
            tty.y = SCR_HEIGHT-1;
            tty_scroll();
        }
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
