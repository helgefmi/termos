#include "stdarg.h"
#include "tty.h"

char *hex_table = "0123456789ABCDEF";

/*
int vprintf(const char *fmt, va_list args)
{
}

int vfprintf(FILE *stream, const char *fmt, va_list args)
{
}
*/

int vsprintf(char *buf, const char *fmt, va_list args)
{
    return vsnprintf(buf, INT_MAX, fmt, args);
}

/* Helper function for vsnprintf*/
void reverse(char *buf, size_t left, size_t right)
{
    char itmp;
    for(;left < right; ++left, --right)
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
                    if(i < 0) {
                        buf[idx++] = '-';
                        i *= -1;
                    }

                    buf[idx++] = '0';
                    buf[idx++] = 'x';

                    idxl = idx;
                    do
                    {
                        buf[idx++] = hex_table[(i&0xf)];
                        i >>= 4;
                    } while(i);
                    idxr = idx-1;

                    reverse(buf, idxl, idxr);
                    break;
                case 'd':
                    /*
                     * Same as hex(x) for base 10
                     */
                    i = va_arg(args, s32);
                    if(i < 0) {
                        buf[idx++] = '-';
                        i *= -1;
                    }

                    idxl = idx;
                    do
                    {
                        buf[idx++] = '0' + (i%10);
                        i /= 10;
                    } while(i);
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
