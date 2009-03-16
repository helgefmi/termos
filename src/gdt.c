#include "gdt.h"
#include "stdio.h"
#include "common.h"
#include "string.h"

/* Defined in boot.S */
extern void gdt_flush();

gdt_entry_t gdt_entries[3];
gdt_ptr_t   gdt_ptr;

void init_gdt()
{
    printf("Initializing GDT..");
    /* Used in flush_gdt to tell the processor where our GDT is, and it's size */
    gdt_ptr.base = (u32)gdt_entries;
    gdt_ptr.limit = sizeof(gdt_entries) - 1;

    /* For good measure and the NULL descriptor (idx 0) */
    memset(gdt_entries, 0, sizeof(gdt_entries));

    /* Code segment */
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    /* Data segment */
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    /* And flush the GDT, making it active */
    gdt_flush();
    printf("\t\tOK. Installed %d entries at %x.\n", sizeof(gdt_entries)/sizeof(gdt_entry_t), gdt_ptr.base);
}

void gdt_set_gate(s32 idx, u32 base, u32 limit, u8 access, u8 gran)
{
    /* These are explained in gdt.h */
    gdt_entries[idx].base_low = base & 0xFFFF;
    gdt_entries[idx].base_middle = (base >> 16) & 0xFF;
    gdt_entries[idx].base_high = (base >> 24) & 0xFF;

    gdt_entries[idx].limit_low = limit & 0xFFFF;
    gdt_entries[idx].granularity = (limit >> 16) & 0x0F;

    gdt_entries[idx].granularity = gran & 0xF0;
    gdt_entries[idx].access = access;
}
