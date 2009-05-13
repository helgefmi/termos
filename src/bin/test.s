[BITS 32]

[SECTION .bss]
    ape: resw 11024

[SECTION .data]
    msg: db "Hello World from /bin/test!",10,0

[SECTION .text]
[GLOBAL _start]
_start:
    mov dword [ape], dword 0x1234
    mov eax, [ape]

    ret
