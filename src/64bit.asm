bits 64

global Kernel_64
extern kernel_main

section .text

Kernel_64:
    jmp kernel_main

section .note.GNU-stack noalloc noexec nowrite progbits