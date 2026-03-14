BITS 32
global _start
extern kernel_main

section .multiboot_header
align 4
dd 0x1BADB002       ; magic
dd 0x0              ; flags (align + memory info)
dd -(0x1BADB002 + 0x0)

section .text
_start:
    call kernel_main
    cli
.hang:
    hlt
    jmp .hang

section .note.GNU-stack noalloc noexec nowrite progbits