#include "common.h"
#include "tty.h"
#include "mem.h"
#include "gdt.h"
#include "idt.h"

int main()
{
    init_tty();
    init_memory();
    init_gdt();
    init_idt();

    printf("\nBai!\n");
    return 0x12345678;
}
