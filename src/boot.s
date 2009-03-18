;
;  This file is part of TermOS.
;
;  TermOS is free software: you can redistribute it and/or modify
;  it under the terms of the GNU General Public License as published by
;  the Free Software Foundation, either version 3 of the License, or
;  (at your option) any later version.
;
;  TermOS is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;  GNU General Public License for more details.
;
;  You should have received a copy of the GNU General Public License
;  along with TermOS.  If not, see <http://www.gnu.org/licenses/>.
;

MBOOT_PAGE_ALIGN    equ 1<<0
MBOOT_MEM_INFO      equ 1<<1
MBOOT_HEADER_MAGIC  equ 0x1BADB002
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

[BITS 32]

[GLOBAL mboot]

; These are defined in link.ld
[EXTERN code]
[EXTERN bss]
[EXTERN end]

mboot:
    dd  MBOOT_HEADER_MAGIC
    dd  MBOOT_HEADER_FLAGS
    dd  MBOOT_CHECKSUM

    dd  mboot
    dd  code
    dd  bss
    dd  end
    dd  start

[GLOBAL start]
[EXTERN main]       ; Defined in main.c

start:
    push    ebx
    cli
    call main
    jmp $

; GDT STUFF
[GLOBAL gdt_flush]
[EXTERN gdt_ptr]    ; Defined in gdt.c

gdt_flush:
    lgdt [gdt_ptr]  ; Load the new GDT pointer

    mov ax, 0x10    ; 0x10 is the offset in the GDT to our data segment
    mov ds, ax      ; Load all data segment selectors
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush ; 0x08 is the offset to our code segment: Far jump!
.flush:
    ret

; IDT STUFF
[GLOBAL idt_flush]
[EXTERN idt_ptr]    ; Defined in idt.c

idt_flush:
   lidt [idt_ptr]   ; Load the IDT pointer.
   ret
