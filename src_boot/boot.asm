BITS 32
global _start
extern kernel_main
extern Kernel_64

extern Print
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
;dw 5
;dw 0
;dd 24
;dd 800
;dd 600
;dd 32

align 8

; end tag
dw 0
dw 0
dd 8

header_end:

align 8

section .text
_start:
    cli
    push eax ; Magic
    push ebx ; Mb2 Info

    call Elevate
.hang:
    hlt
    jmp .hang

section .note.GNU-stack noalloc noexec nowrite progbits