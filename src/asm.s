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

;
; interrupt.s -- Contains interrupt service routine wrappers.
;                Based on Bran's kernel development tutorials.
;                Rewritten for JamesM's kernel development tutorials.

; This macro creates a stub for an ISR which does NOT pass it's own
; error code (adds a dummy errcode byte).
%macro ISR_NOERRCODE 1
  global isr%1
  isr%1:
    cli                         ; Disable interrupts firstly.
    push byte 0                 ; Push a dummy error code.
    push byte %1                ; Push the interrupt number.
    jmp isr_common_stub         ; Go to our common handler code.
%endmacro

; This macro creates a stub for an ISR which passes it's own
; error code.
%macro ISR_ERRCODE 1
  global isr%1
  isr%1:
    cli                         ; Disable interrupts.
    push byte %1                ; Push the interrupt number
    jmp isr_common_stub
%endmacro
 
; In isr.c
extern isr_handler

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
isr_common_stub:
    pusha               ; Pushes edi, esi, ebp, esp, ebx, edx, ecx, eax

    mov ax, ds          ; Lower 16-bits of eax = ds.
    push eax            ; save the data segment descriptor

    mov ax, 0x10        ; load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call isr_handler

    pop ebx             ; reload the original data segment descriptor
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa                ; Pops edi,esi,ebp...
    add esp, 8          ; Cleans up the pushed error code and pushed ISR number
    sti
    iret                ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

; Set up ISR's by the number of arguments they have
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31
