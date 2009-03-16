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

void init_tty();

void putchar(char);
void puts(char*);
void tty_scroll();
void tty_clear();
void tty_move_cursor();

#endif
