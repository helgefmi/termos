[BITS 32]

[GLOBAL read_eip]
read_eip:
    pop eax
    jmp eax 

[GLOBAL read_ebp]
read_ebp:
    mov eax, ebp
    ret

[GLOBAL read_esp]
read_esp:
    mov eax, esp
    ret
