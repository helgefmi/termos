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

[GLOBAL idt_flush]
[EXTERN idt_ptr]                ; Defined in idt.c

idt_flush:
   lidt [idt_ptr]               ; Load the IDT pointer.
   ret

; This is used for interrupts that doesn't push an error code
%macro ISR_NOERRCODE 1
  global isr%1
  isr%1:
    cli
    push byte 0                 ; Push a dummy error code.
    push byte %1                ; Push the interrupt number.
    jmp isr_common_stub
%endmacro

; This is used for interrupts that pushes an error code
%macro ISR_ERRCODE 1
  global isr%1
  isr%1:
    cli
    push byte %1                ; Push the interrupt number
    jmp isr_common_stub
%endmacro
 
extern isr_handler              ; Defined in idt.c

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
isr_common_stub:
    pusha                       ; Pushes edi, esi, ebp, esp, ebx, edx, ecx, eax

    mov ax, ds                  ; Lower 16-bits of eax = ds.
    push eax                    ; save the data segment descriptor

    mov ax, 0x10                ; load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, esp
    push eax

    call isr_handler

    pop eax
    mov esp, eax

    pop ebx                     ; reload the original data segment descriptor
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa                        ; Pops edi,esi,ebp...
    add esp, 8                  ; Cleans up the pushed error code and pushed ISR number
    sti
    iret                        ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

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
