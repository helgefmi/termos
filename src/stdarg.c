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

#include "stdarg.h"
#include "tty.h"

static char hex_table[] = "0123456789ABCDEF";

int vsprintf(char *buf, const char *fmt, va_list args)
{
    return vsnprintf(buf, INT_MAX, fmt, args);
}

/* Helper function for vsnprintf*/
static void reverse(char *buf, size_t left, size_t right)
{
    char itmp;
    for (; left < right; ++left, --right)
    {
        itmp = buf[left];
        buf[left] = buf[right];
        buf[right] = itmp;
    }
}

int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
    /* s */
    char *str;

    /* d, x */
    s32 i;
    u32 ui;
    int idxl;
    int idxr;

    /* current length of buffer */
    size_t idx = 0;

    while (*fmt && idx < size)
    {
        if (*fmt != '%')
        {
            buf[idx++] = *fmt++;
        }
        else
        {
            /* check if we're done before the switch */
            if (!*(++fmt))
                break;

            switch (*fmt++)
            {
                case 's':
                    /* Read a string into buf */
                    str = va_arg(args, char*);
                    while (idx < size && *str)
                    {
                        buf[idx++] = *str++;
                    }
                    break;
                case 'x':
                    /* Append the least significant number (base 16) and shift right by 4 untill i == 0
                     * Then reverse the buffer containing the number
                     */
                    i = va_arg(args, s32);
                    ui = (u32)i;

                    buf[idx++] = '0';
                    buf[idx++] = 'x';

                    idxl = idx;
                    do
                    {
                        buf[idx++] = hex_table[(ui&0xf)];
                        ui >>= 4;
                    } while (ui);
                    idxr = idx-1;

                    reverse(buf, idxl, idxr);
                    break;
                case 'd':
                    /* Same as hex(x) for base 10
                     */
                    i = va_arg(args, s32);
                    if (i < 0) {
                        buf[idx++] = '-';
                        i *= -1;
                    }

                    idxl = idx;
                    do
                    {
                        buf[idx++] = '0' + (i%10);
                        i /= 10;
                    } while (i);
                    idxr = idx-1;

                    reverse(buf, idxl, idxr);

                    break;
                case '%':
                    buf[idx++] = '%';
                    break;
                default:
                    /* unsure what to do here.. */
                    break;
            }
        }
    }

    return idx;
}

/*
int vprintf(const char *fmt, va_list args)
{
}

int vfprintf(FILE *stream, const char *fmt, va_list args)
{
}
*/

