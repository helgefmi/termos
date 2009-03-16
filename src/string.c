#include "string.h"
#include "common.h"

void *memset(void *src, int c, size_t num)
{
    u8 *src_u8 = src;
    while (num--)
        *src_u8++ = c;

    return src;
}

void *wmemset(void *src, int c, size_t num)
{
    u16 *src_u16 = src;
    while (num--)
        *src_u16++ = c;

    return src;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    u8 *dst_u8 = dst;
    const u8 *src_u8 = src;

    while (n--)
        *dst_u8++ = *src_u8++;

    return dst;
}

void *wmemcpy(void *dst, const void *src, size_t n)
{
    u16 *dst_u16 = dst;
    const u16 *src_u16 = src;

    while (n--)
        *dst_u16++ = *src_u16++;

    return dst;
}
