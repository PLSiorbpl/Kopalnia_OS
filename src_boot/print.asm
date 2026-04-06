bits 32

global Print

section .bss
    global_x resd 1
    global_y resd 1

section .text
Print:
    pusha
    ; color | char
    mov cx, 0x07 ; Color White
    shl cx, 8
    mov cl, dl ; char in r8b
    ; (y*80)
    mov eax, [global_y]
    mov edx, 80
    mul edx
    ; + x
    mov edx, [global_x]
    add eax, edx
    ; Write to vga text mode
    add eax, 0xB8000
    mov [eax], cx
    ; Store x + 2
    add edx, 2
    mov [global_x], edx

    popa
    ret

section .note.GNU-stack noalloc noexec nowrite progbits