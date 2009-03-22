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

%macro IRQ 2
    global irq%1
    irq%1:
        cli
        push byte %1
        push byte %2
        jmp irq_common_stub
%endmacro

[EXTERN irq_handler]            ; Defined in irq.c
irq_common_stub:
    pusha                       ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

    mov ax, ds                  ; Lower 16-bits of eax = ds.
    push eax                    ; save the data segment descriptor

    mov ax, 0x10                ; load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, esp
    push eax

    call irq_handler

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

IRQ 0,  32
IRQ 1,  33
IRQ 2,  34
IRQ 3,  35
IRQ 4,  36
IRQ 5,  37
IRQ 6,  38
IRQ 7,  39
IRQ 8,  40
IRQ 9,  41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47
