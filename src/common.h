#ifndef _COMMON_H
#define _COMMON_H

#define INT_MAX (4294967295ul)

typedef unsigned long long int  u64;
typedef signed long long int    s64;
typedef unsigned long int       u32;
typedef signed long int         s32;
typedef unsigned short          u16;
typedef signed short            s16;
typedef unsigned char           u8;
typedef signed char             s8;

typedef u32 size_t;
typedef u32 FILE;

void outb(u16, u8);
u8 inb(u16);
u16 inw(u16);

#endif
