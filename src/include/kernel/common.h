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

#ifndef _COMMON_H
#define _COMMON_H

#define NULL (0)
#define INT_MAX (4294967295ul)
#define PANIC(msg) \
    { \
    printf("Kernel PANIC (%s:%d): %s\n", __FILE__, __LINE__, (msg)); \
    asm volatile("cli;hlt"); \
    }

#define ASSERT(expr) \
    if (!(expr)) \
    { \
        printf("%s\n", #expr); \
        PANIC("Failed assertion"); \
    }
typedef unsigned long long int  u64;
typedef signed long long int    s64;
typedef unsigned long int       u32;
typedef signed long int         s32;
typedef unsigned short          u16;
typedef signed short            s16;
typedef unsigned char           u8;
typedef signed char             s8;

typedef u32 size_t;

typedef struct registers
{
   u32 ds;                                       /* Data segment selector */
   u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;   /* Pushed by pusha. */
   u32 int_no, err_code;                         /* Interrupt number and error code (if applicable) */
   u32 eip, cs, eflags, useresp, ss;             /* Pushed by the processor automatically. */
} registers_t; 

#define MULTIBOOT_FLAG_MEM     0x001
#define MULTIBOOT_FLAG_DEVICE  0x002
#define MULTIBOOT_FLAG_CMDLINE 0x004
#define MULTIBOOT_FLAG_MODS    0x008
#define MULTIBOOT_FLAG_AOUT    0x010
#define MULTIBOOT_FLAG_ELF     0x020
#define MULTIBOOT_FLAG_MMAP    0x040
#define MULTIBOOT_FLAG_CONFIG  0x080
#define MULTIBOOT_FLAG_LOADER  0x100
#define MULTIBOOT_FLAG_APM     0x200
#define MULTIBOOT_FLAG_VBE     0x400

typedef struct multiboot_header
{
   u32 flags;
   u32 mem_lower;
   u32 mem_upper;
   u32 boot_device;
   u32 cmdline;
   u32 mods_count;
   u32 mods_addr;
   u32 num;
   u32 size;
   u32 addr;
   u32 shndx;
   u32 mmap_length;
   u32 mmap_addr;
   u32 drives_length;
   u32 drives_addr;
   u32 config_table;
   u32 boot_loader_name;
   u32 apm_table;
   u32 vbe_control_info;
   u32 vbe_mode_info;
   u32 vbe_mode;
   u32 vbe_interface_seg;
   u32 vbe_interface_off;
   u32 vbe_interface_len;
}  __attribute__((packed)) multiboot_header_t;

void outb(u16, u8);
u8 inb(u16);
u16 inw(u16);

extern int debug;
extern multiboot_header_t *multiboot_header;

#endif
