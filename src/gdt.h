#ifndef _GDT_H
#define _GDT_H

#include "common.h"

typedef struct
{
    u16 limit_low;   /* The lower 16 bits of the limit */
    u16 base_low;    /* The lower 16 bits of the base */
    u8  base_middle; /* The next 8 bits of the base */
    u8  access;      /* Access flags, determine what ring this segment can be used in */
    u8  granularity;
    u8  base_high;   /* The last 8 bits of the base */
} __attribute__((packed)) gdt_entry_t; 

typedef struct
{
   u16 limit; /* Size of the GDT entries */
   u32 base;  /* Location of the first GDT entry */
} __attribute__((packed)) gdt_ptr_t; 

void init_gdt();

void gdt_set_gate(s32, u32, u32, u8, u8);

#endif
