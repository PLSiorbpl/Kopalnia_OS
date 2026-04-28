BITS 32
global _start
global mb2_info
global mb2_magic_number

extern kernel_main
extern Kernel_64
extern gdt
extern gdt_descriptor
extern stack_top
extern tss

extern Paging
extern Elevate

section .multiboot2 ;multiboot_header
align 8
header_start:

align 8
dd 0xE85250D6       ; magic multiboot1 = 0x1BADB002 multiboot2 = 0xE85250D6
dd 0x0              ; flags
dd header_end - header_start
dd -(0xE85250D6 + 0 + (header_end - header_start))

; Framebuffer tag
dw 5
dw 0
dd 24
dd 800
dd 600
dd 32

align 8

; end tag
dw 0
dw 0
dd 8

header_end:

align 8
section .bss
mb2_magic_number:
    dd 0x0
mb2_info:
    dq 0x0

align 8
section .text
_start:
    cli

    mov [mb2_magic_number], eax
    mov [mb2_info], ebx

    call Elevate
.hang:
    hlt
    jmp .hang

section .note.GNU-stack noalloc noexec nowrite progbits