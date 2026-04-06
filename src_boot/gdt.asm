bits 32

global gdt_descriptor

section .text
gdt_start:
gdt_null:
    dq 0x0000000000000000
gdt_32_code:
    dw 0xffff
    dw 0
    db 0
    db 0x9a
    db 11001111b
    db 0
gdt_32_data:
    dw 0xffff
    dw 0
    db 0
    db 0x92
    db 11001111b
    db 0
gdt_64_code:
    dw 0x0000
    dw 0x0000
    db 0x00
    db 0x9a
    db 0x20
    db 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

section .note.GNU-stack noalloc noexec nowrite progbits
