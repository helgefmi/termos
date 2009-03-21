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
