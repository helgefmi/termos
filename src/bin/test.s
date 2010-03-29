[BITS 32]

[SECTION .text]
[GLOBAL _start]
_start:
    mov eax, 0x1337
    int 80
